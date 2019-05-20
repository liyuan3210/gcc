#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
			/*使用linux系统调用（文件复制功能）*/
int main(int argc,char **argv){
	//定义变量
	int from_fd,to_fd;
	int bytes_read,bytes_write;
	char buffer[BUFFER_SIZE];
	char * ptr;
	int file_len=0;
	//判断参数
	if(argc!=3){
	printf("usage:%s fromfile tofile\n",argv[0]);
	exit(1);
	}
	/*打开源文件*/
	if((from_fd=open(argv[1],O_RDONLY))<0){
	printf("open file %s error!\n",argv[1]);
	exit(1);
	}
	/*创建目的文件*/
	if((to_fd=open(argv[2],O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR))<0){
	printf("creat file %s error!",argv[2]);
	exit(1);
	}
	//测得文件大小
	file_len=lseek(from_fd,0,SEEK_END);
	printf("\nlseek测得文件长度是：%d\n",file_len);
	/*拷贝代码*/
	while(bytes_read=read(from_fd,buffer,BUFFER_SIZE)){
		//发生了错误
		if(bytes_read<0){
		printf("read error!\n");
		exit(1);
		}
		write(to_fd,buffer,bytes_read);
		file_len=bytes_read;
		//bzero(buffer,BUFFER_SIZE);
	}
	printf("\nthere are %d byte data left witout copy\n",file_len);
	close(from_fd);
	close(to_fd);
	exit(0);
	
}
