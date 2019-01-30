#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/highmem.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("www.enjoylinux.cn");
MODULE_DESCRIPTION("Memory alloc test Module");
MODULE_ALIAS("malloc module");

unsigned char *pagemem;
unsigned char *pagezmem;
unsigned char *pagesmem;
unsigned char *kmallocmem;
unsigned char *vmallocmem;
#ifdef HMEMTST
struct page *allocpage;
unsigned char *kmapmem;
#endif

int __init alloc_init(void)
{
	pagemem = (unsigned char*)__get_free_page(GFP_KERNEL);
	printk("<1>get_free_page: pagemem va addr=%p  "
            "\tpa addr=%lx\n", pagemem, __pa(pagemem));//__pa(用来转换成物理地址)
	pagesmem = (unsigned char*)__get_free_pages(GFP_KERNEL, 3);
	printk("<1>get_free_pages: pagesmem va addr=%p"
            "\tpa addr=%lx\n", pagesmem, __pa(pagesmem));
    pagezmem = (unsigned char*)get_zeroed_page(GFP_KERNEL);
	printk("<1>get_zeroed_page: pagezmem va addr=%p"
            "\tpa addr=%lx\n", pagezmem, __pa(pagezmem));

 	kmallocmem = (unsigned char*)kmalloc(100, GFP_KERNEL);
    memset(kmallocmem, 0, 100);
	strcpy(kmallocmem, "<<< --- Kmalloc Mem OK! --- >>>");
 	printk("<1>kmalloc: kmallocmem va addr=%p  "
            "\tpa addr=%lx\n", kmallocmem, __pa(kmallocmem));
 	printk("<1>kmalloc: kmallocmem say %s\n", kmallocmem);

	vmallocmem = (unsigned char*)vmalloc(1000000);
	printk("<1>vmalloc: vmallocmem va addr=%p\n", vmallocmem);
#ifdef HMEMTST   
    allocpage = alloc_pages(__GFP_HIGHMEM|GFP_KERNEL, 0);
    if (!PageHighMem(allocpage))
        printk("<1> it's not highmem\n");
	printk("<1>page_address: va addr=%p\n", (unsigned char*)page_address(allocpage));
    kmapmem = (unsigned char*)kmap(allocpage);
	printk("<1>kmap: kmapmem va addr=%p\n", kmapmem);    
    kunmap(allocpage);    
    kmapmem = (unsigned char*)kmap_atomic(allocpage, 0);
	printk("<1>kmap_atomic: kmapmem va addr=%p\n", kmapmem);    
    kunmap_atomic(allocpage, 0);  
#endif
	return 0;
}
void __exit alloc_exit(void)
{
	free_page((int)pagemem);
	free_page((int)pagezmem);
	free_pages((int)pagesmem, 3);
	kfree(kmallocmem);
    vfree(vmallocmem);
#ifdef HMEMTST    
    __free_pages(allocpage, 0);
#endif
	printk("<1><<< --- Module Exit! --->>>\n");
}

module_init(alloc_init);
module_exit(alloc_exit);
