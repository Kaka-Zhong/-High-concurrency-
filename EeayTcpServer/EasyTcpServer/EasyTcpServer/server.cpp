#include "EsayTcpServer.hpp"

int main()
{
	//��������ʼ��
	EasyTcpServer server;
	server.InitSocket();
	server.BindSocket(NULL,4567);
	server.Listen(5);
	
	while (server.isRun())
	{
		server.OnRun();
	}

	server.Close();
	return 0;
}
