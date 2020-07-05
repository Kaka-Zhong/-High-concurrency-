#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#include "stdio.h"


//#pragma comment(lib,"ws2_32.lib");	//��������  ������ͨ�õķ��� �Ѿ��������������
enum CMD	//ö�ٵ�¼�͵ǳ�
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};

struct DataHeader	//�������ݰ�ͷ
{
	short dataLength;
	short cmd;
};

struct Login : public DataHeader		//����һ���ṹ���װ����
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
};

struct LoginResult : public DataHeader	//���ص�¼�Ľ��
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 1;
	}
	int result;
};

struct Logout : public DataHeader		//����˭Ҫ�˳�
{
	Logout()
	{
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char username[32];
};
struct LogoutResult : public DataHeader		//���صǳ��Ľ��
{
	LogoutResult()
	{
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};
struct NewUserJoin : public DataHeader		//���û�����
{
	NewUserJoin()
	{
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		scok = 0;
	}
	int scok;
};

int processor(SOCKET _cSock)
{

	//������
	char szRecv[4096] = {};
	int len = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (len <= 0)
	{
		printf("��������Ͽ����ӣ��������\n");
		return -1;
	}
	switch (header->cmd)
	{
		
		case CMD_LOGIN_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LoginResult *login = (LoginResult*)szRecv;
			printf("�յ�����������Ϣ��CMD_LOGIN_RESULT, ���ݳ���%d \n", _cSock, header->dataLength);
			
			break;
		}
		case CMD_LOGOUT_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LogoutResult *logout = (LogoutResult*)szRecv;
			printf("�յ�����������Ϣ��CMD_LOGIN_RESULT, ���ݳ���%d \n", _cSock, header->dataLength);
			break;
		}
		case CMD_NEW_USER_JOIN:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			NewUserJoin *userjoin = (NewUserJoin*)szRecv;
			printf("�յ�����������Ϣ��CMD_NEW_USER_JOIN, ���ݳ���%d \n", _cSock, header->dataLength);
			break;
		}
	}
	return 0;
}

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
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(_sock,&fdRead);
		timeval t = { 1,0 };//ǰ������ �����Ǻ���
		int ret = select(_sock,&fdRead,0,0,&t);
		if (ret < 0)
		{
			printf("select �������1");
		}
		if (FD_ISSET(_sock,&fdRead))
		{
			FD_CLR(_sock,&fdRead);
			if (-1 == processor(_sock))
			{
				printf("select �������2");
				break;
			}
		}

		//printf("����ʱ�� ������ҵ��\n");
		Login login;
		strcpy(login.userName,"majun");
		strcpy(login.passWord,"518811");
		send(_sock,(const char*)&login,sizeof(Login),0);
		Sleep(2000);
		
		/*
		//3��������������
		scanf("%s",cmdBuf);

		//4��������������
		if (0 == strcmp(cmdBuf,"exit"))
		{
			break;
		}
		else if (0 == strcmp(cmdBuf, "Login"))
		{
			Login login;
			strcpy(login.userName,"majun");
			strcpy(login.passWord,"5188119");

			//5�����������������
			send(_sock, (const char *)&login, sizeof(login), 0);	//�ٷ�����

			//�ٽ��н���
			LoginResult loginret = {};
			recv(_sock, (char*)&loginret, sizeof(loginret), 0);
			printf("LoginResult = %d\n ",loginret.result);
		}
		else if (0 == strcmp(cmdBuf, "Logout"))
		{
			Logout logout;
			strcpy(logout.username,"majun");
			//5�����������������
			send(_sock, (const char *)&logout, sizeof(logout), 0);	//�ٷ�����

			//�ٽ��н���	
			LogoutResult logoutret = {};			
			recv(_sock, (char*)&logoutret, sizeof(logoutret), 0);
			printf("LoginResult = %d\n ", logoutret.result);
		}
		else
		{
			printf("��֧�ֵ����������������\n");
		}
		*/
	}
	
	WSACleanup();
	closesocket(_sock);
	getchar();
	return 0;
}