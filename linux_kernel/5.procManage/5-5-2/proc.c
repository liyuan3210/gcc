#include <linux/kernel.h>  
#include <linux/module.h>  
#include <linux/proc_fs.h>  
#include <linux/init.h>  
#include <asm/uaccess.h>  
#include <linux/sched.h>  

MODULE_LICENSE("GPL");
MODULE_AUTHOR("www.enjoylinux.cn");
MODULE_DESCRIPTION("Proc Module");
MODULE_ALIAS("proc module");

#define MAX_LENGTH 	64  
#define MAX_RECORD  5 	 
#define F_NAME 	"pid"  
#define D_NAME 	"tst"  
#define P_NAME 	"/proc/" D_NAME "/" F_NAME 

typedef struct {
    pid_t pid;
    char comm[TASK_COMM_LEN];
} pidcomm_t;
static int bufPos = 0;
pidcomm_t pidcommBuf[MAX_RECORD];

static struct proc_dir_entry *pid_entry;  
static struct proc_dir_entry *dir_entry;  

static int  pid_write (struct file *file, const char __user *buffer, 
                        unsigned long count, void *data)  
{     
	struct task_struct *_process;  
    char pid_buffer[MAX_LENGTH];
	unsigned long pid_buffer_size = count;  

    if (pid_buffer_size > MAX_LENGTH)  
 	    pid_buffer_size = MAX_LENGTH;   
    
    memset(pid_buffer, 0, MAX_LENGTH);
 	if (copy_from_user (pid_buffer, buffer, pid_buffer_size))  
  		return -EFAULT;  
  	
 	for_each_process (_process) {  
  		char _tmp[MAX_LENGTH];  
 		sprintf (_tmp, "%d\n", _process->pid);  
 		if (!strcmp (_tmp, pid_buffer)) {
  			printk ("<1> The EXE file is: %20s \n", _process->comm);  
            pidcommBuf[bufPos].pid = _process->pid;
            memset(pidcommBuf[bufPos].comm, 0, TASK_COMM_LEN);
            strcpy(pidcommBuf[bufPos].comm, _process->comm);
            bufPos = bufPos==MAX_RECORD-1 ? 0: bufPos+1;
        }
 	}  
 	return pid_buffer_size;  
}  
    
static int pid_read (char *page, char **start, off_t off, 
                        int count, int *eof, void *data)  
{  
    int i, ret, len = 0;
    char *pos = page;
    
    if (off > 0) {
        *eof = 1;
        return 0;
    }

    ret = sprintf(pos, "PID\tCOMMAND\n");
    len += ret;
    pos += ret;
    for (i=0; i<MAX_RECORD; i++) {
        if (0==pidcommBuf[i].pid) 
            break;
        ret = sprintf(pos, "%d\t%s\n", pidcommBuf[i].pid, 
                                        pidcommBuf[i].comm);
        len += ret;
        pos += ret;
    }
 	return len;  
}  

static int __init  pid_init (void)  
{  
	dir_entry = proc_mkdir(D_NAME, NULL);
 	if (!dir_entry) {
 		printk(KERN_ERR "Can't create /proc/" D_NAME "\n");
   		return -ENOMEM;
    }
 	pid_entry = create_proc_entry (F_NAME, 0666, dir_entry);  
 	if (!pid_entry) {  
  		remove_proc_entry (F_NAME, dir_entry);  
		printk (KERN_ERR P_NAME " created failed\n");  
  		return -ENOMEM;  
 	}  
	pid_entry->read_proc = pid_read;  
 	pid_entry->write_proc = pid_write;  
  	printk ("<1> create " P_NAME " SUCCESSFULLY.\n");  
     
  	return 0;  
}  
 
static void __exit pid_exit (void)  
{  
	remove_proc_entry (F_NAME, dir_entry);  
	remove_proc_entry (D_NAME, NULL);  
 	printk ("<1> Exit.\n");    
}  
   
module_init (pid_init);  
module_exit (pid_exit);  
