
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
		static Cleaner cl;//����ʹ˱�����Ϊ�˵����������Զ��ͷ�
	}
	return _instance;
}

CWriteLog::CWriteLog()
{
#if 0
	m_pLogQueue=(PLOG_DATA_QUEUE)HeapAlloc(
		GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LOG_DATA_QUEUE));//�洢��־��Ϣ�ö���
	//����־�ļ�
	BOOL b=CreateDirectory("��־�ļ�",NULL);
	CTime time=CTime::GetCurrentTime(); 
	m_nToday = time.GetDay();
	CString strFileName = time.Format("��־�ļ�\\%Y-%m-%d.txt");//2006-04-10
	m_hLogFile =CreateFile(strFileName,GENERIC_WRITE,FILE_SHARE_READ ,NULL,OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,NULL);
	if( INVALID_HANDLE_VALUE == m_hLogFile )
	{   
		AfxMessageBox("����־�ļ�ʧ�ܣ����鲢������������");
		
	}
	//��ʼ���¼�����
	m_hEvent_Exit_Log=CreateEvent(NULL,TRUE,FALSE,NULL);
	//��ʼ���ٽ���
	InitializeCriticalSection(&m_csLog_buffer);
	//����д��־�߳�
	HANDLE hThr= CreateThread(NULL, 0, CWriteLog::WriteLogThread, (void*)this, 0, NULL);
	if (NULL==hThr)
	{
		AfxMessageBox("����д��־�߳�ʧ�ܣ����鲢������������");
		
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
//д��־����
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
//�߳̿Ǻ���ʵ��
DWORD __stdcall CWriteLog::WriteLogThread(void* p_OWriteLog)//�������߳̿�,Ϊ���ܽ��ຯ����Ϊ�̺߳�������
{
#if 0
	CWriteLog* p=(CWriteLog*)p_OWriteLog;
	p->TrueWriteLogThread();
#endif
	return 0;
}
//д��־�߳�
void CWriteLog::TrueWriteLogThread()
{
#if 0
	while(true)
	{
		if (WAIT_OBJECT_0==WaitForSingleObject(m_hEvent_Exit_Log,1))//׼���˳��߳�,�����ֳ�
		{
			CloseHandle(m_hLogFile);
			CloseHandle(m_hEvent_Exit_Log);
			break;
		}
		//�µ�һ��,�½���־�ļ�
		CTime T=CTime::GetCurrentTime();
		if (m_nToday != T.GetDay() || NULL==m_hLogFile)
		{
			try
			{
				CloseHandle(m_hLogFile);
				m_nToday = T.GetDay();
				CString strFileName = T.Format("��־�ļ�\\%Y-%m-%d.txt");
				m_hLogFile =CreateFile(strFileName,GENERIC_WRITE,FILE_SHARE_READ ,NULL,OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,NULL);
				
			}catch(...){}
		}
		//д��־
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