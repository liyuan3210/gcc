#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include "memdev.h"

static int mem_major = MEMDEV_MAJOR;
module_param(mem_major, int, S_IRUGO);

struct mem_dev *mem_devp; /*设备结构体指针*/
struct cdev cdev; 
struct class *mem_class;  /*设备的类*/
/*
 * 文件打开函数
 * */
int mem_open(struct inode *inode, struct file *filp)
{
    struct mem_dev *dev;
    /*获取次设备号*/
    int num = MINOR(inode->i_rdev);
    if (num >= MEMDEV_NR_DEVS) 
        return -ENODEV;
    dev = &mem_devp[num];
    /*将设备描述结构指针赋值给文件私有数据指针*/
    filp->private_data = dev;
    return 0; 
}

/*
 * 文件释放函数
 * */
int mem_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/*
 * 读函数
 * */
static ssize_t mem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
    unsigned int count = size;
    unsigned int pos = *ppos;
    int ret = 0;
    struct mem_dev *dev = filp->private_data; /*获得设备结构体指针*/
    
    if (pos >= MEMDEV_SIZE) 
        return 0;
    if (count > MEMDEV_SIZE-pos) 
        count = MEMDEV_SIZE-pos;

    if(down_interruptible(&dev->sem))
        return -ERESTARTSYS;
    
    /*读数据到用户空间*/
    if (copy_to_user(buf, (void*)(dev->data + pos), count)) {
        ret =  - EFAULT;
    } else {
        *ppos += count;
        ret = count;
    }
    
    up(&dev->sem);    
    return ret;
}

/*
 * 写函数
 * */
static ssize_t mem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    unsigned int count = size;
    unsigned int pos = *ppos;
    int ret = 0;
    struct mem_dev *dev = filp->private_data; /*获得设备结构体指针*/

    if (pos >= MEMDEV_SIZE) 
        return 0;
    if (count > MEMDEV_SIZE-pos) 
        count = MEMDEV_SIZE-pos;

    if(down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    /*从用户空间写入数据*/
    if (copy_from_user(dev->data + pos, buf, count)) {
        ret =  - EFAULT;
    } else {
        *ppos += count;
        ret += count;
    }
    
    up(&dev->sem);    
    return ret;
}

/* 
 * seek文件定位函数 
 * */
static loff_t mem_llseek(struct file *filp, loff_t offset, int whence)
{ 
    loff_t newpos;

    switch(whence) {
        case 0: /* SEEK_SET */
            newpos = offset;
            break;
        case 1: /* SEEK_CUR */
            newpos = filp->f_pos + offset;
            break;
        case 2: /* SEEK_END */
            newpos = MEMDEV_SIZE -1 + offset;
            break;
        default: /* can't happen */
            return -EINVAL;
    }
    if ((newpos<0) || (newpos>MEMDEV_SIZE))
        return -EINVAL;
    filp->f_pos = newpos;
    return newpos;
}

/* 
 * mmap操作
 * */
//#define NORM_MMAP
#ifdef NORM_MMAP
static int mem_mmap(struct file *filp, struct vm_area_struct *vma)
{ 
    struct mem_dev *dev = filp->private_data; /*获得设备结构体指针*/
    unsigned long pfn  = (unsigned long)(virt_to_phys(dev->data)>>PAGE_SHIFT);
    unsigned long size = (unsigned long)(vma->vm_end - vma->vm_start);
  
    printk(KERN_INFO "\n[before remap: vm_pgoff = 0x%lx]\n", vma->vm_pgoff);
    printk(KERN_INFO "[pfn = 0x%lx]\n", pfn);
    printk(KERN_INFO "[vm_start = 0x%lx]\n", vma->vm_start);
    printk(KERN_INFO "[vm_end = 0x%lx]\n", vma->vm_end);
    printk(KERN_INFO "[size = %ld]\n", size);
    
    if (remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot))
        return -EAGAIN;
    printk(KERN_INFO "[after remap: vm_pgoff = 0x%lx]\n", vma->vm_pgoff);
    return 0;
}
#else
/*
 * page fault操作，替代了过去的nopage方法
 * */
static int mem_vma_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
    struct page *page;
    unsigned long pfn = vma->vm_pgoff; 
    printk(KERN_INFO "[page fault!]\n");
    
    /* 针对不同设备，缺页异常时page的获得方式不同，这里我们只是
     * 由memdev数据区对应的页帧（当然是在mmap()中传来的）通过
     * pfn_to_page获得。这也正是我们需要map到用户空间的内存*/
    page = pfn_to_page(pfn); 
    if (!page)
        return VM_FAULT_SIGBUS;
    get_page(page); /*获得该页，并增加页的使用计数*/
    /* 过去的nopage方法需要将获得的页描述符返回，而fault方法中
     * 则要将页描述符赋值给vmf的page成员*/
    vmf->page = page;
    
    return 0;
}

static struct vm_operations_struct mem_vm_ops = 
{
    .fault = mem_vma_fault
};

static int mem_mmap(struct file *filp, struct vm_area_struct *vma)
{ 
    struct mem_dev *dev = filp->private_data; 
    unsigned long phaddr  = (unsigned long)virt_to_phys(dev->data);
    
    vma->vm_pgoff = phaddr>>PAGE_SHIFT;
    if (phaddr > __pa(high_memory) || (filp->f_flags &O_SYNC))
        vma->vm_flags |= VM_IO;
    printk("<1> high_memory = %p\n", high_memory);
    vma->vm_flags |= VM_RESERVED;
    vma->vm_ops = &mem_vm_ops;

    return 0;
}

#endif
/*
 * 文件操作结构体
 * */
static const struct file_operations mem_fops =
{
    .owner = THIS_MODULE,
    .read = mem_read,
    .write = mem_write,
    .llseek = mem_llseek,
    .mmap = mem_mmap,
    .open = mem_open,
    .release = mem_release,
};

/*
 * 设备驱动模块加载函数
 * */
static int memdev_init(void)
{
    int result;
    int i;

    dev_t devno = MKDEV(mem_major, 0);

    /* 静态申请设备号*/
    if (mem_major)
        result = register_chrdev_region(devno, 2, "memdev");
    else { /* 动态分配设备号 */
        result = alloc_chrdev_region(&devno, 0, 2, "memdev");
        mem_major = MAJOR(devno);
    }  

    if (result < 0)
        return result;

    /*初始化cdev结构*/
    cdev_init(&cdev, &mem_fops);
    cdev.owner = THIS_MODULE;
    cdev.ops = &mem_fops;

    /* 注册字符设备 */
    cdev_add(&cdev, MKDEV(mem_major, 0), MEMDEV_NR_DEVS);

    /* 为设备描述结构分配内存*/
    mem_devp = kmalloc(MEMDEV_NR_DEVS*sizeof(struct mem_dev), GFP_KERNEL);
    if (!mem_devp)    /*申请失败*/
    {
        result =  - ENOMEM;
        goto fail_malloc;
    }
    memset(mem_devp, 0, sizeof(struct mem_dev));

    /*在自动创建设备节点之前，先创建sysfs下对应的类*/
    mem_class = class_create(THIS_MODULE, "mem_class");
    /*初始化设备结构*/
    for (i=0; i < MEMDEV_NR_DEVS; i++)  {
        mem_devp[i].size = MEMDEV_SIZE;
        mem_devp[i].data = kmalloc(MEMDEV_SIZE, GFP_KERNEL);
        memset(mem_devp[i].data, 0, MEMDEV_SIZE);
        /*初始化信号量*/
        sema_init(&mem_devp[i].sem, 1);
        /*在sysfs中注册该设备，这将引起udev/mdev创建对应的设备节点*/
        device_create(mem_class, NULL, MKDEV(mem_major, i), NULL, "memdev%d", i);
    }
    return 0;
fail_malloc: 
    unregister_chrdev_region(devno, 1);
    return result;
}

/*模块卸载函数*/
static void memdev_exit(void)
{
    int i;
    for (i=0; i < MEMDEV_NR_DEVS; i++)  {
        kfree(mem_devp[i].data) ;
        /*删除在/dev下的设备节点*/
        device_destroy(mem_class, MKDEV(mem_major, i));
    }
    cdev_del(&cdev);   /*注销设备*/
    class_destroy(mem_class);  /*删除类*/
    kfree(mem_devp);     /*释放设备结构体内存*/
    unregister_chrdev_region(MKDEV(mem_major, 0), 2); /*释放设备号*/
}

MODULE_AUTHOR("www.enjoylinux.cn");
MODULE_LICENSE("GPL");

module_init(memdev_init);
module_exit(memdev_exit);
