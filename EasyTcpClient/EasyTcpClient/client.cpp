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
	char cmdBuf[128] = {};
	while (true)
	{
		//3��������������
		scanf("%s",cmdBuf);
		//4��������������
		if (0 == strcmp(cmdBuf,"exit"))
		{
			break;
		}
		else if (0 == strcmp(cmdBuf, "Login"))
		{
			Login login = { "majun","5188119" };
			DataHeader dh = { sizeof(login),CMD_LOGIN };
			//5�����������������
			send(_sock, (const char *)&dh, sizeof(dh), 0);	//�ȷ���ͷ
			send(_sock, (const char *)&login, sizeof(login), 0);	//�ٷ�����

			//�ٽ��н���
			DataHeader retHeader = {};
			LoginResult loginret = {};

			recv(_sock,(char*)&retHeader,sizeof(retHeader),0 );
			recv(_sock, (char*)&loginret, sizeof(loginret), 0);
			printf("LoginResult = %d\n ",loginret.result);
		}
		else if (0 == strcmp(cmdBuf, "Logout"))
		{
			Logout logout = { "majun" };
			DataHeader dh = { sizeof(logout),CMD_LOGOUT };
			//5�����������������
			send(_sock, (const char *)&dh, sizeof(dh), 0);	//�ȷ���ͷ
			send(_sock, (const char *)&logout, sizeof(logout), 0);	//�ٷ�����

																	//�ٽ��н���
			DataHeader retHeader = {};
			LogoutResult logoutret = {};

			recv(_sock, (char*)&retHeader, sizeof(retHeader), 0);
			recv(_sock, (char*)&logoutret, sizeof(logoutret), 0);
			printf("LoginResult = %d\n ", logoutret.result);
		}
		else
		{
			printf("��֧�ֵ����������������\n");
		}
	}
	
	WSACleanup();
	closesocket(_sock);
	getchar();
	return 0;
}


