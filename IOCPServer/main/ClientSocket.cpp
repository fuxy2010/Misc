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
void CClientSocket::DataDispose()//�������ݴ���,����2000����Э��
{

}
void CClientSocket::ReceiveData(const void* pV,int L)//�������ݵ����ն���
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
void CClientSocket::SendData(const void *pV,int L)//����iocp����������
{
	m_pOServer->SendData(m_sFarSokAddr,pV,L);
}
