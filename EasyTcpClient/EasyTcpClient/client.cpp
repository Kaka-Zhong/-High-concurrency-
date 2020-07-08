#include <thread>
#include "EasyTcpClient.hpp"


bool g_bRun = true;
void cmdThread()
{
	while (true)
	{
		char cmdBuf[256] = {};
		std::cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			cout << "�˳��߳�" << endl;
			break;
		}
	}
}

int main()
{
	const int count = 500;//FD_SETSIZE-1;
	EasyTcpClient* client[count];

	for (int i = 0; i< count;i++)
	{
		client[i] = ::new EasyTcpClient();
	}

	for (int i = 0; i< count; i++)
	{
		client[i]->Connect("192.168.1.188", 4567);//140.143.12.171
		cout <<"count= " <<i << endl;
	}
	

	std::thread t1(cmdThread);       //��һ���Ǻ����� �ڶ�������Ҫ����Ĳ���
	t1.detach();

          //�����߳̽��з���  һ��Ҫ���з��룬��Ȼ���̵߳��˳���ֱ�ӵ������߳�Ҳ�˳������ǣ����߳�û�������������򣬾ͻ��������
	Login login;
	strcpy(login.userName,"majun majun majun");
	strcpy(login.passWord,"0123456789");
	while (g_bRun)
	{
		for (int i = 0; i <count;i++)
		{
			client[i]->SendData(&login);
			client[i]->OnRun();
		}

	}
	for (int i = 0; i <count; i++)
	{
		client[i]->Close();
	}

	cout << "�Ѿ��˳�" << endl;
	getchar();
	return 0;
}
