#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#include "stdio.h"


//#pragma comment(lib,"ws2_32.lib");	//��������  ������ͨ�õķ��� �Ѿ��������������
int main()
{
	//����window socket ����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	
	//1������һ���׽���
	SOCKET _sock = socket(AF_INET,SOCK_STREAM,0);
	if (INVALID_SOCKET == _sock)
	{
		printf("���󣬽���Socketʧ��...\n");
	}
	else
	{
		printf("����Socket�ɹ�...\n");
	}

	//2�����ӷ����� connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret)
	{
		printf("��������Socketʧ��...\n");
	}
	else
	{
		printf("����Socket�ɹ�...\n");
	}

	//3�����շ�������Ϣ
	char recvBuf[256] = {};
	int nlen = recv(_sock,recvBuf,256,0 );
	if(nlen>0)
	{
		printf("���յ����ݣ� %s",recvBuf);
	}

	
	WSACleanup();
	getchar();
	return 0;
}


