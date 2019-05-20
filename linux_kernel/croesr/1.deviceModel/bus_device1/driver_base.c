#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>

MODULE_AUTHOR("luyuan");
MODULE_LICENSE("GPL");

extern struct device my_bus_device;
extern struct bus_type my_bus;

static int my_probe(struct device *dev){
	printk("my_probe.\n");
	return 0;
}

static int my_remove(struct device *dev){
	printk("my_remove\n");
	return 0;
}

struct device_driver my_drv={
	.name="my_dev",
	.bus=&my_bus,
	.probe=my_probe,
	.remove=my_remove,
};

static ssize_t mydrv_show(struct device_driver *driver,char *buf){
	return sprintf(buf,"%s\n","this is my driver!\n");
}

static DRIVER_ATTR(drv,0777,mydrv_show,NULL);

static int driver_base_init(){
	int ret=0;
	
	driver_register(&my_drv);
	
	driver_create_file(&my_drv,&driver_attr_drv);
	
	return ret;
}

static void driver_base_exit(){
	
	driver_unregister(&my_drv);
	
}

module_init(driver_base_init);
module_exit(driver_base_exit);
