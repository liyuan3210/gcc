#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>

#define procfs_name "proctest"

struct proc_dir_entry *Our_Proc_File;

int procfile_read(char *buffer,char **buffer_location,off_t offset,
                  int buffer_length,int *eof,void *data){
	int ret;
	ret=sprintf(buffer,"Hello World!\n");
	return ret;
}

//模块入口函数
static int __init proc1_init(void)
{
	Our_Proc_File=create_proc_entry(procfs_name,0644,NULL);
	if(Our_Proc_File==NULL){
		remove_proc_entry(procfs_name,NULL);
		printk(KERN_EMERG "/proc/%s removed!\n",procfs_name);
		return -ENOMEM;
	}
	Our_Proc_File->read_proc=procfile_read;
	Our_Proc_File->owner=THIS_MODULE;
	Our_Proc_File->mode=S_IFREG|S_IRUGO;
	Our_Proc_File->uid=0;
	Our_Proc_File->gid=0;
	Our_Proc_File->size=37;
	printk(KERN_EMERG "/proc/%s removed!\n",procfs_name);
	return 0;
}
//模块卸载函数
static void __exit proc1_exit(void)
{
	remove_proc_entry(procfs_name,NULL);
	printk(KERN_EMERG "/proc/%s removed!\n",procfs_name);
}

module_init(proc1_init);
module_exit(proc1_exit);
