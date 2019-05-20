#include <stdio.h>
#include <stdlib.h> 
			/*管道通信(无名管道)*/
/*创建无名管道*/
void creatpipe(){
	int pipe_fd[2];
	if(pipe(pipe_fd)<0){
	printf("creat pipe error!\n");
	return;
	}else{
	printf("creat pipe success!\n");
	}
	//关闭管道
	close(pipe_fd[0]);
	close(pipe_fd[1]);
}
/*无名管道通信示例*/
void pipetx(){
	//声明变量
	int pipe_fd[2];
	pid_t pid;
	char buf_r[100];
	char * p_wbuf;
	int r_num;
	//创建管道(必须在fork前创建)
	if(pipe(pipe_fd)<0){
	printf("creat pipe error!\n");
	return;
	}
	//创建进程
	if((pid=fork())==0){//子进程
		printf("\n");
		sleep(2);//睡眠2秒，让父进程先执行写入
		close(pipe_fd[1]);
		if((r_num=read(pipe_fd[0],buf_r,sizeof(buf_r)))>0){
		printf("%d numbers read from the pipe is: %s\n",r_num,buf_r);
		}
		close(pipe_fd[0]);
		exit(0);
	}else if(pid>0){//父进程
		close(pipe_fd[0]);
		if(write(pipe_fd[1],"hello",5)){
		printf("parent write hello!\n");
		}
		if(write(pipe_fd[1]," pipe",5)){
		printf("parent write pipe!\n");
		}
		close(pipe_fd[1]);
		sleep(2);
		waitpid(pid,NULL,0);
		exit(0);
	}
	
	
}
void main(){
	//creatpipe();
	pipetx();
}

