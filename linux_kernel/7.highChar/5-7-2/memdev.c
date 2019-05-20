#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include "memdev.h"

static int mem_major = MEMDEV_MAJOR;
module_param(mem_major, int, S_IRUGO);

struct mem_dev *mem_devp; /*设备结构体指针*/
struct cdev cdev; 

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
    dev->nattch++;
    return 0; 
}

/*
 * 文件释放函数
 * */
int mem_release(struct inode *inode, struct file *filp)
{
    struct mem_dev *dev = filp->private_data; /*获得设备结构体指针*/
    dev->nattch--;
    if (0==dev->nattch) {
        /*使读写位置标志重新指到开头, 并修改可读可写标志*/
        dev->rpos = 0;
        dev->wpos = 0;
        dev->canRead  = false; 
        dev->canWrite = true;  
        /*printk("<1> [release and renew dev]");*/
    }
    return 0;
}

/*
 * 读函数
 * */
static ssize_t mem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
    unsigned int count;
    int ret = 0;
    struct mem_dev *dev = filp->private_data; /*获得设备结构体指针*/

    /*printk("<1> [Before mem_read: rpos=%lu, wpos=%lu, canR=%d, canW=%d]\n", 
            dev->rpos, dev->wpos, dev->canRead, dev->canWrite);*/
    /* 这里使用while的好处是可以保证是因为有数据可读而跳出的，但
     * 使用while又带来了另外一个问题：无法通过中断信号跳出循环*/
    if (!dev->canRead) {
        if (filp->f_flags & O_NONBLOCK) {
            return -EAGAIN;
        }
        wait_event_interruptible(dev->rwq, dev->canRead);
    }
    if(down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    if (dev->rpos < dev->wpos) {
        count = dev->wpos - dev->rpos;
        count = count>size ? size : count;
    } else {
        count = MEMDEV_SIZE - dev->rpos - 1;
        if (count >= size) {
            count = size;    
        } else {
            if (copy_to_user(buf, (void*)(dev->data + dev->rpos), count)) 
                ret =  - EFAULT;
            ret += count;
            dev->rpos = 0;
            buf  += count;
            size -= count;
            count = dev->wpos>size ? size: dev->wpos;
        }
    }
    /*读数据到用户空间*/
    if (copy_to_user(buf, (void*)(dev->data + dev->rpos), count)) {
        ret =  - EFAULT;
    } else {
        dev->rpos += count;
        ret += count;
    }
    
    if (ret) {
        dev->canWrite = true;        /*有空间可写*/
        wake_up(&(dev->wwq));   /*唤醒写等待队列*/
        if (dev->rpos==dev->wpos)
            dev->canRead = false;    /*无数据可读*/
    }
    /*printk("<1> [After mem_read: rpos=%lu, wpos=%lu, canR=%d, canW=%d]\n", 
            dev->rpos, dev->wpos, dev->canRead, dev->canWrite);*/
    up(&dev->sem);    
    return ret;
}

/*
 * 写函数
 * */
static ssize_t mem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    unsigned int count;
    int ret = 0;
    struct mem_dev *dev = filp->private_data; /*获得设备结构体指针*/

    /*printk("<1> [Before mem_write: rpos=%lu, wpos=%lu, canR=%d, canW=%d]\n", 
            dev->rpos, dev->wpos, dev->canRead, dev->canWrite);*/

    /* 没有空间可写，则进入睡眠 */
    if (!dev->canWrite) {
        if (filp->f_flags & O_NONBLOCK) {
            return -EAGAIN;
        }
        wait_event_interruptible(dev->wwq, dev->canWrite);
    }
    if(down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    if (dev->rpos > dev->wpos) {
        count = dev->rpos - dev->wpos;
        count = count>size ? size : count;
    } else {
        count = MEMDEV_SIZE - dev->wpos - 1;
        if (count >= size) {
            count = size;    
        } else {
            if (copy_from_user(dev->data + dev->wpos, buf, count)) 
                ret =  - EFAULT;
            ret += count;
            dev->wpos = 0;
            buf  += count;
            size -= count;
            count = dev->rpos>size ? size: dev->rpos;
        }
    }

    /*从用户空间写入数据*/
    if (copy_from_user(dev->data + dev->wpos, buf, count)) {
        ret =  - EFAULT;
    } else {
        dev->wpos += count;
        ret += count;
    }
    
    if (ret) {
        dev->canRead = true;         /*有空间可写*/
        wake_up(&(dev->rwq));   /*唤醒读等待队列*/
        if (dev->rpos==dev->wpos)
            dev->canWrite = false;    /*无数据可写*/
    }
    /*printk("<1> [After mem_write: rpos=%lu, wpos=%lu, canR=%d, canW=%d]\n", 
            dev->rpos, dev->wpos, dev->canRead, dev->canWrite);*/
    up(&dev->sem);    
    return ret;
}

/*
 * 文件操作结构体
 * */
static const struct file_operations mem_fops =
{
    .owner = THIS_MODULE,
    .read = mem_read,
    .write = mem_write,
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
    mem_devp = kmalloc(MEMDEV_NR_DEVS * sizeof(struct mem_dev), GFP_KERNEL);
    if (!mem_devp)    /*申请失败*/
    {
        result =  - ENOMEM;
        goto fail_malloc;
    }
    memset(mem_devp, 0, sizeof(struct mem_dev));

    /*为设备分配内存*/
    for (i=0; i < MEMDEV_NR_DEVS; i++)  {
        mem_devp[i].size = MEMDEV_SIZE;
        mem_devp[i].data = kmalloc(MEMDEV_SIZE, GFP_KERNEL);
        memset(mem_devp[i].data, 0, MEMDEV_SIZE);

        mem_devp[i].canRead  = false; /*一开始设备没有数据可供读*/
        mem_devp[i].canWrite = true;  /*一开始设备有空间可供写*/
        /*初始化读写指针*/
        mem_devp[i].rpos = 0;
        mem_devp[i].wpos = 0;
        /*初始化等待队列*/
        init_waitqueue_head(&(mem_devp[i].rwq));
        init_waitqueue_head(&(mem_devp[i].wwq));
        /*设备文件被打开的次数*/
        mem_devp[i].nattch = 0;
        /*初始化信号量*/
        sema_init(&mem_devp[i].sem, 1);
    }
    return 0;
fail_malloc: 
    unregister_chrdev_region(devno, 1);
    return result;
}

/*模块卸载函数*/
static void memdev_exit(void)
{
    cdev_del(&cdev);   /*注销设备*/
    kfree(mem_devp);     /*释放设备结构体内存*/
    unregister_chrdev_region(MKDEV(mem_major, 0), 2); /*释放设备号*/
}

MODULE_AUTHOR("www.enjoylinux.cn");
MODULE_LICENSE("GPL");

module_init(memdev_init);
module_exit(memdev_exit);
