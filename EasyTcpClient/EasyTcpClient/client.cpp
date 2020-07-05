#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define _CRT_SECURE_NO_DEPRECATE
	
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

#include <thread>
#include "EasyTcpClient.hpp"

void cmdThread(EasyTcpClient* client)
{
	while (true)
	{
		char cmdBuf[256] = {};
		/*scanf("%s", cmdBuf);*/
		cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit"))
		{
			client->Close();
			cout<<"�˳��߳�"<<endl;
			break;
		}
		else if (0 == strcmp(cmdBuf, "Login"))
		{
			Login login;
			strcpy(login.userName, "majun");
			strcpy(login.passWord, "518811");
			client->SendData(&login);
		}
		else if (0 == strcmp(cmdBuf, "Logout"))
		{
			Logout logout;
			strcpy(logout.username, "majun");
			client->SendData(&logout);
		}
		else {
			cout<<"��֧�ֵ�����"<<endl;
		}
	}
}

int main()
{
	EasyTcpClient client;
	//client.InitSoket();	//�����ӵĹ����п��Գ�ʼ��������Ҳ���Բ�д
	client.Connect("127.0.0.1",4567);
	std::thread t1(cmdThread, &client);       //��һ���Ǻ����� �ڶ�������Ҫ����Ĳ���
	t1.detach();            //�����߳̽��з���  һ��Ҫ���з��룬��Ȼ���̵߳��˳���ֱ�ӵ������߳�Ҳ�˳������ǣ����߳�û�������������򣬾ͻ��������

	while (client.isRun())
	{
		client.OnRun();
	}
	client.Close();
	cout << "�Ѿ��˳�" << endl;
	getchar();
	return 0;
}
