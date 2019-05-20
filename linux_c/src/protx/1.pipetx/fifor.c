#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#define FIFO "/home/ly/protx/myfifo"
			/*有名管道（读）_读取端*/ 
/*需要通过写入端fifow进行写入数据*/
void main(){
	int fd;
	char buf_r[100];
	int nread;
	//创建管道
	if((mkfifo(FIFO,0700)<0))
	printf("can not creat fifoserver!\n");
	
	printf("preparing for reading bytes...!\n");
	
	memset(buf_r,0,sizeof(buf_r));

	/*打开管道*/
	fd=open(FIFO,O_RDONLY,0);//|O_NONBLOCK
	if(fd==-1){
	perror("open");
	exit(1);
	}
	
	while(1){
		memset(buf_r,0,sizeof(buf_r));
		if((nread=read(fd,buf_r,sizeof(buf_r)))==-1){
			if(errno==EAGAIN)
				printf("no data yet\n");
		}
		printf("read %s from FIFOf\n",buf_r);
		sleep(1);
	}
	//pause();//暂停，等待信号
	unlink(FIFO);//删除fifo文件
	
}
