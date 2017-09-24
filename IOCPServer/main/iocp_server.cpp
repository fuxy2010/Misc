#include "stdafx.h"
#include "iocp_server.h"
#include "Config.h"
#include "ShowData.h"
#include "ClientSocket.h"
#include "ParseTask.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ScheduleServer;

CIOCPServer::CIOCPServer()
{

	m_pListBaseIO=NULL;
	m_pListClient=NULL;
	m_cNode_AcceptEx=NULL;
	m_hListenSocket=NULL;
	m_hIOCP=NULL;
	m_pOLog=CWriteLog::Instance();

	InitializeCriticalSection(&m_CriSnC);
	InitializeCriticalSection(&m_CriSnCB);
	InitializeCriticalSection(&m_CriSnIOB);
}
CIOCPServer::~CIOCPServer()
{
	if(!m_cNode_AcceptEx)
	{
		HeapFree(GetProcessHeap(), 0, m_cNode_AcceptEx);//�ͷ�AcceptEx������ڵ�
	}
	//�ͷŸ��洢�ṹ
	CIOCPServer::PPER_HANDLE_CONTEXT cNode;
	CIOCPServer::PPER_IO_CONTEXT ioNode;
	//�ر�����socket
	if (!m_hListenSocket)
	{
		shutdown(m_hListenSocket,2);
		closesocket(m_hListenSocket);
	}	
	//�رո��ͻ���socket
	while(m_pListClient!=NULL)
	{
		shutdown(m_pListClient->LinkSocket,2);
		closesocket(m_pListClient->LinkSocket);
		cNode=m_pListClient;
		m_pListClient=m_pListClient->pNext;
		if (!cNode->pDisposeDtr)
		{
			delete cNode->pDisposeDtr;
		}
		
		HeapFree(GetProcessHeap(), 0, cNode);
	}
	
	while(m_pListBaseIO!=NULL)
	{
		ioNode=m_pListBaseIO;
		m_pListBaseIO=m_pListBaseIO->pNext;
		HeapFree(GetProcessHeap(), 0, ioNode);
		
	}
	///////////
	if (!m_hIOCP)
	{

		CloseHandle(m_hIOCP);
	}
	DeleteCriticalSection(&m_CriSnC);
	DeleteCriticalSection(&m_CriSnCB);
	DeleteCriticalSection(&m_CriSnIOB);
	
	WSACleanup();
}
CString CIOCPServer::IntToHexStr(char* phead, int length)
{
	CString str;
	CString S="";
	str="";
	int i;
	for(i=0;i<length;i++)
	{
		str.Format(_T("%x"),phead[i]);
		S+=str;
	}
	return S;
}
///�������߳��ຯ��ʵ��
DWORD __stdcall CIOCPServer::WorkThread(void* p_OIocp)//�������߳̿�,Ϊ���ܽ��ຯ����Ϊ�̺߳�������
{
	CIOCPServer* p=(CIOCPServer*)p_OIocp;
	p->TrueWorkThread();
	return 0;
}

void CIOCPServer::TrueWorkThread()//ʵ�ʹ����̺߳���
{
	BOOL bBack;
	DWORD dwNumberBytes;
	PPER_HANDLE_CONTEXT cNode=NULL;
	PPER_IO_CONTEXT ioNode=NULL;
	PPER_IO_CONTEXT ioNodeNew=NULL;
	LPWSAOVERLAPPED lpOverlapped;

	SOCKADDR*   remote   ;   
	SOCKADDR*   local   ;   
	int   remote_len =   sizeof(   SOCKADDR_IN   );   
	int   local_len =   sizeof(   SOCKADDR_IN   );   
	SOCKADDR_IN   addr;   
	
	CString sLogData;
	CString sLog;//д��־��
	CConfig* pOConfig=CConfig::Instance();//�����ļ�
	CShowData* pOShow=CShowData::Instance();//������ʾ

	int iResult;
	DWORD dwIosize = 0;
	DWORD dwFlags =0;
	
	while(true)
	{
		bBack = GetQueuedCompletionStatus(
           m_hIOCP,
           &dwNumberBytes,
           (LPDWORD)&cNode,
           &lpOverlapped,
           INFINITE
           );
		if (NULL==cNode)
		{
			break;//�յ�������,�˳��߳�
		}
		if (FALSE == bBack)
		{
			
			//sLog.Format("GetQueuedCompletionStatus()���ؼ�¼:%d",WSAGetLastError());
			//m_pOLog->WriteLog(sLog.GetBuffer(0));
			continue;//��������,����
			
		}
		
		
		ioNode=(PPER_IO_CONTEXT)lpOverlapped;//ȡ��io�ڵ�

		switch(ioNode->IoOperation)//���ݴ����Ĳ������ͽ��в���
		{
		case IoAccept:			
			//��¼����ʱ��,����ͻ�������,�����������������ݲ�����
			cNode=ioNode->pCNode;//�������ݽ�����cNode�����ͬ����socketһ�������cNode,û��ȷ����ֵ,����Ӧ���ӵ�cNode
			ioNode->pCNode=NULL;//�����Ժ�˱�������,�ÿ�
			cNode->ConnetTime=CTime::GetCurrentTime();//��¼����ʱ��
			//�������ӵ�ַ
			lpfnGetAcceptExSockaddrs(   
				ioNode->RBuffer,   
				0,   
				sizeof(SOCKADDR_IN)   +   16,   
				sizeof(SOCKADDR_IN)   +   16,   
				&local,   
				&local_len,   
				&remote,   
				&remote_len);   
			memcpy(   &addr,   remote,   sizeof(SOCKADDR_IN));
			cNode->FarSokAddr =addr;//��¼�Է���ַ
			if (pOConfig->m_bLog)
			{
				sLog.Format("�ͻ���(%s:%d)������",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
				m_pOLog->WriteLog(sLog.GetBuffer(0));
			}
			sLog.Format("%s:%d",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
			pOShow->ShowData(sLog,"",0,cNode->ConnetTime,cNode->ReceiveTime,"");
			//Ͷ��һ��������,�ȴ���������,ע��Ͷ��ǰһ��Ҫ���㻺������ղ�������
			ioNode->IoOperation=IoRead;
			ZeroMemory(ioNode->RBuffer, BUFFER_SMALL_SIZE);
			ZeroMemory(&(ioNode->ol), sizeof(ioNode->ol));

			iResult = 0;
			iResult = WSARecv(cNode->LinkSocket, 
				&(ioNode->wsaRBuffer),
				1,
				NULL,
				&dwFlags,
				&(ioNode->ol), 
				NULL
				);
			if(SOCKET_ERROR  == iResult && ERROR_IO_PENDING  != WSAGetLastError() )
			{
				//������Ϣ��Զд��־
				sLog.Format("��һ��Ͷ��WSARecv����ʧ��!����ԭ��:%d",WSAGetLastError());
				m_pOLog->WriteLog(sLog.GetBuffer(0));
				sLog.Format("%s:%d",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
				pOShow->ShowData(sLog,"",1,cNode->ConnetTime,cNode->ReceiveTime,"Ͷ�ݶ�����ʧ��,�ر�socket,�ȴ�����...");
				//io�ڵ����,c�ڵ��ͷ�,socket�ر�
				InsertToListBase(ioNode);
				shutdown(cNode->LinkSocket,2);
				closesocket(cNode->LinkSocket);
				RemoveFromListClient(cNode);
				HeapFree(GetProcessHeap(), 0, cNode);
				continue;
			}
			//���ɿͻ��˶���,���������Э��װ��
			cNode->pDisposeDtr=new CClientSocket(this,cNode->FarSokAddr);//ԭʼ����
			//fym cNode->pDisposeDtr=new CGuangZhouMobile_InterfaceB(cNode->pDisposeDtr);//װ��,���µ�װ������ж��ٴ�װ��
		
			//����
			break;
		case IoRead:
			if(0==dwNumberBytes)
			{
				if (pOConfig->m_bLog)
				{
					sLog.Format("�ͻ���(%s:%d)�ѶϿ�",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
					m_pOLog->WriteLog(sLog.GetBuffer(0));
				}
				cNode->ConnetTime=CTime::GetCurrentTime();//��ʱΪ�Ͽ�ʱ��
				sLog.Format("%s:%d",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
				pOShow->ShowData(sLog,"",1,cNode->ConnetTime,cNode->ReceiveTime,"�Է����ӶϿ�");
				
				//io�ڵ����,c�ڵ��ͷ�,socket�ر�
				InsertToListBase(ioNode);
				shutdown(cNode->LinkSocket,2);
				closesocket(cNode->LinkSocket);
				if(cNode->pDisposeDtr)
				{
					delete cNode->pDisposeDtr;
				}
				RemoveFromListClient(cNode);
				HeapFree(GetProcessHeap(), 0, cNode);
			}
			else
			{
				//���յ�����д��־,����
				if(pOConfig->m_bLog)
				{
					//fym Recv Data
					//fym sLogData.Format("���(%s:%d) %d %s", inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port),dwNumberBytes, IntToHexStr(ioNode->wsaRBuffer.buf,dwNumberBytes));
					sLogData.Format("��� <%s> %d %s", CParseTask::parse_id(ioNode->wsaRBuffer.buf).c_str(), dwNumberBytes, IntToHexStr(ioNode->wsaRBuffer.buf, dwNumberBytes));
					//m_pOLog->WriteLog(sLogData);

					//////////////////////////////////////////////////////////////////////////
					PARSE_TASK_INFO task_info;
					task_info.task_id = timeGetTime();
					task_info.msg = new char[dwNumberBytes];
					memcpy(task_info.msg, ioNode->wsaRBuffer.buf, dwNumberBytes);
					task_info.length = dwNumberBytes;

					CParseTask* task = new CParseTask(task_info);

					if(SS_NoErr != SINGLETON(CScheduleServer).add_task(task, task_info.task_id))
					{
						delete task;
						task = NULL;
					}
					//////////////////////////////////////////////////////////////////////////
				}
				cNode->ReceiveTime=CTime::GetCurrentTime();//��¼���һ�ν������ݵ�ʱ��
				sLog.Format("%s:%d",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
				pOShow->ShowData(sLog,"",0,cNode->ConnetTime,cNode->ReceiveTime,sLogData);
			
				cNode->pDisposeDtr->ReceiveData(ioNode->wsaRBuffer.buf,dwNumberBytes);//���ݽ���
				cNode->pDisposeDtr->DataDispose();//���ݴ���
				//Ͷ�ݶ�����
				ioNode->IoOperation=IoRead;
				ZeroMemory(ioNode->RBuffer, BUFFER_SMALL_SIZE);
				ZeroMemory(&(ioNode->ol), sizeof(ioNode->ol));
				iResult = 0;
				iResult = WSARecv(cNode->LinkSocket, 
					&(ioNode->wsaRBuffer),
					1,
					&dwIosize,
					&dwFlags,
					&(ioNode->ol), 
					NULL
					);
				
				//�ж�Ͷ�ݽ��
				if(SOCKET_ERROR  == iResult && ERROR_IO_PENDING  != WSAGetLastError() )
				{
					//������Ϣ��Զд��־
					sLog.Format("Ͷ��WSARecv����ʧ��!����ԭ��:%d",WSAGetLastError());
					m_pOLog->WriteLog(sLog.GetBuffer(0));
					sLog.Format("%s:%d",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
					pOShow->ShowData(sLog,"",1,cNode->ConnetTime,cNode->ReceiveTime,"Ͷ�ݶ�����ʧ��,�ر�socket,�ȴ�����...");
					//�ӿͻ��������Ƴ�
					RemoveFromListClient(cNode);
					//io�ڵ����,c�ڵ��ͷ�,socket�ر�
					InsertToListBase(ioNode);
					shutdown(cNode->LinkSocket,2);
					closesocket(cNode->LinkSocket);
					delete cNode->pDisposeDtr;
					RemoveFromListClient(cNode);
					HeapFree(GetProcessHeap(), 0, cNode);
					continue;
				}
			}
			break;	
		case IoWrite:
			//�������ݷ���,����ioNode
			if (pOConfig->m_bLog)
			{
				cNode->ReceiveTime=CTime::GetCurrentTime();
				sLogData.Format("���(%s:%d)  %s",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port),IntToHexStr(ioNode->wsaSBuffer.buf,ioNode->wsaSBuffer.len));
				m_pOLog->WriteLog(sLogData.GetBuffer(0));
				sLog.Format("%s:%d",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
				pOShow->ShowData(sLog,"",0,cNode->ConnetTime,cNode->ReceiveTime,sLogData);
			}
			InsertToListBase(ioNode);
			
			break;
		default://�����ʹ���,��Ҫ����־
			//��¼��־,����ioNode
			sLog.Format("�ͻ���(%s:%d)���ַǷ�IO��������",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
			m_pOLog->WriteLog(sLog.GetBuffer(0));
			
			InsertToListBase(ioNode);
			break;
		}//switch end
	}//while end
}
////////////////////////////////////////////////////////////////////


//�����߳��ຯ����ʵ��,ͬʱ���˸������Ĵ���,�Ǻ�
DWORD __stdcall CIOCPServer::ListionThread(void* p_LT)//�����߳̿�
{
	PLISTION_THREAD p=(PLISTION_THREAD)p_LT;
	p->p_OIocp->TrueListionThread(p->AccN);
	return 0;
}

void CIOCPServer::TrueListionThread(int AccN)//ʵ�������̺߳���
{
	while(true)
	{		
		if (WaitForSingleObject(m_hEvent_Exit_Listion,0)==WAIT_OBJECT_0)
		{
			CloseHandle(m_hEvent_Accept);
			CloseHandle(m_hEvent_Exit_Listion);
			break; //�˳��߳�
		}
		if(WAIT_OBJECT_0==WaitForSingleObject(m_hEvent_Accept, 0))
		{
			PostAcceptEx(AccN);
		}	
	}
}

//��ʼ������
BOOL CIOCPServer::initIocp(CString IP,int Port,int ThreadCount,int AccN)
{
	CString sLog;
	//�����׽��ֿ�
	WSADATA wsd;
	DWORD nResult = WSAStartup(MAKEWORD(2,2), &wsd);
	if (0 != nResult)
	{
		m_pOLog->WriteLog("�����׽��ֿ�2.2ʧ��");
		return FALSE;
	}
	//���������˿�
	m_hListenSocket=WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, 
          NULL, 0, WSA_FLAG_OVERLAPPED); 
    if (INVALID_SOCKET == m_hListenSocket)
    {
		m_pOLog->WriteLog("��������������socketʧ��");
        WSACleanup();
		return FALSE;
    }
	SOCKADDR_IN InternetAddr;
    InternetAddr.sin_family = AF_INET;
    InternetAddr.sin_addr.s_addr = inet_addr(IP);
    InternetAddr.sin_port = htons(Port);    
    nResult = bind(m_hListenSocket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr));
    if (SOCKET_ERROR == nResult)
    {
		shutdown(m_hListenSocket,2);
		closesocket(m_hListenSocket);
		WSACleanup();
		m_pOLog->WriteLog("��socket��ַʧ��");
        return FALSE;
    } 
	nResult = listen(m_hListenSocket, 20);
    if (SOCKET_ERROR == nResult)
    {
		shutdown(m_hListenSocket,2);
        closesocket(m_hListenSocket);
		WSACleanup();
        m_pOLog->WriteLog("����socketʧ��");
        return FALSE;
    }  

	//�����õ�AcceptEx,TransmitFile,GetAcceptExSockaddrs����,���ڵ���Ч�ʵ�����,������������������չ����ָ��
	GUID GUIDAcceptEx = WSAID_ACCEPTEX;
	GUID GUIDTransmitFile = WSAID_TRANSMITFILE;
	GUID GUIDGetAcceptExSockaddrs=WSAID_GETACCEPTEXSOCKADDRS;
	
	DWORD dwResult;
	nResult = WSAIoctl( 
		m_hListenSocket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GUIDAcceptEx,
		sizeof(GUIDAcceptEx),
		&lpAcceptEx,
		sizeof(lpAcceptEx),
		&dwResult,
		NULL,
		NULL
		);

	if (SOCKET_ERROR == nResult)
	{
		shutdown(m_hListenSocket,2);
		closesocket(m_hListenSocket);
		m_pOLog->WriteLog("��ȡAcceptEx������չָ��ʧ��");
		return FALSE;
	}

	nResult = WSAIoctl( 
		m_hListenSocket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GUIDTransmitFile,
		sizeof(GUIDTransmitFile),
		&lpTransmitFile,
		sizeof(lpTransmitFile),
		&dwResult,
		NULL,
		NULL
		);

	if (SOCKET_ERROR == nResult)
	{
		shutdown(m_hListenSocket,2);
		closesocket(m_hListenSocket);
		m_pOLog->WriteLog("��ȡTransmitFile������չָ��ʧ��");
		return FALSE;
	}
	nResult = WSAIoctl(   
        m_hListenSocket,   
        SIO_GET_EXTENSION_FUNCTION_POINTER,   
        &GUIDGetAcceptExSockaddrs,   
        sizeof(GUIDGetAcceptExSockaddrs),   
        &lpfnGetAcceptExSockaddrs,   
        sizeof(lpfnGetAcceptExSockaddrs),   
        &dwResult,   
        NULL,   
        NULL);
	if (SOCKET_ERROR == nResult)
	{
		shutdown(m_hListenSocket,2);
		closesocket(m_hListenSocket);
		m_pOLog->WriteLog("��ȡGetAcceptExSockaddrs������չָ��ʧ��");
		return FALSE;
	}
	//��ʼ�������߳�Accept�¼�����
	m_hEvent_Accept = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(NULL==m_hEvent_Accept)
	{
		m_pOLog->WriteLog("�����¼�m_hEvent_Acceptʧ��");
		return FALSE;
	}
	nResult = WSAEventSelect(m_hListenSocket, m_hEvent_Accept, FD_ACCEPT);
	if (SOCKET_ERROR == nResult)
	{
		CloseHandle(m_hEvent_Accept);
		m_pOLog->WriteLog("�����¼�m_hEvent_Acceptʧ��");
		return FALSE;
	}
	int i;
	//����io���ýڵ�
	CIOCPServer::PPER_IO_CONTEXT ioNode;
	for(i=0;i<INBASE_SIZE;i++)
	{
		ioNode=(PPER_IO_CONTEXT)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PER_IO_CONTEXT));
		if(NULL==ioNode)
		{
			m_pOLog->WriteLog("����initIocp()������ioNode�ڴ�ʧ��");
			return FALSE;
		}
		
		ZeroMemory(ioNode->RBuffer, BUFFER_SMALL_SIZE);
		ZeroMemory(ioNode->SBuffer, BUFFER_SMALL_SIZE);
		ZeroMemory(&(ioNode->ol), sizeof(ioNode->ol));
		ioNode->wsaRBuffer.buf=ioNode->RBuffer;
		ioNode->wsaRBuffer.len=BUFFER_SMALL_SIZE;
		ioNode->wsaSBuffer.buf=ioNode->SBuffer;
		ioNode->wsaSBuffer.len=BUFFER_SMALL_SIZE;
		//���뱸������
		ioNode->pNext=m_pListBaseIO;
		m_pListBaseIO=ioNode;
	}
	//������ɶ˿�
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (NULL == m_hIOCP)
	{
		m_pOLog->WriteLog("������ɶ˿�ʧ��");
		return FALSE;
	}
	//��socket����ɶ˿ڹ���,�˲�������,���������߳����ղ���Accept������ź�
	m_cNode_AcceptEx=(PPER_HANDLE_CONTEXT)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PER_HANDLE_CONTEXT));
	if(NULL==m_cNode_AcceptEx)
	{
		m_pOLog->WriteLog("m_cNode_AcceptEx����ʧ��");
		return FALSE;
	}
	//����
	HANDLE hResult = CreateIoCompletionPort(
       (HANDLE)m_hListenSocket,
       m_hIOCP,
       (DWORD)m_cNode_AcceptEx,//�˽ڵ�����,AcceptEX��,��Ӧ�ڵ���Ϣ��ioNode��
       0);
	if (NULL == hResult)
	{
		m_pOLog->WriteLog("��ɶ˿�������Socket����ʧ��");
		return FALSE;
	}
	
	//�ȷ�������AcceptEx����
	if (FALSE==PostAcceptEx(AccN))
	{
		m_pOLog->WriteLog("initIocp��������PostAcceptEx����ʧ��");
		return FALSE;
	};
	//���������߳��˳��¼�
	m_hEvent_Exit_Listion=CreateEvent(NULL,TRUE,FALSE,NULL);
	//�����������߳�,����AccN�����Ĵ����и�С����,����IO��,�Ǻ�
	PLISTION_THREAD p=(PLISTION_THREAD)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LISTION_THREAD));
	if(NULL==p)
	{
		m_pOLog->WriteLog("����initIocp()������p�ڴ�ʧ��");
		return FALSE;
	}
	p->p_OIocp=this;
	p->AccN=AccN;
	HANDLE hThr=CreateThread(NULL,0,CIOCPServer::ListionThread,(void*)p,0,NULL);
	if (NULL == hThr)
	{
		CloseHandle(hThr);
		m_pOLog->WriteLog("���������߳�ʧ��");
		return FALSE;
	}
	m_pOLog->WriteLog("���������߳�");
	CloseHandle(hThr);
	//�����������߳�
	for (i=0; i<ThreadCount; i++)
	{
		HANDLE hThr= CreateThread(NULL, 0, CIOCPServer::WorkThread, (void*)this, 0, NULL);
		if (NULL == hThr)
		{
			CloseHandle(hThr);
			m_pOLog->WriteLog("�����������߳�ʧ��");
			CloseHandle(m_hIOCP);
			return FALSE;
		}
		sLog="";
		sLog.Format("�����������߳�%d",i);
		m_pOLog->WriteLog(sLog);
		CloseHandle(hThr);
		
	}

	//����
	return TRUE;
}
BOOL CIOCPServer::PostAcceptEx(int AccN)//������������AcceptEx����
{

	for(int i=0;i<AccN;i++)
	{
		//����һ��cNode
		CIOCPServer::PPER_HANDLE_CONTEXT cNode=GetCNode();
		if(NULL==cNode)
		{
			m_pOLog->WriteLog("PostAcceptEx����������cNodeʧ��");
			continue;
		}
		//��socket����ɶ˿ڹ���
		HANDLE hResult = CreateIoCompletionPort(
           (HANDLE)cNode->LinkSocket,
           m_hIOCP,
           (DWORD)cNode,
           0);
		if (NULL == hResult)
		{
			int nResult = WSAGetLastError();
			if(nResult!=ERROR_INVALID_PARAMETER)//�Ѿ�����ɶ˿ڰ󶨹���socket�᷵�ش˴���
			{
				m_pOLog->WriteLog("PostAcceptEx�����а���ɶ˿�ʧ��");
				
				shutdown(cNode->LinkSocket,2);
				closesocket(cNode->LinkSocket);
				if(cNode->pDisposeDtr)
				{
					delete cNode->pDisposeDtr;
				}
				HeapFree(GetProcessHeap(), 0, cNode);
				continue;
			}
		}
		//����һ��ioNode
		CIOCPServer::PPER_IO_CONTEXT ioNode=GetIONode();
		if(NULL==ioNode)
		{
			m_pOLog->WriteLog("PostAcceptEx����������ioNodeʧ��");
			InsertToListBase(ioNode);
			shutdown(cNode->LinkSocket,2);
			closesocket(cNode->LinkSocket);
			if(cNode->pDisposeDtr)
			{
				delete cNode->pDisposeDtr;
			}
			HeapFree(GetProcessHeap(), 0, cNode);
			continue;
		}
		//��cNode����ioNode
		ioNode->pCNode=cNode;//�����и�ֵ,��ΪioNode��û����������ͻ��˽ڵ��ڴ�
		DWORD dwBytes;
		//����һ��AcceptEx
		BOOL bSuccess = lpAcceptEx(
			m_hListenSocket,
			ioNode->pCNode->LinkSocket,
			ioNode->RBuffer,
			0,//�������Ӽ�����//BUFFER_SMALL_SIZE - (sizeof(SOCKADDR_IN) + 16) * 2,
			sizeof(SOCKADDR_IN) + 16,
			sizeof(SOCKADDR_IN) + 16,
			&dwBytes,
			&(ioNode->ol));//����io�ڵ�,���а�����ӦcNode��ָ��

		if (FALSE == bSuccess)
		{
			int nResult = WSAGetLastError();
			if (nResult != ERROR_IO_PENDING)//�ص�io����������
			{
				m_pOLog->WriteLog("PostAcceptEx������AcceptExʧ��");
				InsertToListBase(ioNode);
				shutdown(cNode->LinkSocket,2);
				closesocket(cNode->LinkSocket);
				if(cNode->pDisposeDtr)
				{
					delete cNode->pDisposeDtr;
				}
				HeapFree(GetProcessHeap(), 0, cNode);
				continue;
			}
		}
		//����ͻ�������
		InsertToListClient(cNode);
	}//for end
	
	return TRUE;
}
CIOCPServer::PPER_HANDLE_CONTEXT CIOCPServer::GetCNode()//�ӱ����б�ȡ��һ��Client�ڵ�
{
	PPER_HANDLE_CONTEXT cNode;
	cNode=(CIOCPServer::PPER_HANDLE_CONTEXT)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CIOCPServer::PER_HANDLE_CONTEXT));
	if(NULL==cNode)
	{
		m_pOLog->WriteLog("����GetcNode()������cNode�ڴ�ʧ��");
		return NULL;
	}
	SOCKET CSocket= WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED); 
	if (INVALID_SOCKET == CSocket)
	{
		m_pOLog->WriteLog("����GetcNode()�д���Socketʧ��");
		return NULL;
	}
	cNode->LinkSocket=CSocket;

	
	//��ʼ���ڵ�
	cNode->pFront=NULL;
	cNode->pNext=NULL;
	cNode->ConnetTime=0;
	cNode->ReceiveTime=0;
	cNode->pDisposeDtr=NULL;	
	ZeroMemory(&(cNode->FarSokAddr), sizeof(cNode->FarSokAddr));
	return cNode;
}
CIOCPServer::PPER_IO_CONTEXT CIOCPServer::GetIONode()//�ӱ����б�ȡ��һ��io�ڵ�
{

	CWriteLog* OLog=CWriteLog::Instance();
	PPER_IO_CONTEXT ioNode;
EnterCriticalSection(&m_CriSnIOB);
	if (m_pListBaseIO!=NULL)
	{
		ioNode=m_pListBaseIO;
		m_pListBaseIO=m_pListBaseIO->pNext;	

LeaveCriticalSection(&m_CriSnIOB);
	}
	else
	{
		ioNode=(PPER_IO_CONTEXT)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PER_IO_CONTEXT));
		if(NULL==ioNode)
		{
			OLog->WriteLog("����GetIONode()������ioNode�ڴ�ʧ��");
			return NULL;
		}
	}
	
	ioNode->pNext=NULL;
	ioNode->IoOperation=IoAccept;
	ZeroMemory(ioNode->RBuffer, BUFFER_SMALL_SIZE);
	ZeroMemory(ioNode->SBuffer, BUFFER_SMALL_SIZE);
	ZeroMemory(&(ioNode->ol), sizeof(ioNode->ol));
	ioNode->wsaRBuffer.buf=ioNode->RBuffer;
	ioNode->wsaRBuffer.len=BUFFER_SMALL_SIZE;
	ioNode->wsaSBuffer.buf=ioNode->SBuffer;
	ioNode->wsaSBuffer.len=BUFFER_SMALL_SIZE;
	return ioNode;

}
void CIOCPServer::InsertToListBase(CIOCPServer::_PER_IO_CONTEXT* pIoNode)//���ڵ���뱸��������
{
	if(pIoNode!=NULL)
	{
		EnterCriticalSection(&m_CriSnIOB);
		pIoNode->pNext=m_pListBaseIO;
		m_pListBaseIO=pIoNode;
		LeaveCriticalSection(&m_CriSnIOB);
	}
}
void CIOCPServer::InsertToListClient(CIOCPServer::_PER_HANDLE_CONTEXT* pHandleNode)//���ڵ����ͻ���������,��ͬʱָ��ǰ�ýڵ�
{
	if(pHandleNode!=NULL)
	{
		EnterCriticalSection(&m_CriSnC);
		pHandleNode->pNext=m_pListClient;
		if (m_pListClient!=NULL)
			{m_pListClient->pFront=pHandleNode;}
		m_pListClient=pHandleNode;
		//g_iClientNum++;
		LeaveCriticalSection(&m_CriSnC);
	}
}
void CIOCPServer::RemoveFromListClient(CIOCPServer::_PER_HANDLE_CONTEXT* pHandleNode)//���ڵ��Ƴ��ͻ���������,��ͬʱָ��ǰ�ýڵ�
{

	if(pHandleNode!=NULL)
	{
		EnterCriticalSection(&m_CriSnC);
		if(pHandleNode==m_pListClient)
		{
			m_pListClient=m_pListClient->pNext;
		}
		else
		{
			if(pHandleNode->pFront!=NULL)
			{
				pHandleNode->pFront->pNext=pHandleNode->pNext;
			}
			if (pHandleNode->pNext!=NULL)
			{
				pHandleNode->pNext->pFront=pHandleNode->pFront;
			}
		}
		//g_iClientNum--;
		LeaveCriticalSection(&m_CriSnC);
	}
}
void CIOCPServer::SendData(SOCKADDR_IN FarSokAddr,const void* pV,int nL)//��������
{
	CString sLog;//д��־��
	CShowData* pOShow=CShowData::Instance();//������ʾ

	PPER_HANDLE_CONTEXT cNode=NULL;
	cNode=m_pListClient;
	while (cNode!=NULL)
	{
		if(inet_ntoa(cNode->FarSokAddr.sin_addr)==inet_ntoa(FarSokAddr.sin_addr) && ntohs(cNode->FarSokAddr.sin_port)==ntohs(FarSokAddr.sin_port) )
		{
			PPER_IO_CONTEXT ioNode=GetIONode();
			if (NULL==ioNode)
			{
				sLog.Format("��ͻ���(%s:%d)��������ʧ��,ԭ��:����ioNodeʧ��",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
				m_pOLog->WriteLog(sLog.GetBuffer(0));
				return;
			}
			memcpy(ioNode->wsaSBuffer.buf,pV,nL);
			ioNode->wsaSBuffer.len=nL;
			ioNode->IoOperation=IoWrite;

			int nResult;
			DWORD dwIosize = 0;
			DWORD dwFlags =0;
			
			nResult = WSASend(cNode->LinkSocket,
				&(ioNode->wsaSBuffer),
				1,
				&dwIosize,
				0,
				&(ioNode->ol),
				NULL
				);
			if((SOCKET_ERROR==nResult) && (ERROR_IO_PENDING != WSAGetLastError()))
			{
				sLog.Format("��ͻ���(%s:%d)��������ʧ��,������:%l",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port),WSAGetLastError());
				m_pOLog->WriteLog(sLog.GetBuffer(0));
				//����ʧ��,socketӦ��������
				//io�ڵ����,c�ڵ��ͷ�,socket�ر�
				sLog.Format("%s:%d",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
				pOShow->ShowData(sLog,"",1,cNode->ConnetTime,cNode->ReceiveTime,"��������ʧ��,�ر�socket,�ȴ�����...");
				
				InsertToListBase(ioNode);
				shutdown(cNode->LinkSocket,2);
				closesocket(cNode->LinkSocket);
				if(cNode->pDisposeDtr)
				{
					delete cNode->pDisposeDtr;
				}
				HeapFree(GetProcessHeap(), 0, cNode);
				return ;
			}
			return ;
		}
		cNode=cNode->pNext;
	}
}