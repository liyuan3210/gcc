#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>

MODULE_AUTHOR("luyuan");
MODULE_LICENSE("GPL");

static char *Version="$Revision:1.0$";

static void my_bus_release(struct device *dev){
	printk("release my_bus_release!");
}

static int my_match(struct device *dev,struct device_driver *driver){
	//return !strncmp(dev->bus_id,driver->name,strlen(driver->name));
	//return !strncmp(dev->init_name,driver->name,strlen(driver->name));
	printk("bus_base match my_match.");
	return !0;
}

struct device my_bus_device={
	//.bus_id="my_bus0",
	.init_name="my_bus0",
	.release=my_bus_release,
};
	
struct bus_type my_bus={
	.name="my_bus",
	.match=my_match,
};

EXPORT_SYMBOL(my_bus_device);
EXPORT_SYMBOL(my_bus);

static ssize_t show_bus_version(struct bus_type *bus,char *buf){
	return snprintf(buf,PAGE_SIZE,"%s\n",Version);
}

static BUS_ATTR(version,0777,show_bus_version,NULL);

static int bus_base_init(){
	int ret;
	ret=bus_register(&my_bus);
	
	if(bus_create_file(&my_bus,&bus_attr_version))
		printk("create bus file fail!\n");

	ret=device_register(&my_bus_device);

	if(ret)
		printk("register device fail!\n");
	
	return ret;
}

static void bus_base_exit(){
	device_unregister(&my_bus_device);
	bus_unregister(&my_bus);
}

module_init(bus_base_init);
module_exit(bus_base_exit);
