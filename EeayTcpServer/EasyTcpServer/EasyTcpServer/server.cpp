#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#include "stdio.h"

//#pragma comment(lib,"ws2_32.lib");	//��������  ������ͨ�õķ��� �Ѿ��������������

enum CMD	//ö�ٵ�¼�͵ǳ�
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR
};

struct DataHeader	//�������ݰ�ͷ
{
	short dataLength;
	short cmd;
};

struct Login	//DatePackage	//����һ���ṹ���װ����
{
	char userName[32];
	char passWord[32];
};

struct LoginResult		//���ص�¼�Ľ��
{
	int result;
};

struct Logout		//����˭Ҫ�˳�
{
	char username[32];
};
struct LogoutResult		//���صǳ��Ľ��
{
	int result;
};

int main()
{
	//����window socket ����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

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
	if (SOCKET_ERROR == listen(_sock, 5))
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
	
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock)
	{
		printf("���󣬽��յ���Ч�ͻ���...\n");
	}
	else
	{
		printf("�����˿ڳɹ�...\n");
	}
	printf("�µĿͻ��˼��룺IP = %s \n", inet_ntoa(clientAddr.sin_addr));
	char _recvBuf[128] = {};
	while (true)
	{
		DataHeader header = {};
		int len = recv(_cSock,(char*)& header,sizeof(DataHeader),0);
		if (len <= 0)
		{
			printf("�ͻ����Ѿ��˳����������\n");
			break;
		}
		printf("�յ����%d ���ݳ���%d \n",header.cmd, header.dataLength);

		switch (header.cmd)
		{
			case CMD_LOGIN:
			{
				Login login = {};
				recv(_cSock, (char*)& login, sizeof(Login), 0);
				//�����û������Ƿ���ȷ�Ĺ���
				LoginResult ret = { 1 };
				send(_cSock, (const char*)&header, sizeof(DataHeader ), 0);	//����Ϣͷ
				send(_cSock,(char*)&ret,sizeof(LoginResult),0 );
				break;
			}
			case CMD_LOGOUT :
			{
				Logout logout = {};
				recv(_cSock, (char*)& logout, sizeof(Logout), 0);
				//�����û������Ƿ���ȷ�Ĺ���
				LogoutResult ret = { 0 };
				send(_cSock, (const char*)&header, sizeof(DataHeader), 0);	//����Ϣͷ
				send(_cSock, (char*)&ret, sizeof(LogoutResult), 0);
				break;
			}
			default:
				header.cmd = CMD_ERROR;
				header.dataLength = 0;
				send(_cSock,(char*)&header,sizeof(header),0);
				break;
		}
	}


	//6���ر��׽���
	closesocket(_sock);


	//��Socket API ��������TCP�ͻ���
	//1������һ��socket
	//2��bind �����ڽ��տͻ��������ӵ�����˿�


	WSACleanup();
	return 0;
}


