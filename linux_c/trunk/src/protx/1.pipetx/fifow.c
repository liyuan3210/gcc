#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#define FIFO_SERVER "/home/ly/protx/myfifo"
			/*命名管道（写）*/
/*要先开启fifor进程，写进fifor中去*/
void main(int argc,char * argv[]){
	int fd;
	char buf_w[100];
	int nwrite;
	//打开管道
	fd=open(FIFO_SERVER,O_WRONLY|O_NONBLOCK,0);
	if(argc==1){
	printf("please send data!\n");
	exit(-1);
	}
	printf("\nwrit data is %s \n",argv[1]);
	strcpy(buf_w,argv[1]);
	//向管道写入数据
	if((nwrite=write(fd,buf_w,sizeof(buf_w)))==-1){
		if(errno==EAGAIN)
			printf("the fifo has not been read yet,please try later\n");
	}else{
	printf("write %s to the fifo\n",buf_w);
	}
}
