
#ifndef _MEMDEV_H_
#define _MEMDEV_H_

#ifndef MEMDEV_MAJOR
#define MEMDEV_MAJOR 251   /**/
#endif

#ifndef MEMDEV_NR_DEVS
#define MEMDEV_NR_DEVS 2    /**/
#endif

#ifndef MEMDEV_SIZE
#define MEMDEV_SIZE 4096
#endif

/*mem*/
struct mem_dev                                     
{     
    bool canRead;   /*设备可读标识*/
    bool canWrite;  /*设备可写标识*/                                                   
    char *data;                      
    unsigned long size; 
    unsigned long rpos; /*读定位标识*/
    unsigned long wpos; /*写定位标识*/
    unsigned long nattch;
    wait_queue_head_t rwq;
    wait_queue_head_t wwq;      
    struct semaphore sem; /*抢占式内核时需要添加*/ 
};

#endif /* _MEMDEV_H_ */
