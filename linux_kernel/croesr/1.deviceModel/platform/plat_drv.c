#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/platform_device.h>

MODULE_AUTHOR("luyuan");
MODULE_LICENSE("GPL");

static int my_probe(struct device *dev){
	printk("my_probe !\n");
	return 0;
}

static int my_remove(struct device *dev){
	printk("my_remove !\n");
	return 0;
}

static struct platform_driver my_drv={
	.probe=my_probe,
	.remove=my_remove,
	.driver={
		.owner=THIS_MODULE,
		.name="my_dev",
	},
} ;

static int plat_drv_init(){
	return platform_driver_register(&my_drv);
}

static void plat_drv_exit(){
	platform_driver_unregister(&my_drv);
}

module_init(plat_drv_init);
module_exit(plat_drv_exit);
