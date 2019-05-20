#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
			/*消息队列*/
//结构
struct msg_buf{
	int mtype;//mtype名字可以任意取，但是里面必须要有一个int整形的数据，当前进程id(getpid())
	char data[255];//数据段名字也是可以任意取的，也可以在里面添加其它的数据段
};
void main(){
	key_t key;
	int msgid;
	int ret;
	struct msg_buf msgbuf;
	//根据文件名转换得到key
	key=ftok("/home/ly/protx/msg",'l');
	printf("key=[%x]\n",key);
	//msgget(key,flg)根据key打开或创建消息队列
	//flg IPC_CREAT:(创建),IPC_EXCL:与IPC_CREAT一同使用(要创建的已存在，则返回错误),IPC_NOWAIT:不阻塞
	msgid=msgget(key,IPC_CREAT|0666);
	if(msgid==-1){
	printf("create msg error!\n");
	return;
	}
	//消息队列发送
	msgbuf.mtype=getpid();
	strcpy(msgbuf.data,"test haha!");
	ret=msgsnd(msgid,&msgbuf,sizeof(msgbuf.data),IPC_NOWAIT);
	if(ret==-1){
	printf("send message error!\n");
	return;
	}
	//消息队列接收
	memset(&msgbuf,0,sizeof(msgbuf));
	ret=msgrcv(msgid,&msgbuf,sizeof(msgbuf.data),getpid(),IPC_NOWAIT);
	if(ret==-1){
	printf("recv message error!\n");
	return;
	}
	//打印
	printf("recv meg=[%s]\n",msgbuf.data);
}
