#if !defined(Client_2010_07_15_14_44_54_mxj)
#define Client_2010_07_15_14_44_54_mxj

#if _MSC_VER >1000
#pragma once
#endif

#define    BUFFER_BIG_SIZE           4096//����4k

class CClient//�ͻ��������
{
protected:
	typedef struct _RECEIVE_DATA_QUEUE  //�洢�������ݵĶ���
	{
		char ReceiveByte[BUFFER_BIG_SIZE];
		DWORD Head;
		DWORD Tail;
		_RECEIVE_DATA_QUEUE()
		{
			Head=0;
			Tail=0;
		}
	}RECEIVE_DATA_QUEUE,*PRECEIVE_DATA_QUEUE;
public:
	virtual PRECEIVE_DATA_QUEUE GetBuff()=0;
	virtual void DataDispose()=0;//������ն����е�����
	virtual void ReceiveData(const void* pV,int L)=0;//��������,constֻ��Ϊ�˱�������������岻���ı�
	virtual void SendData(const void* pV,int L)=0;//��������
};

#endif