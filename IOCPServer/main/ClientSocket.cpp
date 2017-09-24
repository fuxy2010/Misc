#include "stdafx.h"
#include "ClientSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CClientSocket::CClientSocket(CIOCPServer* pOServer,SOCKADDR_IN SokAddr )
{
	m_pRBuff=(CClientSocket::PRECEIVE_DATA_QUEUE)HeapAlloc(
		GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CClientSocket::RECEIVE_DATA_QUEUE));
	m_pOServer=pOServer;
	m_sFarSokAddr=SokAddr;
}
CClientSocket::~CClientSocket()
{
	HeapFree(GetProcessHeap(), 0, m_pRBuff);
}
CClientSocket::PRECEIVE_DATA_QUEUE CClientSocket::GetBuff()
{
	return m_pRBuff;
}
void CClientSocket::DataDispose()//基本数据处理,比如2000接收协议
{

}
void CClientSocket::ReceiveData(const void* pV,int L)//加入数据到接收队列
{
	int i;
	char* pC;
	pC=(char*)pV;
	for(i=0;i<L;i++)
	{
		m_pRBuff->ReceiveByte[m_pRBuff->Head]=pC[i];
		m_pRBuff->Head=(m_pRBuff->Head+1) % BUFFER_BIG_SIZE;
	}
}
void CClientSocket::SendData(const void *pV,int L)//调用iocp对象发送数据
{
	m_pOServer->SendData(m_sFarSokAddr,pV,L);
}
