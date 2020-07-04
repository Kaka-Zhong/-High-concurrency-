#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#include "stdio.h"
#include <vector>

//#pragma comment(lib,"ws2_32.lib");	//��������  ������ͨ�õķ��� �Ѿ��������������

enum CMD	//ö�ٵ�¼�͵ǳ�
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
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

std::vector<SOCKET> g_clients;	//������е�socket

int processor(SOCKET _cSock)
{

	//������
	char szRecv[4096] = {};
	int len = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (len <= 0)
	{
		printf("�ͻ����Ѿ��˳����������\n");
		return -1;
	}
	switch (header->cmd)
	{
		printf("switch\n");
		case CMD_LOGIN:
		{
			//ע�� ����ΪʲôҪ�� sizeof(DataHeader)�ͼ�ȥsizeof(DataHeader)  ����Ϊ ǰ�� �����Ѿ�������һ��ͷ  ���� ����Ҫ����ַƫ��
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			Login *login = (Login*)szRecv;
			printf("�յ����CMD_LOGIN, ���ݳ���%d ,userName = %s Password = %s\n", login->dataLength, login->userName, login->passWord);
			//�����û������Ƿ���ȷ�Ĺ���
			LoginResult ret;
			send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
			break;
		}
		case CMD_LOGOUT:
		{
			Logout *logout = (Logout*)szRecv;
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			printf("�յ����CMD_LOGOUT, ���ݳ���%d ,userName = %s\n", logout->dataLength, logout->username);
			//�����û������Ƿ���ȷ�Ĺ���
			LogoutResult ret;
			send(_cSock, (char*)&ret, sizeof(LogoutResult), 0);
			break;
		}
		default:
		{
			DataHeader header = { 0,CMD_ERROR };
			send(_cSock, (char*)&header, sizeof(header), 0);
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


	while (true)
	{
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);

		FD_SET(_sock,&fdRead);
		FD_SET(_sock,&fdWrite);
		FD_SET(_sock,&fdExp);

		for (int n  =(int)g_clients.size()- 1;n>=0 ;n--)
		{
			FD_SET(g_clients[n],&fdRead);
		}

		//nfds��һ������ֵ����ָfd_set������������������socket���ķ�Χ�����������������������ļ����������ֵ+1����window�У���ֵ����д0
		int ret = select(_sock+1,&fdRead,&fdWrite,&fdExp,NULL );
		if (ret < 0)
		{
			printf("select �������\n");
			break;
		}
		if (FD_ISSET(_sock,&fdRead))	//FD_ISSET:�ж�һ���ļ��������Ƿ���һ�������У�����ֵ:��1,����0
		{
			FD_CLR(_sock,&fdRead);	//FD_CLR:��һ���ļ��������Ӽ������Ƴ�
			
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
			g_clients.push_back(_cSock);	//�µ��׽���ֱ�Ӽ��뵽��̬������
			printf("�µĿͻ��˼��룺IP = %s \n", inet_ntoa(clientAddr.sin_addr));
			
		}

		for (size_t n =0; n < fdRead.fd_count; n++)
		{
			int result = processor(fdRead.fd_array[n]);
			if (-1 == result)	//˵���г����˳���  ���Ǿ�Ӧ���ҵ���� Ȼ���Ƴ��� 
			{
				auto iter = find(g_clients.begin(),g_clients.end(),fdRead.fd_array[n]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}
	}
	for (int n = (int)g_clients.size() - 1; n >= 0; n--)
	{
		closesocket(g_clients[n]);	//�ر�
	}

	//6���ر��׽���
	closesocket(_sock);

	//��Socket API ��������TCP�ͻ���
	//1������һ��socket
	//2��bind �����ڽ��տͻ��������ӵ�����˿�
	WSACleanup();
	printf("���˳�\n");
	getchar();
	return 0;
}