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

#define    BUFFER_LITTLE_SIZE        512//缓存512
#define    BUFFER_SMALL_SIZE         1024//缓存1k
#define    BUFFER_BIG_SIZE           4096//缓存4k
#define    CNBASE_SIZE               200//客户端备用节点数量
#define    INBASE_SIZE				 500//IO备用节点数量

class CIOCPServer//完成端口服务器类
{
private:
//自用类型声明

	//io类型
	typedef enum _IO_OPERATION 
	{
	  IoAccept, //AcceptEx/accept
	  IoRead,   //WSARecv/recv/ReadFile
	  IoWrite   //WSASend/send/WriteFile
	}IO_OPERATION, *PIO_OPERATION;

	//“完成键”(单句柄数据),保存socket

	typedef struct _PER_HANDLE_CONTEXT
	{
		SOCKET                    LinkSocket;//socket标识,注意是本地连接远端客户端的套接字标识
		SOCKADDR_IN               FarSokAddr;//远端客户端soket地址(包括端口)
		CTime					  ConnetTime;//连接时间
		CTime					  ReceiveTime;//上次接收数据时间
		CClient*				  pDisposeDtr;//客户端类对象指针

		_PER_HANDLE_CONTEXT*      pNext;//后节点
		_PER_HANDLE_CONTEXT*      pFront;//前节点

	}PER_HANDLE_CONTEXT, *PPER_HANDLE_CONTEXT;

	//单IO数据，扩展的WSAOVERLAPPED
	typedef struct _PER_IO_CONTEXT
	{
		WSAOVERLAPPED              ol;
		WSABUF                     wsaRBuffer;//接收数据用缓存
		WSABUF					   wsaSBuffer;//发送用数据缓存
		char                       RBuffer[BUFFER_SMALL_SIZE];
		char                       SBuffer[BUFFER_SMALL_SIZE];
		IO_OPERATION               IoOperation;
		
		PPER_HANDLE_CONTEXT        pCNode;//指向客户端节点的指针,没办法,AcceptEX只能传递一个io节点
		_PER_IO_CONTEXT*           pNext; 
	}PER_IO_CONTEXT, *PPER_IO_CONTEXT;
	//只为能给TrueListionThread函数传入AccN参数
	typedef struct _LISTION_THREAD  
	{
		CIOCPServer* p_OIocp;
		int AccN;	
	}LISTION_THREAD,*PLISTION_THREAD;
//成员变量
	PPER_HANDLE_CONTEXT  m_pListClient;//所有连接客户端双向节点链表
	PPER_IO_CONTEXT      m_pListBaseIO;//备用io节点链表
	
	CRITICAL_SECTION              m_CriSnC;//插入客户端链表节点的关键代码段
	CRITICAL_SECTION              m_CriSnCB;//插入备用客户端链表节点的关键代码段
	CRITICAL_SECTION              m_CriSnIOB;//插入备用io节点链表的关键代码段

	HANDLE                        m_hIOCP;//完成端口句柄
	SOCKET                        m_hListenSocket;//侦听socket

	HANDLE						  m_hEvent_Accept;//通知Accept调用耗尽的事件对象
	HANDLE						  m_hEvent_Exit_Listion;//通知侦听线程退出的事件对象
	//下面这3个成员均属于函数指针,所以就不带m_开头了
	LPFN_TRANSMITFILE             lpTransmitFile;//TransmitFile函数扩展指针
	LPFN_ACCEPTEX                 lpAcceptEx;//AcceptEx函数扩展指针
	LPFN_GETACCEPTEXSOCKADDRS	  lpfnGetAcceptExSockaddrs;//GetAcceptExSockaddres函数扩展指针  

	PPER_HANDLE_CONTEXT			  m_cNode_AcceptEx;//响应连接用的一个cNode类型指针,没用,最后释放就行了

	CWriteLog*					  m_pOLog;
//成员函数
	CString IntToHexStr(char* phead, int length);
	BOOL PostAcceptEx(int AccN);//连续发出若干AcceptEx调用。参数为调用的数量
	
	PPER_HANDLE_CONTEXT GetCNode();//从备用列表取得一个Client节点
	PPER_IO_CONTEXT     GetIONode();//从备用列表取得一个io节点

	void InsertToListClient(PPER_HANDLE_CONTEXT pHandleNode);//将节点插入客户端链表中
	void RemoveFromListClient(PPER_HANDLE_CONTEXT pHandleNode);//将节点从客户端链表中移除
	void InsertToListBase(PPER_IO_CONTEXT pIoNode);//回收节点
	
	
	static DWORD __stdcall WorkThread(void* p_OIocp);//工作者线程壳,为了能将类函数做为线程函数创建
	void TrueWorkThread();//工作者线程函数
	static DWORD __stdcall ListionThread(void* p_LT);//侦听线程壳
	void TrueListionThread(int AccN);//侦听线程
public:
	CIOCPServer();
	~CIOCPServer();
	BOOL initIocp(CString IP,int Port,int ThreadCount,int AccN);//初始化,创建完成端口,工作者线程,侦听线程等,需传入主机ip和侦听端口,工作者线程数量和一次AcceptEx的数量
	//同时初始化备用节点链表,我的想法是先申请若干节点备用,可申请使用.删除的节点同时保存起来备用
	void SendData(SOCKADDR_IN SokAddr,const void* pV,int nL);//发送数据

};

#endif