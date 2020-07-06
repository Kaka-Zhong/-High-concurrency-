#ifndef _EasyTcpServer_hpp_
#define _EasyTcpServer_hpp_

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS

	#include <WinSock2.h>
	#include <windows.h>
	#pragma comment(lib,"ws2_32.lib")       //��������  ������ͨ�õķ��� �Ѿ��������������
#else
	#include <unistd.h>             //unix�ı�׼��
	#include <arpa/inet.h>
	#include <string.h>

	#define SOCKET int
	#define INVALID_SOCKET (SOCKET)(~0)
	#define SOCKET_ERROR (-1)
#endif

#include <iostream>
#include <vector>
#include "MessageHeader.hpp"
using namespace std;

class EasyTcpServer
{
private:
	SOCKET _sock;
	vector<SOCKET> g_clients;  //������е�socket
public:
	EasyTcpServer() 
	{
		_sock = INVALID_SOCKET;
	}
	virtual ~EasyTcpServer()
	{
		Close();
	}

	//��ʼ��socket
	void InitSocket()
	{
		//����window socket �Ļ���
#ifdef _WIN32
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif
		//1������һ���׽���
		if (INVALID_SOCKET != _sock)
		{
			cout << "Socket = " << _sock << " �������Ѿ��ر�" << endl;
			Close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == _sock)
		{
			cout << "���󣬽���Socket = " << _sock << " ʧ��..." << endl;
		}
		else
		{
			cout << "����Socket = " << _sock << " �ɹ�..." << endl;
		}
	}
	
	//�󶨶˿�
	void BindSocket(const char* ip,unsigned short port)
	{
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
#ifdef _WIN32
		if (ip)	//ip��Ϊ��
		{
			_sin.sin_addr.S_un.S_addr = inet_addr(ip); //���� inet_addr("127.0.0.1"
		}
		else
		{
			_sin.sin_addr.S_un.S_addr = INADDR_ANY; //���� inet_addr("127.0.0.1")
		}
#else
		if (ip)
		{
			_sin.sin_addr.s_addr = inet_addr(ip);      //���� inet_addr("127.0.0.1")
		}
		else
		{
			_sin.sin_addr.s_addr = INADDR_ANY ;      //���� inet_addr("127.0.0.1")
		}		
#endif // _WIN32
		
		int ret =::bind(_sock, (sockaddr*)&_sin, sizeof(_sin));		//����std�е�bing���socket�е�bind��ͻ���������::���з���
		if (SOCKET_ERROR == ret)
		{
			cout << "Socket = " << _sock << " �󶨶˿�ʧ��" << endl;
		}
		else
		{
			cout << "Socket = " << _sock << " �󶨶˿ڳɹ�" << endl;
		}
	}
	//�����˿�
	int Listen(int n)
	{
		int ret = listen(_sock, n);
		if (SOCKET_ERROR == ret)
		{
			cout << "Socket = " << _sock << " �����˿�ʧ��" << endl;
		}
		else
		{
			cout << "Socket = " << _sock << " �����˿ڳɹ�" << endl;
		}
		return ret;
	}
	//���ܿͻ�������
	SOCKET Accept()
	{
		sockaddr_in clientAddr = {};
		int nAddrLen = sizeof(sockaddr_in);
		SOCKET _cSock = INVALID_SOCKET;
#ifdef _WIN32
		_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
#else
		_cSock = accept(_sock, (sockaddr*)&clientAddr, (socklen_t *)&nAddrLen);
#endif // _WIN32

		if (INVALID_SOCKET == _cSock)
		{
			cout << "Socket = " << _cSock << "���󣬽��յ���Ч�ͻ���" << endl;
		}
		else
		{
			NewUserJoin userjoin;
			SendDataToAll(&userjoin);
			g_clients.push_back(_cSock);    //�µ��׽���ֱ�Ӽ��뵽��̬������
			cout<< "�µĿͻ��˼��� Socket = " << _cSock <<endl;
		}
		return _cSock;
	}

	//����������Ϣ
	bool OnRun()
	{
		if (isRun())
		{
			//�������׽��� BSD socket
			fd_set fdRead;
			fd_set fdWrite;
			fd_set fdExp;

			FD_ZERO(&fdRead);
			FD_ZERO(&fdWrite);
			FD_ZERO(&fdExp);

			FD_SET(_sock, &fdRead);
			FD_SET(_sock, &fdWrite);
			FD_SET(_sock, &fdExp);
			SOCKET maxSock = _sock;
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				FD_SET(g_clients[n], &fdRead);
				if (maxSock < g_clients[n])
				{
					maxSock = g_clients[n];
				}
			}

			//nfds��һ������ֵ����ָfd_set������������������socket���ķ�Χ��������������
			//���������ļ����������ֵ+1����window�У���ֵ����д0   ע��  �����ֵ��һ������
			timeval t = { 0,0 };    //��ʱ
			int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t);
			if (ret < 0)
			{
				cout << "select �������" << endl;
				Close();
				return false;
			}
			if (FD_ISSET(_sock, &fdRead))   //FD_ISSET:�ж�һ���ļ��������Ƿ���һ�������У�����ֵ:��1,����0
			{
				FD_CLR(_sock, &fdRead); //FD_CLR:��һ���ļ��������Ӽ������Ƴ�
				Accept();
			}

			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				if (FD_ISSET(g_clients[n], &fdRead))
				{
					int result = RecvData(g_clients[n]);           //������ܵ���Ϣ���׽��ֵ���Ϣ
					if (-1 == result)       //˵���г����˳���  ���Ǿ�Ӧ���ҵ���� Ȼ���Ƴ���
					{
						auto iter = g_clients.begin() + n;
						if (iter != g_clients.end())
						{
							g_clients.erase(iter);
						}
					}
				}
			}
		}
		return true;
	}

	//�ж��Ƿ�������
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}
	//�Ƿ��ڹ�����
	//��������  ����ճ�� ��ְ�
	int RecvData(SOCKET _cSock)            //ר�Ŵ�����ܵ�����Ϣ
	{
		//������
		char szRecv[4096] = {};
		int len = (int)recv(_cSock, szRecv, sizeof(DataHeader), 0);
		DataHeader* header = (DataHeader*)szRecv;
		if (len <= 0)
		{
			cout<<"Sokect = "<< _cSock<< " �Ѿ��˳����������"<<endl;
			return -1;
		}
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		OnNetMsg(_cSock,header);
		return 0;
	}

	//��Ӧ������Ϣ
	virtual void OnNetMsg(SOCKET _cSock,DataHeader* header)
	{
		switch (header->cmd)
		{
			case CMD_LOGIN:
			{
				//ע�� ����ΪʲôҪ�� sizeof(DataHe ader)�ͼ�ȥsizeof(DataHeader)  ����Ϊ ǰ�� �����Ѿ�������һ��ͷ  ���� ����Ҫ����ַƫ��
				
				Login *login = (Login*)header;
				cout<<"�յ��ͻ��� "<<_cSock<<" �����CMD_LOGIN, ���ݳ���= "<< login->dataLength <<" ,userName = "<< login->userName <<" Password = "<< login->passWord<<endl;
				//�����û������Ƿ���ȷ�Ĺ���
				LoginResult ret;
				send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
				break;
			}
			case CMD_LOGOUT:
			{
				Logout *logout = (Logout*)header;
				cout << "�յ��ͻ��� " << _cSock << " �����CMD_LOGOUT, ���ݳ���= " << logout->dataLength << " ,userName = " << logout->username << endl;
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
	}

	//��ָ��������
	int SendData(SOCKET _cSock,DataHeader* header)
	{
		if (isRun() && header)
		{
			send(_cSock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}

	//Ⱥ��������
	void SendDataToAll( DataHeader* header)
	{
		//�µĿͻ��˻�û�м��뵽vector֮�󣬾���Ⱥ��������������Ա
		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			SendData(g_clients[n], header);
		}
	}
	//�ر�socket
	void Close()
	{
		if (_sock != INVALID_SOCKET)
		{
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)    //������������˳�����for����ǲ���õ�ִ�е�
			{
#ifdef _WIN32
				closesocket(g_clients[n]);      //�ر�
#else
				close(g_clients[n]);
#endif
			}

			//6���ر��׽���
#ifdef _WIN32
			WSACleanup();
			closesocket(_sock);
#else
			close(_sock);
#endif
		}
	}



};

#endif // !_EasyTcpServer_hpp_

