#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h> 

MODULE_LICENSE("Dual BSD/GPL"); 

/*
 * struct kset {  
* struct subsystem * subsys;         所在的subsystem的指针  
* struct kobj_type * ktype;             指向该kset对象类型描述符的指针  
* struct list_head list;                       用于连接该kset中所有kobject的链表头  
* struct kobject kobj;                                         嵌入的kobject  
* struct  kset_uevent_ops *uevent_ops;     指向热插拔操作表的指针  
* }; 
*/
struct kset my_kset_parent;
struct kset my_kset_child; 

int my_kset_filter(struct kset *kset,struct  kobject *kobj)
{
        printk("===my_kset_filter===\n");
        return 1;                                            //返回1事事件可以上报
}
const char *my_kset_name(struct kset *kset, struct kobject *kobj )
{
        static char buf[200];                        //可以动态申请
        printk("===my_kset_name is %s===\n", kobj->name);
        sprintf(buf,"%s","what's this mean"); 

        return buf;
} 

int my_kset_uevent(struct kset *kset, struct kobject *kobj, struct kobj_uevent_env *env)
{
        int i = 0;
        printk("===my_kset_uevent===\n");
        while(i<env->envp_idx)
        {
                printk("%s\n",env->envp[i++]);
        }
        return 0;
} 

/*
 * struct kset_uevnt_ops{  
* int (*filter)(struct kset *kset,struct  kobject *kobj);  
* const char *(*name)(struct kset *kset, struct kobject *kobj );  
* int (*uevent)(struct kset *kset,struct  kobject *kobj，struct kobj_uevent *env);  
* } 
*/
struct kset_uevent_ops my_uevent_ops = 
{
        .filter = my_kset_filter,
        .name = my_kset_name,
        .uevent = my_kset_uevent,
}; 

struct kobj_type my_kobj_type ; 

static int __init my_kset_init(void)
{ 
        printk("===my_kset_init===\n");
        kobject_set_name(&my_kset_parent.kobj,"my_kset_parent");
        my_kset_parent.uevent_ops = &my_uevent_ops;
        /* int kset_register(struct kset *kset); */
 
        //下面这句多余，是为了规避kernel oops bug
        my_kset_parent.kobj.ktype = &my_kobj_type;
        kset_register(&my_kset_parent); 

        kobject_set_name(&my_kset_child.kobj,"my_kset_child");
        my_kset_child.kobj.kset = &my_kset_parent;
        my_kset_child.kobj.ktype = &my_kobj_type;
        kset_register(&my_kset_child); 

        return 0;
} 

static void __exit my_kset_exit(void)
{
        printk("===my_kset_exit===\n");
        kset_unregister(&my_kset_parent);
        kset_unregister(&my_kset_child);
} 

module_init(my_kset_init);
module_exit(my_kset_exit);
