#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
	int fd, ret;
	char buf[256];
    printf("Two processes are writing ...\n");

    if (-1==(ret=fork())) {
        printf("fork() error\n");
        _exit(EXIT_FAILURE);
    } else if (0 == ret) {   /*child process*/ 
        if (-1==(fd=open ("/dev/memdev0", O_RDWR))) {
            printf("open() error");
            _exit(EXIT_FAILURE);
        }
        strcpy(buf, "Here is the CHILD writing ...");
        printf("Child write: %s\n", buf);
        write(fd, buf, strlen(buf)); 
        close(fd);
        _exit(EXIT_SUCCESS);
    } else {                 /*father process*/
        if (-1==(fd=open ("/dev/memdev0", O_RDWR))) {
            printf("open() error");
            _exit(EXIT_FAILURE);
        }
        strcpy(buf, "The semaphore is nesessory in the memdev driver ...");
        printf("Father write: %s\n", buf);
        write(fd, buf, strlen(buf)); 
        close(fd);
        wait(NULL); 
    }
    
    if (-1==(fd=open ("/dev/memdev0", O_RDWR))) {
        printf("open() error");
        _exit(EXIT_FAILURE);
    }
    memset(buf, 0, sizeof(buf));
    read(fd, buf, sizeof(buf));
    printf ("At last, the content in memdev is: %s\n", buf);

	close(fd);
	return 0;	
}
