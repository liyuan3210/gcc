#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#define MEMDEV_SIZE 4096

int main(void)
{
	int fd, ret;
	char buf[MEMDEV_SIZE];
    char *pmdev;

    if (-1==(fd=open ("/dev/memdev0", O_RDWR))) {
        printf("open dev0 error\n");
        _exit(EXIT_FAILURE);
    }

    printf("\nTest for mmap(): ...\n");

    //sleep(30);
    pmdev = (char*)mmap(NULL, MEMDEV_SIZE, PROT_READ|PROT_WRITE, 
                        MAP_SHARED, fd, 0);
    printf("mmap() return: pmdev=%p\n", pmdev);
    sleep(20);
    printf("begin to use pmdev ...\n");
    bzero(pmdev, MEMDEV_SIZE);
    strcpy(buf, "Test for mmap() -- write by write()");
    printf("[use write()] write to mdev: %s\n", buf);
    ret = write(fd, buf, strlen(buf));
    printf("[use pmdev] content in mdev: %s\n", pmdev);
    
    bzero(buf, MEMDEV_SIZE);
    strcpy(buf, "write through pmdev, read by read()");
    printf("[use pmdev] store to mdev(offset=%d): %s\n", ret, buf);
    strcat(pmdev, buf);
    ret = read(fd, buf, strlen(buf));
    buf[ret] = 0;
    printf("[use read()] read from mdev: %s\n", buf);

    //sleep(30);
	_exit(EXIT_SUCCESS);	
}
