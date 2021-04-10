#pragma once
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

//  初始化套接字模块
BOOL InitStartUp();

// 创建一个套接字，应该保存[IP:PORT]
BOOL CreateSocket(SOCKET* s);

// 连接服务器
BOOL ConnectServer(SOCKET* client, SOCKADDR_IN* server_addr);

// 清理
void ClearUp(SOCKET* client);
