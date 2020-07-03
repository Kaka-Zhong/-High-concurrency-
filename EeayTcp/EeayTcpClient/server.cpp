#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#include "stdio.h"


//#pragma comment(lib,"ws2_32.lib");	//��������  ������ͨ�õķ��� �Ѿ��������������
int main()
{
	//����window socket ����
	WORD ver = MAKEWORD(2,2);
	WSADATA dat;
	WSAStartup(ver,&dat);
	
	//��Socket API ��������TCP�ͻ���

	//1������һ��socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//2�����ӷ�����connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;	//���� inet_addr("127.0.0.1")
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))
	{
		printf("���󣬰󶨶˿�ʧ��...\n");
	}
	else
	{
		printf("�󶨶˿ڳɹ�...\n");
	}

	//3��listen ��������˿�
	if (SOCKET_ERROR == listen(_sock,5))
	{
		printf("���󣬼����˿�ʧ��...\n");
	}
	else
	{
		printf("�����˿ڳɹ�...\n");
	}

	//4��accept �ȴ����տͻ��˵�����
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;
	char msgBuf[] = "hello,im server";
	while (true)
	{
		accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
		if (INVALID_SOCKET == _cSock)
		{
			printf("���󣬽��յ���Ч�ͻ���...\n");
		}
		else
		{
			printf("�����˿ڳɹ�...\n");
		}
		printf("�µĿͻ��˼��룺IP = %s \n",inet_ntoa(clientAddr.sin_addr));
		//5��send ��ͻ��˷���һ������

		send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
	}


	//6���ر��׽���
	closesocket(_sock);


	//��Socket API ��������TCP�ͻ���
	//1������һ��socket
	//2��bind �����ڽ��տͻ��������ӵ�����˿�
	

	WSACleanup();
	return 0;
}


