#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <windows.h>
//#pragma comment(lib,"ws2_32.lib");	//��������  ������ͨ�õķ��� �Ѿ��������������
int main()
{
	//����window socket ����
	WORD ver = MAKEWORD(2,2);
	WSADATA dat;
	WSAStartup(ver,&dat);


	WSACleanup();
	return 0;
}