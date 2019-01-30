#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MEMDEV_SIZE 4096

int main(void)
{
	int fd, ret;
	char buf[MEMDEV_SIZE];
    printf("One processes read, Another write ...\n");

    if (-1==(ret=fork())) {
        printf("fork() error\n");
        _exit(EXIT_FAILURE);
    } else if (0 == ret) {   /*child process*/ 
        if (-1==(fd=open ("/dev/memdev0", O_RDWR))) {
            printf("open() error\n");
            _exit(EXIT_FAILURE);
        }
        printf("Child sleep 5s ...\n\n");
        sleep(5);
        strcpy(buf, "Here is the CHILD writing ...");
        ret = write(fd, buf, strlen(buf)); 
        printf("Child write1(%d bytes): %s\n", strlen(buf), buf);
        sleep(5);
        printf("Child sleep 10s ...\n\n");
        sleep(10);
        ret = write(fd, buf, strlen(buf)); 
        printf("Child write2(%d bytes): %s\n", ret, buf);
        sleep(5);
        ret = write(fd, buf, sizeof(buf)); 
        printf("Child write3(%d bytes -- buf is full)\n", ret);
        printf("Child try to write4, but there is no space, blocking ...\n");
        ret = write(fd, buf, strlen(buf)); 
        buf[ret] = 0;
        usleep(30000);
        printf("After Father read4, Child write4(%d bytes): %s\n", ret, buf);

        close(fd);
        _exit(EXIT_SUCCESS);
    } else {                 /*father process*/
        if (-1==(fd=open ("/dev/memdev0", O_RDWR))) {
            printf("open() error");
            _exit(EXIT_FAILURE);
        }
        printf("Father try to read, but there is no data, blocking ...\n");
        int ret=0;
        ret = read(fd, buf, 20);
        usleep(30000);
        printf("After Child write1, Father can read:\n");
        buf[ret] = 0;
        printf("Father read1(%d bytes): %s\n", ret, buf);
        ret = read(fd, buf, 20);
        buf[ret] = 0;
        printf("Father read2(%d bytes): %s\n", ret, buf);
        printf("Father read3, no data to read, blocking ...\n");
        ret = read(fd, buf, 50);
        usleep(30000);
        buf[ret] = 0;
        printf("After Child write2, Father read3(%d bytes): %s\n", ret, buf);
        printf("Father sleep 10s ...\n\n");
        sleep(10);
        ret = read(fd, buf, 20);
        printf("Father read4(%d bytes)\n", ret);
        wait(NULL); 
        close(fd);
    }
	_exit(EXIT_SUCCESS);	
}
