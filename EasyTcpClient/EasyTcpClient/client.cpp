#include <thread>
#include "EasyTcpClient.hpp"

int main()
{
	EasyTcpClient client1;
	client1.InitSoket();	//�����ӵĹ����п��Գ�ʼ��������Ҳ���Բ�д

	client1.Connect("192.168.1.188",4567);//140.143.12.171
	//std::thread t1(cmdThread, &client1);       //��һ���Ǻ����� �ڶ�������Ҫ����Ĳ���
	//t1.detach();            //�����߳̽��з���  һ��Ҫ���з��룬��Ȼ���̵߳��˳���ֱ�ӵ������߳�Ҳ�˳������ǣ����߳�û�������������򣬾ͻ��������
	Login login;
	strcpy(login.userName,"majun majun majun");
	strcpy(login.passWord,"0123456789");
	while (client1.isRun())
	{
		client1.SendData(&login);
		client1.OnRun();
	}
	client1.Close();
	cout << "�Ѿ��˳�" << endl;
	getchar();
	return 0;
}
