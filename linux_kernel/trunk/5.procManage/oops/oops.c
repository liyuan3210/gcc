#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h>

void func_D(void)
{
    int *p = NULL;
    int a = 6;
    printk("Function D\n");
    *p = a+5;	
}

void func_C(void)
{
    printk("Function C\n");
    func_D();	
}

void func_B(void)
{
    printk("Function B\n");
    func_C();	
}

void func_A(void)
{
    printk("Function A\n");
    func_B();	
}

int oops_init(void)
{
    printk("oops init\n");
    func_A();
    return 0;
}

void oops_exit(void)
{
	printk("oops exit!\n");
}

module_init(oops_init);
module_exit(oops_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("www.enjoylinux.cn");

