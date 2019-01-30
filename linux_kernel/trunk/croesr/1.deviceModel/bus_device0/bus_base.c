#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>

MODULE_AUTHOR("luyuan");
MODULE_LICENSE("GPL");

static char *Version="$Revision:1.0$";

static int my_match(struct device *dev,struct device_driver *driver){
	//return !strncmp(dev->bus_id,driver->name,strlen(driver->name));
	return !strncmp(dev->init_name,driver->name,strlen(driver->name));
}
	
struct bus_type my_bus={
	.name="my_bus",
	.match=my_match,
};

static ssize_t show_bus_version(struct bus_type *bus,char *buf){
	return snprintf(buf,PAGE_SIZE,"%s\n",Version);
}

static BUS_ATTR(version,0777,show_bus_version,NULL);

static int bus_base_init(){
	int ret;
	ret=bus_register(&my_bus);
	
	if(bus_create_file(&my_bus,&bus_attr_version))
	printk("create bus file fail!\n");
	
	return ret;
}

static void bus_base_exit(){
	bus_unregister(&my_bus);
}

module_init(bus_base_init);
module_exit(bus_base_exit);
