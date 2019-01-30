#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<stdlib.h>
#include<math.h>
#include<errno.h>
			/*信号量*/
//本程序实现父子进程的一个同步，先让父进程占有资源，让子进程等待负进程，最后子进程得到资源
int main()
{
	int pid,semid;
	key_t semkey;
	//将文件路径和项目ID（后面那个“1“）转换为System V IPC Key
	if((semkey=ftok("./semtx.c",1))<0)
	{
		printf("ftok failed\n");
		exit(EXIT_FAILURE);
	}
	printf("ftok ok, semkey=%d\n",semkey);
		/*int semget(key,nsems,semflg)创建信号灯集，其中包含1个信号灯*/
	//key:由ftok获取，nsems:创建信号量个数，semflg:表示列
	if((semid=semget(semkey,1,IPC_CREAT|IPC_EXCL|0700))<0)
	{
		printf("semget failed\n");exit(EXIT_FAILURE);
	}
	printf("semget ok, semid=%d\n",semid);
		/*设置semid对应的信号集中第一个信号灯的semval为1。*/
	if((semctl(semid,0,SETVAL,1))<0)
	{
		printf("semctl set semval failed\n");exit(EXIT_FAILURE);
	}
	printf("semctl set semval ok\n\n");
		/*创建进程*/
	if((pid=fork())<0)
	{
		printf("fork failed\n");
		exit(EXIT_FAILURE);
	}
	else if(pid>0)//父进程，先索取共享资源，而后释放
	{
		struct sembuf p_op_buf,v_op_buf;
		p_op_buf.sem_num=0;//表示要获取或者释放信号量的“第几个”，数组下表
		p_op_buf.sem_op=-1;//表示要“获取”或者“释放”信号量
		//p_op_buf.sem_flg;//表示sops有多少个

		/*以上三行向semid代表的信号灯集的第一个信号灯申请一个资源，即使semval减1*/
		//semop(semid,sops,nsops),semid信号id，sops:是个操作数组，nsops:所指向数组的元素个数
		if(semop(semid,&p_op_buf,1)<0)
		{
		printf("semop failed\n");
		exit(EXIT_FAILURE);
		}
		printf("father get the semaphore\n");
		sleep(6);
		printf("father release the semaphore\n");
		v_op_buf.sem_num=0;
		v_op_buf.sem_op=1;
		v_op_buf.sem_flg=0;
		/*以上三行将释放一个资源给semid代表的信号灯集第一个信号灯*/
		if(semop(semid,&v_op_buf,1)<0)
		{
		printf("semop release semaphore failed\n");
		exit(EXIT_FAILURE);
		}
	}
	else//子进程不断申请共享资源，申请到后声明一下，然后释放
	{
		struct sembuf p_op_buf,v_op_buf;
		sleep(1);//等待父进程将唯一的资源占用
		printf("child wait for the semaphore\n");
		p_op_buf.sem_num=0;
		p_op_buf.sem_op=-1;
		p_op_buf.sem_flg=0;//该标志位不用要清零，此处不清零将出现错误
		/*向semid代表的信号灯集的第一个信号灯申请一个资源，申请不到就会阻塞，直到有了资源*/
		if(semop(semid,&p_op_buf,1)<0)
		{
		printf("semop get semaphore failed\n");
		exit(EXIT_FAILURE);
		}
		/*semctl*/
		printf("child get the semaphore\n");
		if(semctl(semid,0,IPC_RMID,0)<0)
		{
		printf("semctl remove semaphore set failed\n");
		exit(EXIT_FAILURE);
		}
	}
}
 
