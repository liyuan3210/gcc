#include <linux/module.h>
#include <linux/init.h>
//模块可选信息
MODULE_LICENSE("GPL");//许可证声明
MODULE_AUTHOR("liyuan");//作者声明
MODULE_DESCRIPTION("This module is a param example.");//模块描述
MODULE_VERSION("V1.0");//模块别名
MODULE_ALIAS("a simple module");//模块别名

//模块参数
static char *name = "liyuan arg";
static int age = 30;
//S_IRUGO是参数权限，也可以用数字
module_param(age,int,S_IRUGO);
module_param(name,charp,S_IRUGO);


//使用外部文件函数
extern int add(int a,int b);


//声明 外部内核符号 函数
extern int add_integar(int a,int b);
extern int sub_integar(int a,int b);

static int __init mains_init(void)
{
	 //多文件编译

    printk(KERN_EMERG"param hi");
    int vle=add(1,2);
    printk(KERN_EMERG"add value:%d\n",vle);
	//模块参数

	 printk(KERN_EMERG" name : %s\n",name);
	 printk(KERN_EMERG" age : %d\n",age);

	//使用其他模块的函数(内核符号导出)
	int adds=add_integar(3,1);
	int subs=sub_integar(3,1);
	printk(KERN_EMERG" add_integar : %d\n",adds);
	printk(KERN_EMERG" sub_integar : %d\n",subs);
    return 0;
}

static void __exit mains_exit(void)
{
    printk("param exit!");
}

module_init(mains_init);
module_exit(mains_exit);
