#pragma once
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

//  ��ʼ���׽���ģ��
BOOL InitStartUp();

// ����һ���׽��֣�Ӧ�ñ���[IP:PORT]
BOOL CreateSocket(SOCKET* s);

// ���ӷ�����
BOOL ConnectServer(SOCKET* client, SOCKADDR_IN* server_addr);

// ����
void ClearUp(SOCKET* client);
