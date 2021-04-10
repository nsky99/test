#include "pch.h"
#include "ConnectVirusLib.h"
#include <ws2tcpip.h>
//  ��ʼ���׽���ģ��
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

// ����һ���׽��֣�Ӧ�ñ���[IP:PORT]
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



// ���ӷ�����
BOOL ConnectServer(SOCKET* client, SOCKADDR_IN* server_addr)
{
  server_addr->sin_family = AF_INET;		// Э��
  server_addr->sin_port = htons(0x8848);	// �˿�
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


// ����
void ClearUp(SOCKET* client)
{
  // 7. �����ݴ������֮����Ҫ�Ͽ�����
  closesocket(*client);


  // 8. �����׽���ģ�黵��
  WSACleanup();
}