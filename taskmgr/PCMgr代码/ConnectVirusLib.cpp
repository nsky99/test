#include "pch.h"
#include "ConnectVirusLib.h"
#include <ws2tcpip.h>
//  初始化套接字模块
BOOL InitStartUp()
{
  WSADATA wsadata = { 0 };
  int result = WSAStartup(MAKEWORD(2, 2), &wsadata);
  if (result != 0 || wsadata.wVersion != 0x0202)
  {
    return 0;
  }
  return 1;
}

// 创建一个套接字，应该保存[IP:PORT]
BOOL CreateSocket(SOCKET* s)
{
  *s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (*s == INVALID_SOCKET)
  {
    WSACleanup();
    return 0;
  }
  return 1;
}



// 连接服务器
BOOL ConnectServer(SOCKET* client, SOCKADDR_IN* server_addr)
{
  server_addr->sin_family = AF_INET;		// 协议
  server_addr->sin_port = htons(0x8848);	// 端口
  inet_pton(AF_INET, "127.0.0.1", &server_addr->sin_addr);

  int result = 
    connect(*client, (sockaddr*)server_addr, sizeof(*server_addr));
  if (result == SOCKET_ERROR)
  {
    closesocket(*client);
    WSACleanup();
    return 0;
  }
  return 1;
}


// 清理
void ClearUp(SOCKET* client)
{
  // 7. 当数据处理结束之后，需要断开连接
  closesocket(*client);


  // 8. 清理套接字模块坏境
  WSACleanup();
}