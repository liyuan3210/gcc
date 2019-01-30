#include <stdio.h>
#include <pthread.h>
//创建一个线程
//线程编译时要加上-lpthread
int sum=0;
int * pe;
//线程1
void *myt1(void *arg){
	//接收时也必须转换一下
	int *num;
	num=(int *)arg;	
	/*而exit是终止一个进程，终止一个线程应该是pthread_exit*/
	pthread_exit(pe);

	int i;
	for(i=0;i<6;i++){
	sum++;
	printf("the thread 1,arg is %d---sum:%d.\n",*num,sum);
	sleep(1);
	}
}
//线程2
void *myt2(void *arg){
	//接收时也必须转换一下
	int *num;
	num=(int *)arg;	
	/*获取线程id与进程id*/
	printf("thread id is:%d\n",(unsigned int)pthread_self());
	printf("process id is:%d\n",getpid());

	int i;
	for(i=0;i<6;i++){
	sum++;
	printf("the thread 2,arg is %d---sum:%d.\n",*num,sum);
	sleep(1);
	}
}
void main(){
	int i=0,ret=0;
	pthread_t id1,id2;
	//线程传参(转换成指针类型)
	int ag1=3,ag2=6;
	int *arg1,*arg2;
	arg1=&ag1;
	arg2=&ag2;
	//*arg1=&ag1;
	//*arg2=&ag2;

	/*创建线程1*/
	ret=pthread_create(&id1,NULL,myt1,arg1);
	if(ret){
	printf("creat thread 1 error!\n");
	return;
	}
	/*创建线程2*/
	ret=pthread_create(&id1,NULL,myt2,arg2);
	if(ret){
	printf("creat thread 2 error!\n");
	return;
	}
	//printf("thread exit value is:%d\n",*pe);
	/*线程等待*/
	//如果当main进程执行先退出后，上面的两个线程将没有机会执行，所以要加上两个线程等待
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
}
