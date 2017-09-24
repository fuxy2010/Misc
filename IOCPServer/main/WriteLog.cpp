
#include "stdafx.h"
#include "WriteLog.h"
#include "ScheduleServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ScheduleServer;

CWriteLog* CWriteLog::_instance=0;

CWriteLog* CWriteLog::Instance()
{
	if (0==_instance)
	{
		_instance=new CWriteLog();
		static Cleaner cl;//此类和此变量均为了单件对象能自动释放
	}
	return _instance;
}

CWriteLog::CWriteLog()
{
#if 0
	m_pLogQueue=(PLOG_DATA_QUEUE)HeapAlloc(
		GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LOG_DATA_QUEUE));//存储日志信息用队列
	//打开日志文件
	BOOL b=CreateDirectory("日志文件",NULL);
	CTime time=CTime::GetCurrentTime(); 
	m_nToday = time.GetDay();
	CString strFileName = time.Format("日志文件\\%Y-%m-%d.txt");//2006-04-10
	m_hLogFile =CreateFile(strFileName,GENERIC_WRITE,FILE_SHARE_READ ,NULL,OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,NULL);
	if( INVALID_HANDLE_VALUE == m_hLogFile )
	{   
		AfxMessageBox("打开日志文件失败！请检查并重新启动程序。");
		
	}
	//初始化事件对象
	m_hEvent_Exit_Log=CreateEvent(NULL,TRUE,FALSE,NULL);
	//初始化临界区
	InitializeCriticalSection(&m_csLog_buffer);
	//创建写日志线程
	HANDLE hThr= CreateThread(NULL, 0, CWriteLog::WriteLogThread, (void*)this, 0, NULL);
	if (NULL==hThr)
	{
		AfxMessageBox("创建写日志线程失败！请检查并重新启动程序。");
		
	}
	CloseHandle(hThr);
#endif
}
CWriteLog::~CWriteLog()
{
#if 0
	SetEvent(m_hEvent_Exit_Log);
	HeapFree(GetProcessHeap(), 0, m_pLogQueue);
	DeleteCriticalSection(&m_csLog_buffer);
#endif
}
//写日志函数
//fym void CWriteLog::WriteLog(const CString remark)
void CWriteLog::WriteLog(CString remark)
{
#if 0
	EnterCriticalSection(&m_csLog_buffer);	
 		_tcsncpy(m_pLogQueue->Log_Array[m_pLogQueue->Head],remark,_tcslen(remark));
 		m_pLogQueue->Head=(m_pLogQueue->Head+1) % BUFFER_BIG_SIZE;
	LeaveCriticalSection(&m_csLog_buffer);
#endif
	LOG_WRITE(string(remark.GetBuffer(remark.GetLength())), 1, false);
	remark.ReleaseBuffer();
}
//线程壳函数实现
DWORD __stdcall CWriteLog::WriteLogThread(void* p_OWriteLog)//工作者线程壳,为了能将类函数做为线程函数创建
{
#if 0
	CWriteLog* p=(CWriteLog*)p_OWriteLog;
	p->TrueWriteLogThread();
#endif
	return 0;
}
//写日志线程
void CWriteLog::TrueWriteLogThread()
{
#if 0
	while(true)
	{
		if (WAIT_OBJECT_0==WaitForSingleObject(m_hEvent_Exit_Log,1))//准备退出线程,清理现场
		{
			CloseHandle(m_hLogFile);
			CloseHandle(m_hEvent_Exit_Log);
			break;
		}
		//新的一天,新建日志文件
		CTime T=CTime::GetCurrentTime();
		if (m_nToday != T.GetDay() || NULL==m_hLogFile)
		{
			try
			{
				CloseHandle(m_hLogFile);
				m_nToday = T.GetDay();
				CString strFileName = T.Format("日志文件\\%Y-%m-%d.txt");
				m_hLogFile =CreateFile(strFileName,GENERIC_WRITE,FILE_SHARE_READ ,NULL,OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,NULL);
				
			}catch(...){}
		}
		//写日志
		if (m_pLogQueue->Head!=m_pLogQueue->Tail )
		{
			CString remark=m_pLogQueue->Log_Array[m_pLogQueue->Tail];
			try
			{
				DWORD dwWrite;
				SetFilePointer (m_hLogFile, NULL, NULL, FILE_END);
				m_timeCurrent=CTime::GetCurrentTime(); 
				if(remark!="")
				{
				m_strCommandLine = m_timeCurrent.Format("%H:%M:%S ");// 14:59:23
				WriteFile(m_hLogFile,m_strCommandLine,m_strCommandLine.GetLength(),&dwWrite,NULL); 
				}
				WriteFile(m_hLogFile,remark,strlen(remark),&dwWrite,NULL);
				WriteFile(m_hLogFile,"\r\n",strlen("\r\n"),&dwWrite,NULL);
				m_pLogQueue->Tail=(m_pLogQueue->Tail+1) % BUFFER_BIG_SIZE;
			}catch(...) {  }	
		}
	}//while end
#endif
}