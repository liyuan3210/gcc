#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>  /*timer*/
#include <asm/uaccess.h>  /*jiffies*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Xie");
MODULE_DESCRIPTION("Timer Module");
MODULE_ALIAS("timer module");

static int times = 1;
module_param(times, int, S_IRUGO);
struct timer_list timer;

void timer_function(unsigned long para)
{
    static int count=1;
    if (count <= para){
        if (1==count)
            printk("<1>\nTimer counter begins:");
        printk("<1>count = %d", count);	
        mod_timer(&timer, jiffies + (HZ/2));
    }else{
        printk("<1>###");	
        printk("<1>Timer expired %ld(specified by para) times,\n"
                "and you should remove the module to destroy it\n", para);	
    }
    count++;
}

int __init timer_init(void)
{
	init_timer(&timer);
	timer.data = times;
	timer.expires = jiffies + (HZ/2);
	timer.function = timer_function;
	add_timer(&timer);
	
	return 0;
}

void __exit timer_exit(void)
{
	del_timer(&timer);
    printk("<1>Timer has been destroyed\n");	
}

module_init(timer_init);
module_exit(timer_exit);
