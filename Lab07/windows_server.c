
#include <sys/stat.h>
#include <fcntl.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "wsock32.lib")
#include <errno.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include<ws2tcpip.h>
#include <stdio.h>
#include <unistd.h>
#define SERVER_PORT 7701
 
/*
监听后，一直处于accept阻塞状态，
直到有客户端连接，
当客户端如数quit后，断开与客户端的连接
*/
 
int main()
{
 
	//调用socket函数返回的文件描述符
	int serverSocket;
	 
	//声明两个套接字sockaddr_in结构体变量，分别表示客户端和服务器
	struct sockaddr_in server_addr;
	struct sockaddr_in clientAddr;
	 
	int addr_len = sizeof(clientAddr);
	int client;
	char buffer[200]; //存储 发送和接收的信息 
	int iDataNum;

	//必须先初始化 
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) !=2){
	    printf("require version fail!");
	    return -1;
	}
	
	//socket函数，失败返回-1
	//int socket(int domain, int type, int protocol);
	//第一个参数表示使用的地址类型，一般都是ipv4，AF_INET
	//第二个参数表示套接字类型：tcp：面向连接的稳定数据传输SOCK_STREAM
	//第三个参数设置为0
	//建立socket
	if((serverSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
	{
		perror("socket");
		return 1;
	}
	
	//初始化 server_addr
	memset(&server_addr,0, sizeof(server_addr));
	 
	//初始化服务器端的套接字，并用htons和htonl将端口和地址转成网络字节序
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	 
	//ip可是是本服务器的ip，也可以用宏INADDR_ANY代替，代表0.0.0.0，表明所有地址
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	 
	//对于bind，accept之类的函数，里面套接字参数都是需要强制转换成(struct sockaddr *)
	//bind三个参数：服务器端的套接字的文件描述符，
	if(bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("connect");
		return 1;
	}
	 
	//设置服务器上的socket为监听状态
	if(listen(serverSocket, 5) < 0)
	{
		perror("listen");
		return 1;
	}
	
	//循环 接收消息、发送消息 
	while(1)
	{ 
		printf("监听端口: %d\n", SERVER_PORT);
		
		//调用accept函数后，会进入阻塞状态
		//accept返回一个套接字的文件描述符，这样服务器端便有两个套接字的文件描述符，
		//serverSocket和client。
		//serverSocket仍然继续在监听状态，client则负责接收和发送数据
		//clientAddr是一个传出参数，accept返回时，传出客户端的地址和端口号
		//addr_len是一个传入-传出参数，传入的是调用者提供的缓冲区的clientAddr的长度，以避免缓冲区溢出。
		//传出的是客户端地址结构体的实际长度。
		//出错返回-1
		
		client = accept(serverSocket, (struct sockaddr*)&clientAddr, (socklen_t*)&addr_len);
		
		if(client < 0)
		{
			perror("accept");
			continue;
		}
		 
		printf("等待消息...\n");
		
		//inet_ntoa ip地址转换函数，将网络字节序IP转换为点分十进制IP
		//表达式：char *inet_ntoa (struct in_addr);
		printf("IP is %s\n", inet_ntoa(clientAddr.sin_addr)); //把来访问的客户端的IP地址打出来
		printf("Port is %d\n", htons(clientAddr.sin_port)); 
		 
		while(1)
		{
			buffer[0] = '\0';
			iDataNum = recv(client, buffer, 1024, 0);
			if(iDataNum < 0)
			{
	//			perror("recv null");
				continue;
			}
			buffer[iDataNum] = '\0';
			if(strcmp(buffer, "quit") == 0) break;
			printf("收到消息: %s\n", buffer);
			printf("发送消息:");
			scanf("%s", buffer);
			send(client, buffer, strlen(buffer), 0); //服务端也向客户端发送消息 
			if(strcmp(buffer, "quit") == 0) break; //输入quit停止服务端程序 
		}
	}
	 
	close(serverSocket);
	 
	return 0;
 
}
