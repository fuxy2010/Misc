#if !defined(iocp_server_2010_07_14_08_44_40_mxj)
#define iocp_server_2010_07_14_08_44_40_mxj

#if _MSC_VER >1000
#pragma once
#endif

#pragma   comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <mswsock.h>
#include <afxsock.h>

#include "Client.h"
#include "WriteLog.h"

#define    BUFFER_LITTLE_SIZE        512//����512
#define    BUFFER_SMALL_SIZE         1024//����1k
#define    BUFFER_BIG_SIZE           4096//����4k
#define    CNBASE_SIZE               200//�ͻ��˱��ýڵ�����
#define    INBASE_SIZE				 500//IO���ýڵ�����

class CIOCPServer//��ɶ˿ڷ�������
{
private:
//������������

	//io����
	typedef enum _IO_OPERATION 
	{
	  IoAccept, //AcceptEx/accept
	  IoRead,   //WSARecv/recv/ReadFile
	  IoWrite   //WSASend/send/WriteFile
	}IO_OPERATION, *PIO_OPERATION;

	//����ɼ���(���������),����socket

	typedef struct _PER_HANDLE_CONTEXT
	{
		SOCKET                    LinkSocket;//socket��ʶ,ע���Ǳ�������Զ�˿ͻ��˵��׽��ֱ�ʶ
		SOCKADDR_IN               FarSokAddr;//Զ�˿ͻ���soket��ַ(�����˿�)
		CTime					  ConnetTime;//����ʱ��
		CTime					  ReceiveTime;//�ϴν�������ʱ��
		CClient*				  pDisposeDtr;//�ͻ��������ָ��

		_PER_HANDLE_CONTEXT*      pNext;//��ڵ�
		_PER_HANDLE_CONTEXT*      pFront;//ǰ�ڵ�

	}PER_HANDLE_CONTEXT, *PPER_HANDLE_CONTEXT;

	//��IO���ݣ���չ��WSAOVERLAPPED
	typedef struct _PER_IO_CONTEXT
	{
		WSAOVERLAPPED              ol;
		WSABUF                     wsaRBuffer;//���������û���
		WSABUF					   wsaSBuffer;//���������ݻ���
		char                       RBuffer[BUFFER_SMALL_SIZE];
		char                       SBuffer[BUFFER_SMALL_SIZE];
		IO_OPERATION               IoOperation;
		
		PPER_HANDLE_CONTEXT        pCNode;//ָ��ͻ��˽ڵ��ָ��,û�취,AcceptEXֻ�ܴ���һ��io�ڵ�
		_PER_IO_CONTEXT*           pNext; 
	}PER_IO_CONTEXT, *PPER_IO_CONTEXT;
	//ֻΪ�ܸ�TrueListionThread��������AccN����
	typedef struct _LISTION_THREAD  
	{
		CIOCPServer* p_OIocp;
		int AccN;	
	}LISTION_THREAD,*PLISTION_THREAD;
//��Ա����
	PPER_HANDLE_CONTEXT  m_pListClient;//�������ӿͻ���˫��ڵ�����
	PPER_IO_CONTEXT      m_pListBaseIO;//����io�ڵ�����
	
	CRITICAL_SECTION              m_CriSnC;//����ͻ�������ڵ�Ĺؼ������
	CRITICAL_SECTION              m_CriSnCB;//���뱸�ÿͻ�������ڵ�Ĺؼ������
	CRITICAL_SECTION              m_CriSnIOB;//���뱸��io�ڵ�����Ĺؼ������

	HANDLE                        m_hIOCP;//��ɶ˿ھ��
	SOCKET                        m_hListenSocket;//����socket

	HANDLE						  m_hEvent_Accept;//֪ͨAccept���úľ����¼�����
	HANDLE						  m_hEvent_Exit_Listion;//֪ͨ�����߳��˳����¼�����
	//������3����Ա�����ں���ָ��,���ԾͲ���m_��ͷ��
	LPFN_TRANSMITFILE             lpTransmitFile;//TransmitFile������չָ��
	LPFN_ACCEPTEX                 lpAcceptEx;//AcceptEx������չָ��
	LPFN_GETACCEPTEXSOCKADDRS	  lpfnGetAcceptExSockaddrs;//GetAcceptExSockaddres������չָ��  

	PPER_HANDLE_CONTEXT			  m_cNode_AcceptEx;//��Ӧ�����õ�һ��cNode����ָ��,û��,����ͷž�����

	CWriteLog*					  m_pOLog;
//��Ա����
	CString IntToHexStr(char* phead, int length);
	BOOL PostAcceptEx(int AccN);//������������AcceptEx���á�����Ϊ���õ�����
	
	PPER_HANDLE_CONTEXT GetCNode();//�ӱ����б�ȡ��һ��Client�ڵ�
	PPER_IO_CONTEXT     GetIONode();//�ӱ����б�ȡ��һ��io�ڵ�

	void InsertToListClient(PPER_HANDLE_CONTEXT pHandleNode);//���ڵ����ͻ���������
	void RemoveFromListClient(PPER_HANDLE_CONTEXT pHandleNode);//���ڵ�ӿͻ����������Ƴ�
	void InsertToListBase(PPER_IO_CONTEXT pIoNode);//���սڵ�
	
	
	static DWORD __stdcall WorkThread(void* p_OIocp);//�������߳̿�,Ϊ���ܽ��ຯ����Ϊ�̺߳�������
	void TrueWorkThread();//�������̺߳���
	static DWORD __stdcall ListionThread(void* p_LT);//�����߳̿�
	void TrueListionThread(int AccN);//�����߳�
public:
	CIOCPServer();
	~CIOCPServer();
	BOOL initIocp(CString IP,int Port,int ThreadCount,int AccN);//��ʼ��,������ɶ˿�,�������߳�,�����̵߳�,�贫������ip�������˿�,�������߳�������һ��AcceptEx������
	//ͬʱ��ʼ�����ýڵ�����,�ҵ��뷨�����������ɽڵ㱸��,������ʹ��.ɾ���Ľڵ�ͬʱ������������
	void SendData(SOCKADDR_IN SokAddr,const void* pV,int nL);//��������

};

#endif