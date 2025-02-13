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
 
int main()
{
 
	//客户端只需要一个套接字文件描述符，用于和服务器通信
	int serverSocket;
	 
	//描述服务器的socket
	struct sockaddr_in serverAddr;
	 
	char sendbuf[200]; //存储 发送的信息 
	char recvbuf[200]; //存储 接收到的信息 
	 
	int iDataNum;
	
	//下面代码初始化 
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) !=2){
	    printf("require version fail!");
	    return -1;
	}
	
	if((serverSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
	{
		perror("socket");
		return 1;
	}
	 
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	 
	//指定服务器端的ip，本地测试：127.0.0.1
	//inet_addr()函数，将点分十进制IP转换成网络字节序IP
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	 
	if(connect(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("connect");
		return 1;
	}
	 
	printf("连接到主机...\n");
	 
	while(1)
	{
		printf("发送消息:");
		scanf("%s", sendbuf);
		printf("\n");
		send(serverSocket, sendbuf, strlen(sendbuf), 0); //向服务端发送消息
		if(strcmp(sendbuf, "quit") == 0) break;
		printf("读取消息:");
		recvbuf[0] = '\0';
		iDataNum = recv(serverSocket, recvbuf, 200, 0); //接收服务端发来的消息
		recvbuf[iDataNum] = '\0';
		printf("%s\n", recvbuf);
	}

	close(serverSocket);
 
	return 0;
}

