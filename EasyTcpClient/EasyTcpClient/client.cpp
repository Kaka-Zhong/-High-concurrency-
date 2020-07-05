#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	
	#include <WinSock2.h>
	#include <windows.h>
	#pragma comment(lib,"ws2_32.lib")	//��������  ������ͨ�õķ��� �Ѿ��������������
#else
	#include <unistd.h>             //unix�ı�׼��
	#include <arpa/inet.h>
	#include <string.h>

	//����궨�� 
	#define SOCKET int
	#define INVALID_SOCKET (SOCKET)(~0)
	#define SOCKET_ERROR (-1)
#endif

#include <stdio.h>
#include <thread>

enum CMD        //ö�ٵ�¼�͵ǳ�
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};

struct DataHeader       //�������ݰ�ͷ
{
	short dataLength;
	short cmd;
};

struct Login : public DataHeader                //����һ���ṹ���װ����
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
};

struct LoginResult : public DataHeader  //���ص�¼�Ľ��
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 1;
	}
	int result;
};

struct Logout : public DataHeader               //����˭Ҫ�˳�
{
	Logout()
	{
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char username[32];
};
struct LogoutResult : public DataHeader         //���صǳ��Ľ��
{
	LogoutResult()
	{
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};
struct NewUserJoin : public DataHeader          //���û�����
{
	NewUserJoin()
	{
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		scok = 0;
	}
	int scok;
};

int processor(SOCKET _cSock)    //ר�Ŵ�����ܵ�����Ϣ
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
		printf("�յ�����������Ϣ��CMD_LOGIN_RESULT, ���ݳ���%d \n", header->dataLength);

		break;
	}
	case CMD_LOGOUT_RESULT:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LogoutResult *logout = (LogoutResult*)szRecv;
		printf("�յ�����������Ϣ��CMD_LOGIN_RESULT, ���ݳ���%d \n", header->dataLength);
		break;
	}
	case CMD_NEW_USER_JOIN:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		NewUserJoin *userjoin = (NewUserJoin*)szRecv;
		printf("�յ�����������Ϣ��CMD_NEW_USER_JOIN, ���ݳ���%d \n", header->dataLength);
		break;
	}
	}
	return 0;
}

bool g_bRun = true;
void cmdThread(SOCKET _sock)
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			printf("�˳��߳�\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "Login"))
		{
			Login login;
			strcpy(login.userName, "majun");
			strcpy(login.passWord, "518811");
			send(_sock, (const char*)&login, sizeof(Login), 0);
		}
		else if (0 == strcmp(cmdBuf, "Logout"))
		{
			Logout logout;
			strcpy(logout.username, "majun");
			send(_sock, (const char*)&logout, sizeof(Logout), 0);
		}
		else {
			printf("��֧�ֵ�����\n");
		}
	}
}

int main()
{
#ifdef _WIN32
	//����window socket ����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	//1������һ���׽���
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
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
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr("192.168.1.188");	//127.0.0.1
#else
	_sin.sin_addr.s_addr = inet_addr("192.168.1.188");
#endif
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret)
	{
		printf("�������ӷ�����ʧ��...\n");
	}
	else
	{
		printf("���ӷ������ɹ�...\n");
	}

	//�����߳�
	std::thread t1(cmdThread, _sock);       //��һ���Ǻ����� �ڶ�������Ҫ����Ĳ���
	t1.detach();            //�����߳̽��з���  һ��Ҫ���з��룬��Ȼ���̵߳��˳���ֱ�ӵ������߳�Ҳ�˳������ǣ����߳�û�������������򣬾ͻ��������
	while (g_bRun)
	{
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(_sock, &fdRead);

		timeval t = { 1,0 };//ǰ������ �����Ǻ���       ��selectÿ��1��ȥɨ��һ��
		int ret = select(_sock + 1, &fdRead, 0, 0, &t);

		if (ret < 0)
		{
			printf("select �������1");
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			if (-1 == processor(_sock))
			{
				printf("select �������2");
				break;
			}
		}
	}
#ifdef _WIN32
	WSACleanup();
	closesocket(_sock);
#else
	close(_sock);
#endif
	getchar();
	return 0;
}
