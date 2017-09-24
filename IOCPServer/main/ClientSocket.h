#if !defined(ClientSocket_2010_07_15_15_19_04_mxj)
#define ClientSocket_2010_07_15_15_19_04_mxj

#if _MSC_VER >1000
#pragma once
#endif

#define    BUFFER_BIG_SIZE           4096//缓存4k

#include "Client.h"
#include "iocp_server.h"

class CClientSocket:public CClient//socket类型的客户端,要实现基本的数据处理函数
{
private:
	

	PRECEIVE_DATA_QUEUE       m_pRBuff;//接收数据存储队列
	CIOCPServer*			  m_pOServer;//完成端口服务器对象
	SOCKADDR_IN               m_sFarSokAddr;//远端客户端soket地址(包括端口)
public:
	CClientSocket(CIOCPServer* pOServer,SOCKADDR_IN SokAddr);
	~CClientSocket();
	PRECEIVE_DATA_QUEUE GetBuff();
	void DataDispose();//基本的数据处理,扩展的数据处理在其它装饰类中实现
	void ReceiveData(const void* pV,int L);//需要实现的将数据加入接收队列函数
	void SendData(const void* pV,int L);//发送数据,需实现
};

#endif