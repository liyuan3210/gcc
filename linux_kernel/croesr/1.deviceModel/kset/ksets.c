#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h> 

MODULE_AUTHOR("luyuan");
MODULE_LICENSE("GPL");

struct kset kset_p;
struct kset kset_c;

int kset_filter(struct kset *kset,struct kobject *kobj){
	printk("filter: obj %s.\n",kobj->name);
	return 1;
}

const char *kset_name(struct kset *kset,struct kobject *kobj){
	static char buf[20];
	printk("name :obj %s.\n",kobj->name);
	sprintf(buf,"%s","kset_name");
	return buf;
}

int kset_uevent(struct kset *kset,struct kobject *kobj,struct kobj_uevent_env *env){
	int i=0;
	printk("at the kset_uevent(),name:%s \n",kobj->name);
	while(i<env->envp_idx){
		printk("at while %s .\n",env->envp[i]);
		i++;
	}
	
}

struct kset_uevent_ops uevent_ops = {
	.filter = kset_filter,
	.name = kset_name,
	.uevent = kset_uevent,
};

struct kobj_type kobj_t; 

static int ksets_init(){
	printk("kobj kobj_init().\n");
	kobject_set_name(&kset_p.kobj,"kset_p");
	kset_p.uevent_ops = &uevent_ops;
	
	kset_p.kobj.ktype=&kobj_t;
	
	kset_register(&kset_p);
////////////////////////////////////////////////
	

	kobject_set_name(&kset_c.kobj,"kset_c");
	kset_c.kobj.kset=&kset_p;
	kset_c.kobj.ktype=&kobj_t;
	kset_register(&kset_c);

	return 0;
}
static int ksets_exit(){
	printk("kobj kset_exit().\n");
	kset_unregister(&kset_p);
	kset_unregister(&kset_c);
	return 0;
}

module_init(ksets_init);
module_exit(ksets_exit);
