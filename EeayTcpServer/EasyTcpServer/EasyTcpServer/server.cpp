#include "EsayTcpServer.hpp"

bool g_bRun = true;
void cmdThread()
{
	while (true)
	{
		char cmdBuf[256] = {};
		cin >> cmdBuf;
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
	//��������ʼ��
	EasyTcpServer server;
	server.InitSocket();
	server.BindSocket(NULL,4567);
	server.Listen(5);

	std::thread t2(cmdThread);
	t2.detach();

	while (g_bRun)
	{
		server.OnRun();
	}

	server.Close();
	return 0;
}
