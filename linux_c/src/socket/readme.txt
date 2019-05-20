			网络编程
linux网络优势描述：
	1.完善内置网络，其他操作系统不包含如此紧密与内核结合在一起的网络部分.
	2.linux免费提供了大量支持internet的软件
	3.能通过一些linux命令完成内部信息或文件的传输，网络文件传输.
	4.远程访问支持协议比较多，vnc,ssh,telnet.
	5.网络安全，linux采取了许多安全技术措施，包括对读写权限的控制。
1.网络模型：
        ISO模型              TCP/IP(linux模型)
	1>.应用层
	2>.表示层
	3>.会话层             应用层
---------------------------------------------------------
	4>.传输层	      传输层
---------------------------------------------------------
	5>.网络层	      网络层
---------------------------------------------------------
	6>.数据链路层
	7>.物理层            网络接口层


2.TCP/IP常见协议包含(每一层都是从下到上的一个封装协议)：
	1>.应用层[telnet,ftp,tftp,smtp,dns]
---------------------------------------------------------
	2>.传输层[tcp,udp]
---------------------------------------------------------
	3>.网络层[ip,ICMP,arp]
---------------------------------------------------------
	4>.网络接口层[以太网(数据链路层)]

3.结合wireshark分析每一层的协议包

###################################################################################
			socket编程
描述：
	linux网络编程是通过socket套接字实现的，socket是一种文件描述符.
1.socket三种类型：
	1>.流式套接字(sock_stream)
	提供可靠的，面向通讯流，使用的是TCP协议，TCP保证了数据传输的正确和顺序性.
	2>.数据报套接字(sock_dgram)
	无连接的服务，数据通过相互独立的报文进行传输，无序，并不保证可靠的，使用UDP协议.
	3>.原始套接字(sock_raw)
	原始套接子允许使用IP协议，主要用于新的网络协议的测试等.

2.网络地址
	1>.struct sockaddr
	struct sockaddr{
	u_short sa_family;//协议族，“AF_xxx”表示,如AF_INET(ip协议族)
	char sa_data[14];//协议地址
	}
	2>.struct sockaddr_in(实际中经常会用到与sockaddr相同作用的sockaddr_in).
	struct sockaddr_in{
	short int sin_family;//协议族
	unsigned int sin_port;//端口号
	struct in_addr sin_addr;//协议特定地址，通常是以unsigned long S_addr;来记录32位地址
	unsigned char sin_zero[8];//填0
	}
3.地址转换
	1>.int inet_aton(const char *cp,struct in_addr *inp)
	把字符串ip转换成32位的sin_addr
	2>.char*inet_ntoa(struct in_addr in)
	把32位的sin_addr转换成字符串ip
4.字节序转换
	不同cpu对变量的字节存储顺序可能不同，有的是高位在前，有的是低位在前。
	而网络数据顺序一定要是统一的(网络上统一是big endian)，就一定要统一.
	转换函数：
	1>.htons:把unsigned short类型从主机序转换到网络序
	2>.htonl:把unsigned long类型从主机序转换到网络序
	3>.ntohs:把unsigned short类型从网络序转换到主机序
	4>.ntohl:把unsigned long类型从网络序转换到主机序
5.使用主机名访问localhost
	struct hostent*gethostbyname(const char*hostname){
	char*h_name;//主机正式名称
	char*h_aliases;//主机的别名
	int h_addrtype;//主机的地址类型 AF_INET
	int h_length;//主机地址长度
	char **h_addr_list;//主机IP地址列表
	}
6.常用socket函数
	1>.socket:创建一个socket
	2>.bind:用于绑定ip地址与端口号到socket
	3>.connet:用于与服务器进行连接
	4>.listen:设置服务器最大链接请求
	5>.accept:用来等待客户端的socket请求
	6>.send:发送数据(或者read()和write())
	7>.recv:接收数据

###################################################################################
			服务器模型
描述:
常见服务器模型
1>.循环服务器
服务器在统一个时间内只能响应一个客户的请求.
2>.并发服务器
服务器在同一个时刻可以响应多个客户的请求，通常使用多进程，多线程处理并使用在TCP服务器上.





