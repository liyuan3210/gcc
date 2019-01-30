#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/stat.h>

MODULE_AUTHOR("luyuan");
MODULE_LICENSE("GPL");

ssize_t kobj_show(struct kobject *kobj,struct attribute *attr,char *buf);
ssize_t kobj_store(struct kobject *kobj,struct attribute *attr,const char *buf,size_t count);

struct attribute kobj_attr={
	.name = "kobj_config",
	.mode = 0777,
};
static struct attribute *def_attrs[]={
	&kobj_attr,
	NULL,
};
void kobj_release(struct kobject *kobj){
	printk("kobj_type release.\n");
}
struct sysfs_ops kobj_sysfs={
	.show = kobj_show,
	.store = kobj_store,
};
struct kobj_type ktype={
	.release= kobj_release,
	.sysfs_ops= &kobj_sysfs,
	.default_attrs=def_attrs,
};

ssize_t kobj_show(struct kobject *kobj,struct attribute *attr,char *buf){
	printk("at the kobj_show().\n");
	printk("attr name:%s\n",attr->name);
	sprintf(buf,"%s\n",attr->name);
	return strlen(attr->name)+2;
}

ssize_t kobj_store(struct kobject *kobj,struct attribute *attr,const char *buf,size_t count){
	printk("at the kobj_store().\n");
	printk("write :%s\n",buf);
	return count;
}

struct kobject kobj;
static int kobj_init(){
	printk("kobj kobj_init().\n");
	//init and register.
	kobject_init_and_add(&kobj,&ktype,NULL,"kobj");
	return 0;
}
static int kobj_exit(){
	printk("kobj kobj_exit().\n");
	kobject_del(&kobj);
	return 0;
}

module_init(kobj_init);
module_exit(kobj_exit);
