/**********************************************************
*实验要求：   建立基于TCP协议的服务器与客户端的连接。客户端向服务器发送
*           字符串，服务器将收到的字符串打印出来。
*功能描述：   根据套接字建立TCP连接的过程，创建客户端程序，并在与服务端
*           建立连接后，向其发送指定字符串。
**********************************************************/
#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 

#define PORTNUMBER 3333
/*tcp client*/
int main(int argc, char *argv[]) 
{ 
	int sockfd; 
	char buffer[1024]; 
	struct sockaddr_in server_addr; 
	struct hostent *host; 
	//检查参数
	if(argc!=2) 
	{ 
		fprintf(stderr,"Usage:%s hostname \a\n",argv[0]);  
		exit(1); 
	} 

	/* 使用hostname查询host名字 */
	if((host=gethostbyname(argv[1]))==NULL) 
	{ 
		fprintf(stderr,"Gethostname error\n");
		exit(1); 
	} 

	/* 开始建立sockfd描述 */ 
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:Internet;SOCK_STREAM:TCP
	{ 
		fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
		exit(1); 
	} 

	/*客户端填充服务端数据*/ 
	bzero(&server_addr,sizeof(server_addr)); //初始化置0
	server_addr.sin_family=AF_INET;          // IPV4
	server_addr.sin_port=htons(PORTNUMBER);  // 将本机的short转换成网络上的short数据端口号
	server_addr.sin_addr=*((struct in_addr *)host->h_addr); //IP地址

	/* 客户端发起连接 */ 
	if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) 
	{ 
		fprintf(stderr,"Connect Error:%s\a\n",strerror(errno)); 
		exit(1); 
	} 

	/* 链接成功等待输入 */ 
	printf("Please input char:\n");
	/* 发送数据 */
	fgets(buffer,1024,stdin); 
	write(sockfd,buffer,strlen(buffer)); 
	/* 结束通讯 */ 
	close(sockfd); 
	exit(0); 
}

