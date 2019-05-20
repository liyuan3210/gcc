#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <time.h>

#define MEMDEV_SIZE 4096

int main(void)
{
	int fd1, fd2, maxfdp, ret;
    fd_set rfds, wfds; 
    struct timeval tv;
	char buf[MEMDEV_SIZE];

    if (-1==(fd1=open ("/dev/memdev0", O_RDWR))) {
        printf("open dev0 error\n");
        _exit(EXIT_FAILURE);
    }
    if (-1==(fd2=open ("/dev/memdev1", O_RDWR))) {
        printf("open dev1 error\n");
        _exit(EXIT_FAILURE);
    }

    //先清空集合
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    //设置要监控的文件描述符
    FD_SET(fd1, &rfds);
    FD_SET(fd2, &rfds);
    FD_SET(fd1, &wfds);
    FD_SET(fd2, &wfds);
    maxfdp = fd1>fd2 ? fd1: fd2; 
    maxfdp += 1; 
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    printf("Test for select(): ...\n");

    while(1) {
        printf(" ==> select ...\n");
        if (-1==(ret=select(maxfdp, &rfds, &wfds, NULL, &tv))) {
            printf("select error\n");
            _exit(EXIT_FAILURE);
        }
        if (0==ret) {
            printf("No data within 10s, exit\n");
            break;
        } else {
            printf(" ==> %d fds are ready:\n", ret);
            FD_ISSET(fd1, &rfds)? printf("memdev0 is ready to be read\n"): 0;
            FD_ISSET(fd2, &rfds)? printf("memdev1 is ready to be read\n"): 0;
            FD_ISSET(fd1, &wfds)? printf("memdev0 is ready to be write\n"):0;
            FD_ISSET(fd2, &wfds)? printf("memdev1 is ready to be write\n"):0;
            
            if(FD_ISSET(fd1, &rfds)) {
                ret = read(fd1, buf, sizeof(buf));
                buf[ret] = 0;
                printf("read data in memdev0(%d bytes): %s\n", ret, buf);
            }
            if(FD_ISSET(fd2, &rfds)) {
                ret = read(fd2, buf, sizeof(buf));
                buf[ret] = 0;
                printf("read data in memdev1(%d bytes): %s\n", ret, buf);
            }
            if(FD_ISSET(fd1, &wfds)) {
                printf("write something to memdev0? yes/no\n");
                if (!strcmp(fgets(buf, MEMDEV_SIZE, stdin), "yes\n")) {
                    fgets(buf, MEMDEV_SIZE, stdin);
                    write(fd1, buf, strlen(buf));
                }
            }
            if(FD_ISSET(fd2, &wfds)) {
                printf("write something to memdev1? yes/no\n");
                if (!strcmp(fgets(buf, MEMDEV_SIZE, stdin), "yes\n")) {
                    fgets(buf, MEMDEV_SIZE, stdin);
                    write(fd2, buf, strlen(buf));
                }
            }
            printf("remove fd1/fd2 from write-fdset? yes/no\n");
            if (!strcmp(fgets(buf, MEMDEV_SIZE, stdin), "yes\n")) {
                FD_ZERO(&wfds);
            } else {
                FD_SET(fd1, &wfds);
                FD_SET(fd2, &wfds);
            }
            FD_SET(fd1, &rfds);
            FD_SET(fd2, &rfds);
        }
    }
	_exit(EXIT_SUCCESS);	
}
