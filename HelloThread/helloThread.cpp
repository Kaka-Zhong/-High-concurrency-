#include <iostream>
#include <thread>
#include <mutex>	//��
#include <atomic>	//ԭ�Ӳ���
#include "CELLTimestamp.hpp"
using namespace std;

mutex m;
const int tCount = 4;
atomic<int> sum = 0;	//atomic_int sum = 0;	//��sum��Ϊ��������Դ ���Ǹ���ԭ�Ӳ����������Ͳ��������ˣ���������Ѿ���������
void workFunc(int index)
{
	for (int i = 0; i<20000000; i++)
	{
		//lock_guard<mutex> lg(m);	//�Խ���  ���� �ٹ����ʱ���Լ�����  ������ʱ�����
		//m.lock();	//�ٽ�����-��ʼ
		sum++;
		//m.unlock();	//�ٽ�����-����
	}
	//cout << "index = " << index << "  other Tread  i=" << i << endl;
}

int main()
{	
	thread t[tCount];
	for (int i= 0 ;i< tCount;i++)
	{
		t[i] = thread(workFunc,i);	
	}
	CELLTimestamp tTime;
	for (int i = 0; i< tCount; i++)
	{	
		t[i].join();
	}
	double t1 = tTime.getElapsedTimeInMilliSec() ;
	printf("t1 = %f",t1);
	cout << "  Main Thread, sum = "<<sum<< endl;
	getchar();
	return 0;
}

//thread t1(workFunc,);
//t1.detach();	//detach �Ὣ���̺߳����̷ֿ߳��������߳�����֮�� �ͻ�������������߳�
//t1.join();	//join �Ὣ���̼߳��뵽���߳��У�Ȼ�����̻߳�ȴ����߳����������У�����

//���߳��У���ҪƵ����ȥ���������ͽ���  ���������ĺܶ�ʱ��
//lock_guard<mutex> lg(m);	//�Խ���  ���� �ٹ����ʱ���Լ�����  ������ʱ�����


//������ ����Ҫ���̣߳��߳��޷�������� -> ����ʹ���߳�֮������->Ƶ������������ʱ�����Ĺ���
//->���������������->��Ϊ�Ż��ķ�������->ԭ�Ӳ���

//atomic<int> sum = 0;	//atomic_int sum = 0;	//��sum��Ϊ��������Դ ���Ǹ���ԭ�Ӳ����������Ͳ��������ˣ���������Ѿ���������
