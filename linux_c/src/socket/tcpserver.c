/**********************************************************
*实验要求：   建立基于TCP协议的服务器与客户端的连接。客户端向服务器发送
*           字符串，服务器将收到的字符串打印出来。
*功能描述：   根据套接字建立TCP连接的过程，创建服务端程序，并在服务端等
*           待接收客户端的数据，并打印到终端上。
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
/*tcp server*/
void main(){
	int sockfd,new_fd; 
	struct sockaddr_in server_addr; 
	struct sockaddr_in client_addr; 
	int sin_size; 
	int nbytes;
	char buffer[1024];
	/*创建socket服务器*/
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:IPV4;SOCK_STREAM:TCP
	{ 
		fprintf(stderr,"Socket error:%s\n\a",strerror(errno)); 
		exit(1); 
	} 
	/*填充服务器sockaddr结构*/
	bzero(&server_addr,sizeof(struct sockaddr_in));//初始化置0
	server_addr.sin_family=AF_INET;//internet
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);//将本机的long转换程网络上的long，INADDR_ANY表示服务器能够运行在任何ip主机上
	//server_addr.sin_addr.s_addr=inet_addr("192.168.1.1");//用于绑定一个固定的ip，inet_addr表示转换成整形ip
	server_addr.sin_port=htons(PORTNUMBER);//将本机的short转换成网络上的short数据端口号
	/*捆绑sockfd描述符到ip地址*/
	if(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) 
	{ 
		fprintf(stderr,"Bind error:%s\n\a",strerror(errno)); 
		exit(1); 
	} 
	/*设置允许的最大链接数*/
	if(listen(sockfd,5)==-1) 
	{ 
		fprintf(stderr,"Listen error:%s\n\a",strerror(errno));
		exit(1); 
	} 
	//服务循环监听客户端
	while(1){
		/*服务器阻塞，直到客户程序建立连接*/
		sin_size=sizeof(struct sockaddr_in); 
		if((new_fd=accept(sockfd,(struct sockaddr *)(&client_addr),&sin_size))==-1) 
		{ 
			fprintf(stderr,"Accept error:%s\n\a",strerror(errno));
			exit(1); 
		} 
 
		fprintf(stderr,"Server get connection from %s\n",(char*)inet_ntoa(client_addr.sin_addr)); //将网络地址转换成字符串
		if((nbytes=read(new_fd,buffer,1024))==-1) 
		{ 
			fprintf(stderr,"Read Error:%s\n",strerror(errno)); 
			exit(1); 
		} 		
		buffer[nbytes]='\0';
		printf("Server received %s\n",buffer);
		//关闭连接
		close(new_fd); 
		//循环下一个
	}
	//结束通讯
	close(sockfd); 
	exit(0); 

}

