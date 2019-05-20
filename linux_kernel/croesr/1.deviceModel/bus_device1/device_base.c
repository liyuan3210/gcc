#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>

MODULE_AUTHOR("luyuan");
MODULE_LICENSE("GPL");

extern struct device my_bus_device;
extern struct bus_type my_bus;

static void my_dev_release(struct device *dev){

}

struct device my_dev={
	.bus=&my_bus,
	.parent=&my_bus_device,
	.release=my_dev_release,
};

static ssize_t mydev_show(struct device *dev,char *buf){
	return sprintf(buf,"%s\n","this is my device!");
}

static DEVICE_ATTR(dev,0777,mydev_show,NULL);

static int device_base_init(){
	int ret=0;
	
	//strncpy(my_dev.init_name,"my_dev",strlen("my_dev"));
	my_dev.init_name="my_dev";

	device_register(&my_dev);
	
	device_create_file(&my_dev,&dev_attr_dev);
	
	return ret;
}

static void device_base_exit(){

	device_unregister(&my_dev);
	
}

module_init(device_base_init);
module_exit(device_base_exit);
