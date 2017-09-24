#if !defined(Client_2010_07_15_14_44_54_mxj)
#define Client_2010_07_15_14_44_54_mxj

#if _MSC_VER >1000
#pragma once
#endif

#define    BUFFER_BIG_SIZE           4096//缓存4k

class CClient//客户端虚基类
{
protected:
	typedef struct _RECEIVE_DATA_QUEUE  //存储接收数据的队列
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
	virtual void DataDispose()=0;//处理接收队列中的数据
	virtual void ReceiveData(const void* pV,int L)=0;//接收数据,const只是为了保护传入参数本体不被改变
	virtual void SendData(const void* pV,int L)=0;//发送数据
};

#endif