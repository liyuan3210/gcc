#include <linux/module.h>
#include <linux/init.h>

//模块入口函数
//__init:表示代码段中的子段,里面的内容只运行一次并且回收内存.
static int __init hello_init(void)
{
	printk(KERN_EMERG "hello world!\n");
	return 0;
}
//模块卸载函数
//__exit:
static void __exit hello_exit(void)
{
	printk(KERN_EMERG "hello exit!\n");
}
//内核符号导出 函数
int add_integar(int a,int b)
{
	return a+b; 
}
int sub_integar(int a,int b)
{
	return a-b; 
}

module_init(hello_init);
module_exit(hello_exit);
//函数导出
EXPORT_SYMBOL(add_integar);
EXPORT_SYMBOL(sub_integar);
