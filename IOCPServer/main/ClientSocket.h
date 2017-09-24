#if !defined(ClientSocket_2010_07_15_15_19_04_mxj)
#define ClientSocket_2010_07_15_15_19_04_mxj

#if _MSC_VER >1000
#pragma once
#endif

#define    BUFFER_BIG_SIZE           4096//����4k

#include "Client.h"
#include "iocp_server.h"

class CClientSocket:public CClient//socket���͵Ŀͻ���,Ҫʵ�ֻ��������ݴ�����
{
private:
	

	PRECEIVE_DATA_QUEUE       m_pRBuff;//�������ݴ洢����
	CIOCPServer*			  m_pOServer;//��ɶ˿ڷ���������
	SOCKADDR_IN               m_sFarSokAddr;//Զ�˿ͻ���soket��ַ(�����˿�)
public:
	CClientSocket(CIOCPServer* pOServer,SOCKADDR_IN SokAddr);
	~CClientSocket();
	PRECEIVE_DATA_QUEUE GetBuff();
	void DataDispose();//���������ݴ���,��չ�����ݴ���������װ������ʵ��
	void ReceiveData(const void* pV,int L);//��Ҫʵ�ֵĽ����ݼ�����ն��к���
	void SendData(const void* pV,int L);//��������,��ʵ��
};

#endif