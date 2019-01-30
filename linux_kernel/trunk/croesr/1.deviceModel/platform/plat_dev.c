#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/platform_device.h>

MODULE_AUTHOR("luyuan");
MODULE_LICENSE("GPL");

static struct platform_device *my_dev; 

static int plat_dev_init(){
	int ret=0;

	my_dev=platform_device_alloc("my_dev",-1);

	ret=platform_device_add(my_dev);
	
	if(ret)
  		platform_device_put(my_dev);
	
	return ret;
}

static void plat_dev_exit(){
	platform_device_unregister(my_dev);
}

module_init(plat_dev_init);
module_exit(plat_dev_exit);
