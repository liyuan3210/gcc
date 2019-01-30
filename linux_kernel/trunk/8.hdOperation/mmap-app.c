#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

void main()
{
	int fd;
	char *start;
	char buf[100];
	
	/*打开文件*/
	fd=open("testfile",O_RDWR);
	start=mmap(NULL,100,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

	/*读出数据*/
	strcpy(buf,start);
	printf("buf = %s\n",buf);

	/*写入数据*/
	strcpy(start,"buf is not null!");

	munmap(start,100);//解除映射
	close(fd);
}
