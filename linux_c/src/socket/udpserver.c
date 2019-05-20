/**********************************************************
*实验要求：   建立基于UDP协议的服务器与客户端的通讯。客户端向服务器发送
*           字符串，服务器将收到的字符串打印出来。
*功能描述：   根据套接字建立UDP通讯的过程，创建服务端程序，通过服务端的
*           地址接收客户端的数据，并打印到终端上。
**********************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#define SERVER_PORT 8888
#define MAX_MSG_SIZE 1024
			/*udp server*/
//进行读写操作
void udps_respon(int sockfd)
{
	struct sockaddr_in addr;
	int addrlen,n;
	char msg[MAX_MSG_SIZE];

	while(1)
	{ 
		/* 从网络上读,并写到网络上 */
		bzero(msg,sizeof(msg)); // 初始化,清零
		addrlen = sizeof(struct sockaddr);
		n=recvfrom(sockfd,msg,MAX_MSG_SIZE,0,(struct sockaddr*)&addr,&addrlen); // 从客户端接收消息
		msg[n]=0;//将收到的字符串尾端添加上字符串结束标志
		/* 显示服务端已经收到了信息 */
		fprintf(stdout,"Server have received %s",msg); // 显示消息
	}
}

int main(void)
{
	int sockfd;
	struct sockaddr_in addr;

	/* 服务器端开始建立socket描述符 */
	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd<0)
	{
		fprintf(stderr,"Socket Error:%s\n",strerror(errno));
		exit(1);
	}

	/* 服务器端填充 sockaddr结构 */
	bzero(&addr,sizeof(struct sockaddr_in));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(SERVER_PORT);

	/* 捆绑sockfd描述符 */
	if(bind(sockfd,(struct sockaddr *)&addr,sizeof(struct sockaddr_in))<0)
	{
		fprintf(stderr,"Bind Error:%s\n",strerror(errno));
		exit(1);
	}

	udps_respon(sockfd); // 进行读写操作
	close(sockfd);
}
 
