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
		HeapFree(GetProcessHeap(), 0, m_cNode_AcceptEx);//释放AcceptEx用特殊节点
	}
	//释放各存储结构
	CIOCPServer::PPER_HANDLE_CONTEXT cNode;
	CIOCPServer::PPER_IO_CONTEXT ioNode;
	//关闭侦听socket
	if (!m_hListenSocket)
	{
		shutdown(m_hListenSocket,2);
		closesocket(m_hListenSocket);
	}	
	//关闭各客户端socket
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
///工作者线程类函数实现
DWORD __stdcall CIOCPServer::WorkThread(void* p_OIocp)//工作者线程壳,为了能将类函数做为线程函数创建
{
	CIOCPServer* p=(CIOCPServer*)p_OIocp;
	p->TrueWorkThread();
	return 0;
}

void CIOCPServer::TrueWorkThread()//实际工作线程函数
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
	CString sLog;//写日志用
	CConfig* pOConfig=CConfig::Instance();//配置文件
	CShowData* pOShow=CShowData::Instance();//界面显示

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
			break;//收到空数据,退出线程
		}
		if (FALSE == bBack)
		{
			
			//sLog.Format("GetQueuedCompletionStatus()返回记录:%d",WSAGetLastError());
			//m_pOLog->WriteLog(sLog.GetBuffer(0));
			continue;//其他错误,继续
			
		}
		
		
		ioNode=(PPER_IO_CONTEXT)lpOverlapped;//取得io节点

		switch(ioNode->IoOperation)//根据传进的操作类型进行操作
		{
		case IoAccept:			
			//记录连接时间,加入客户端链表,如果有数据则接收数据并处理
			cNode=ioNode->pCNode;//函数传递进来的cNode是最初同侦听socket一块关联的cNode,没有确定赋值,非响应连接的cNode
			ioNode->pCNode=NULL;//连接以后此变量不用,置空
			cNode->ConnetTime=CTime::GetCurrentTime();//记录连接时间
			//解析连接地址
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
			cNode->FarSokAddr =addr;//记录对方地址
			if (pOConfig->m_bLog)
			{
				sLog.Format("客户端(%s:%d)已连接",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
				m_pOLog->WriteLog(sLog.GetBuffer(0));
			}
			sLog.Format("%s:%d",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
			pOShow->ShowData(sLog,"",0,cNode->ConnetTime,cNode->ReceiveTime,"");
			//投递一个读操作,等待接收数据,注意投递前一定要清零缓冲否则收不到数据
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
				//错误信息永远写日志
				sLog.Format("第一次投递WSARecv请求失败!错误原因:%d",WSAGetLastError());
				m_pOLog->WriteLog(sLog.GetBuffer(0));
				sLog.Format("%s:%d",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
				pOShow->ShowData(sLog,"",1,cNode->ConnetTime,cNode->ReceiveTime,"投递读操作失败,关闭socket,等待重连...");
				//io节点回收,c节点释放,socket关闭
				InsertToListBase(ioNode);
				shutdown(cNode->LinkSocket,2);
				closesocket(cNode->LinkSocket);
				RemoveFromListClient(cNode);
				HeapFree(GetProcessHeap(), 0, cNode);
				continue;
			}
			//生成客户端对象,并进行相关协议装饰
			cNode->pDisposeDtr=new CClientSocket(this,cNode->FarSokAddr);//原始对象
			//fym cNode->pDisposeDtr=new CGuangZhouMobile_InterfaceB(cNode->pDisposeDtr);//装饰,有新的装饰类就有多少次装饰
		
			//返回
			break;
		case IoRead:
			if(0==dwNumberBytes)
			{
				if (pOConfig->m_bLog)
				{
					sLog.Format("客户端(%s:%d)已断开",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
					m_pOLog->WriteLog(sLog.GetBuffer(0));
				}
				cNode->ConnetTime=CTime::GetCurrentTime();//此时为断开时间
				sLog.Format("%s:%d",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
				pOShow->ShowData(sLog,"",1,cNode->ConnetTime,cNode->ReceiveTime,"对方连接断开");
				
				//io节点回收,c节点释放,socket关闭
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
				//接收的数据写日志,处理
				if(pOConfig->m_bLog)
				{
					//fym Recv Data
					//fym sLogData.Format("●←(%s:%d) %d %s", inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port),dwNumberBytes, IntToHexStr(ioNode->wsaRBuffer.buf,dwNumberBytes));
					sLogData.Format("●← <%s> %d %s", CParseTask::parse_id(ioNode->wsaRBuffer.buf).c_str(), dwNumberBytes, IntToHexStr(ioNode->wsaRBuffer.buf, dwNumberBytes));
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
				cNode->ReceiveTime=CTime::GetCurrentTime();//记录最后一次接收数据的时间
				sLog.Format("%s:%d",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
				pOShow->ShowData(sLog,"",0,cNode->ConnetTime,cNode->ReceiveTime,sLogData);
			
				cNode->pDisposeDtr->ReceiveData(ioNode->wsaRBuffer.buf,dwNumberBytes);//数据接收
				cNode->pDisposeDtr->DataDispose();//数据处理
				//投递读操作
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
				
				//判断投递结果
				if(SOCKET_ERROR  == iResult && ERROR_IO_PENDING  != WSAGetLastError() )
				{
					//错误信息永远写日志
					sLog.Format("投递WSARecv请求失败!错误原因:%d",WSAGetLastError());
					m_pOLog->WriteLog(sLog.GetBuffer(0));
					sLog.Format("%s:%d",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
					pOShow->ShowData(sLog,"",1,cNode->ConnetTime,cNode->ReceiveTime,"投递读操作失败,关闭socket,等待重连...");
					//从客户端链表移除
					RemoveFromListClient(cNode);
					//io节点回收,c节点释放,socket关闭
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
			//发送数据返回,回收ioNode
			if (pOConfig->m_bLog)
			{
				cNode->ReceiveTime=CTime::GetCurrentTime();
				sLogData.Format("●→(%s:%d)  %s",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port),IntToHexStr(ioNode->wsaSBuffer.buf,ioNode->wsaSBuffer.len));
				m_pOLog->WriteLog(sLogData.GetBuffer(0));
				sLog.Format("%s:%d",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
				pOShow->ShowData(sLog,"",0,cNode->ConnetTime,cNode->ReceiveTime,sLogData);
			}
			InsertToListBase(ioNode);
			
			break;
		default://按发送处理,但要几日志
			//记录日志,回收ioNode
			sLog.Format("客户端(%s:%d)出现非法IO操作类型",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
			m_pOLog->WriteLog(sLog.GetBuffer(0));
			
			InsertToListBase(ioNode);
			break;
		}//switch end
	}//while end
}
////////////////////////////////////////////////////////////////////


//侦听线程类函数的实现,同时多了个参数的传递,呵呵
DWORD __stdcall CIOCPServer::ListionThread(void* p_LT)//侦听线程壳
{
	PLISTION_THREAD p=(PLISTION_THREAD)p_LT;
	p->p_OIocp->TrueListionThread(p->AccN);
	return 0;
}

void CIOCPServer::TrueListionThread(int AccN)//实际侦听线程函数
{
	while(true)
	{		
		if (WaitForSingleObject(m_hEvent_Exit_Listion,0)==WAIT_OBJECT_0)
		{
			CloseHandle(m_hEvent_Accept);
			CloseHandle(m_hEvent_Exit_Listion);
			break; //退出线程
		}
		if(WAIT_OBJECT_0==WaitForSingleObject(m_hEvent_Accept, 0))
		{
			PostAcceptEx(AccN);
		}	
	}
}

//初始化函数
BOOL CIOCPServer::initIocp(CString IP,int Port,int ThreadCount,int AccN)
{
	CString sLog;
	//加载套接字库
	WSADATA wsd;
	DWORD nResult = WSAStartup(MAKEWORD(2,2), &wsd);
	if (0 != nResult)
	{
		m_pOLog->WriteLog("加载套接字库2.2失败");
		return FALSE;
	}
	//创建侦听端口
	m_hListenSocket=WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, 
          NULL, 0, WSA_FLAG_OVERLAPPED); 
    if (INVALID_SOCKET == m_hListenSocket)
    {
		m_pOLog->WriteLog("创建服务器侦听socket失败");
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
		m_pOLog->WriteLog("绑定socket地址失败");
        return FALSE;
    } 
	nResult = listen(m_hListenSocket, 20);
    if (SOCKET_ERROR == nResult)
    {
		shutdown(m_hListenSocket,2);
        closesocket(m_hListenSocket);
		WSACleanup();
        m_pOLog->WriteLog("侦听socket失败");
        return FALSE;
    }  

	//类中用到AcceptEx,TransmitFile,GetAcceptExSockaddrs函数,基于调用效率的问题,需获得这三个函数的扩展函数指针
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
		m_pOLog->WriteLog("获取AcceptEx函数扩展指针失败");
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
		m_pOLog->WriteLog("获取TransmitFile函数扩展指针失败");
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
		m_pOLog->WriteLog("获取GetAcceptExSockaddrs函数扩展指针失败");
		return FALSE;
	}
	//初始化侦听线程Accept事件对象
	m_hEvent_Accept = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(NULL==m_hEvent_Accept)
	{
		m_pOLog->WriteLog("创建事件m_hEvent_Accept失败");
		return FALSE;
	}
	nResult = WSAEventSelect(m_hListenSocket, m_hEvent_Accept, FD_ACCEPT);
	if (SOCKET_ERROR == nResult)
	{
		CloseHandle(m_hEvent_Accept);
		m_pOLog->WriteLog("关联事件m_hEvent_Accept失败");
		return FALSE;
	}
	int i;
	//申请io备用节点
	CIOCPServer::PPER_IO_CONTEXT ioNode;
	for(i=0;i<INBASE_SIZE;i++)
	{
		ioNode=(PPER_IO_CONTEXT)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PER_IO_CONTEXT));
		if(NULL==ioNode)
		{
			m_pOLog->WriteLog("函数initIocp()中申请ioNode内存失败");
			return FALSE;
		}
		
		ZeroMemory(ioNode->RBuffer, BUFFER_SMALL_SIZE);
		ZeroMemory(ioNode->SBuffer, BUFFER_SMALL_SIZE);
		ZeroMemory(&(ioNode->ol), sizeof(ioNode->ol));
		ioNode->wsaRBuffer.buf=ioNode->RBuffer;
		ioNode->wsaRBuffer.len=BUFFER_SMALL_SIZE;
		ioNode->wsaSBuffer.buf=ioNode->SBuffer;
		ioNode->wsaSBuffer.len=BUFFER_SMALL_SIZE;
		//加入备用链表
		ioNode->pNext=m_pListBaseIO;
		m_pListBaseIO=ioNode;
	}
	//创建完成端口
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (NULL == m_hIOCP)
	{
		m_pOLog->WriteLog("创建完成端口失败");
		return FALSE;
	}
	//将socket与完成端口关联,此操作必须,否则工作者线程中收不到Accept的完成信号
	m_cNode_AcceptEx=(PPER_HANDLE_CONTEXT)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PER_HANDLE_CONTEXT));
	if(NULL==m_cNode_AcceptEx)
	{
		m_pOLog->WriteLog("m_cNode_AcceptEx创建失败");
		return FALSE;
	}
	//关联
	HANDLE hResult = CreateIoCompletionPort(
       (HANDLE)m_hListenSocket,
       m_hIOCP,
       (DWORD)m_cNode_AcceptEx,//此节点无用,AcceptEX后,对应节点信息在ioNode里
       0);
	if (NULL == hResult)
	{
		m_pOLog->WriteLog("完成端口与侦听Socket关联失败");
		return FALSE;
	}
	
	//先发出若干AcceptEx调用
	if (FALSE==PostAcceptEx(AccN))
	{
		m_pOLog->WriteLog("initIocp函数调用PostAcceptEx函数失败");
		return FALSE;
	};
	//建立侦听线程退出事件
	m_hEvent_Exit_Listion=CreateEvent(NULL,TRUE,FALSE,NULL);
	//再启动侦听线程,这里AccN参数的传递有个小技巧,类似IO键,呵呵
	PLISTION_THREAD p=(PLISTION_THREAD)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LISTION_THREAD));
	if(NULL==p)
	{
		m_pOLog->WriteLog("函数initIocp()中申请p内存失败");
		return FALSE;
	}
	p->p_OIocp=this;
	p->AccN=AccN;
	HANDLE hThr=CreateThread(NULL,0,CIOCPServer::ListionThread,(void*)p,0,NULL);
	if (NULL == hThr)
	{
		CloseHandle(hThr);
		m_pOLog->WriteLog("创建侦听线程失败");
		return FALSE;
	}
	m_pOLog->WriteLog("创建侦听线程");
	CloseHandle(hThr);
	//创建工作者线程
	for (i=0; i<ThreadCount; i++)
	{
		HANDLE hThr= CreateThread(NULL, 0, CIOCPServer::WorkThread, (void*)this, 0, NULL);
		if (NULL == hThr)
		{
			CloseHandle(hThr);
			m_pOLog->WriteLog("创建工作者线程失败");
			CloseHandle(m_hIOCP);
			return FALSE;
		}
		sLog="";
		sLog.Format("创建工作者线程%d",i);
		m_pOLog->WriteLog(sLog);
		CloseHandle(hThr);
		
	}

	//返回
	return TRUE;
}
BOOL CIOCPServer::PostAcceptEx(int AccN)//连续发出若干AcceptEx调用
{

	for(int i=0;i<AccN;i++)
	{
		//申请一个cNode
		CIOCPServer::PPER_HANDLE_CONTEXT cNode=GetCNode();
		if(NULL==cNode)
		{
			m_pOLog->WriteLog("PostAcceptEx函数中申请cNode失败");
			continue;
		}
		//将socket与完成端口关联
		HANDLE hResult = CreateIoCompletionPort(
           (HANDLE)cNode->LinkSocket,
           m_hIOCP,
           (DWORD)cNode,
           0);
		if (NULL == hResult)
		{
			int nResult = WSAGetLastError();
			if(nResult!=ERROR_INVALID_PARAMETER)//已经与完成端口绑定过的socket会返回此错误
			{
				m_pOLog->WriteLog("PostAcceptEx函数中绑定完成端口失败");
				
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
		//申请一个ioNode
		CIOCPServer::PPER_IO_CONTEXT ioNode=GetIONode();
		if(NULL==ioNode)
		{
			m_pOLog->WriteLog("PostAcceptEx函数中申请ioNode失败");
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
		//将cNode加入ioNode
		ioNode->pCNode=cNode;//必须有赋值,因为ioNode中没有真正申请客户端节点内存
		DWORD dwBytes;
		//发出一个AcceptEx
		BOOL bSuccess = lpAcceptEx(
			m_hListenSocket,
			ioNode->pCNode->LinkSocket,
			ioNode->RBuffer,
			0,//接受连接即返回//BUFFER_SMALL_SIZE - (sizeof(SOCKADDR_IN) + 16) * 2,
			sizeof(SOCKADDR_IN) + 16,
			sizeof(SOCKADDR_IN) + 16,
			&dwBytes,
			&(ioNode->ol));//传递io节点,其中包括对应cNode的指针

		if (FALSE == bSuccess)
		{
			int nResult = WSAGetLastError();
			if (nResult != ERROR_IO_PENDING)//重叠io操作进行中
			{
				m_pOLog->WriteLog("PostAcceptEx函数中AcceptEx失败");
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
		//加入客户端链表
		InsertToListClient(cNode);
	}//for end
	
	return TRUE;
}
CIOCPServer::PPER_HANDLE_CONTEXT CIOCPServer::GetCNode()//从备用列表取得一个Client节点
{
	PPER_HANDLE_CONTEXT cNode;
	cNode=(CIOCPServer::PPER_HANDLE_CONTEXT)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CIOCPServer::PER_HANDLE_CONTEXT));
	if(NULL==cNode)
	{
		m_pOLog->WriteLog("函数GetcNode()中申请cNode内存失败");
		return NULL;
	}
	SOCKET CSocket= WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED); 
	if (INVALID_SOCKET == CSocket)
	{
		m_pOLog->WriteLog("函数GetcNode()中创建Socket失败");
		return NULL;
	}
	cNode->LinkSocket=CSocket;

	
	//初始化节点
	cNode->pFront=NULL;
	cNode->pNext=NULL;
	cNode->ConnetTime=0;
	cNode->ReceiveTime=0;
	cNode->pDisposeDtr=NULL;	
	ZeroMemory(&(cNode->FarSokAddr), sizeof(cNode->FarSokAddr));
	return cNode;
}
CIOCPServer::PPER_IO_CONTEXT CIOCPServer::GetIONode()//从备用列表取得一个io节点
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
			OLog->WriteLog("函数GetIONode()中申请ioNode内存失败");
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
void CIOCPServer::InsertToListBase(CIOCPServer::_PER_IO_CONTEXT* pIoNode)//将节点插入备用链表中
{
	if(pIoNode!=NULL)
	{
		EnterCriticalSection(&m_CriSnIOB);
		pIoNode->pNext=m_pListBaseIO;
		m_pListBaseIO=pIoNode;
		LeaveCriticalSection(&m_CriSnIOB);
	}
}
void CIOCPServer::InsertToListClient(CIOCPServer::_PER_HANDLE_CONTEXT* pHandleNode)//将节点插入客户端链表中,需同时指定前置节点
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
void CIOCPServer::RemoveFromListClient(CIOCPServer::_PER_HANDLE_CONTEXT* pHandleNode)//将节点移除客户端链表中,需同时指定前置节点
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
void CIOCPServer::SendData(SOCKADDR_IN FarSokAddr,const void* pV,int nL)//发送数据
{
	CString sLog;//写日志用
	CShowData* pOShow=CShowData::Instance();//界面显示

	PPER_HANDLE_CONTEXT cNode=NULL;
	cNode=m_pListClient;
	while (cNode!=NULL)
	{
		if(inet_ntoa(cNode->FarSokAddr.sin_addr)==inet_ntoa(FarSokAddr.sin_addr) && ntohs(cNode->FarSokAddr.sin_port)==ntohs(FarSokAddr.sin_port) )
		{
			PPER_IO_CONTEXT ioNode=GetIONode();
			if (NULL==ioNode)
			{
				sLog.Format("向客户端(%s:%d)发送数据失败,原因:申请ioNode失败",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
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
				sLog.Format("向客户端(%s:%d)发送数据失败,错误编号:%l",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port),WSAGetLastError());
				m_pOLog->WriteLog(sLog.GetBuffer(0));
				//发送失败,socket应该有问题
				//io节点回收,c节点释放,socket关闭
				sLog.Format("%s:%d",inet_ntoa(cNode->FarSokAddr.sin_addr),ntohs(cNode->FarSokAddr.sin_port));
				pOShow->ShowData(sLog,"",1,cNode->ConnetTime,cNode->ReceiveTime,"发送数据失败,关闭socket,等待重连...");
				
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