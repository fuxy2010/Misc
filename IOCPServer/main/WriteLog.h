#if !defined(WriteLog_2010_06_03_09_51_21_mxj)
#define WriteLog_2010_06_03_09_51_21_mxj

#if _MSC_VER >1000
#pragma once
#endif

#define    BUFFER_LITTLE_SIZE        512//����512
#define    BUFFER_SMALL_SIZE         1024//����1k
#define    BUFFER_BIG_SIZE           4096//����4k

class CWriteLog//SingLetonģʽȫ��ֻ��һ������
{
private:
	static CWriteLog* _instance;//Ψһ����

	HANDLE    m_hLogFile;//��־�ļ����
	HANDLE    m_hEvent_Exit_Log;       //֪ͨд��־�߳��˳����¼����� 
	CTime     m_timeCurrent;//д��־�ļ��ã�����ֲ�����Ƶ���Ĺ��졢����
	CString   m_strCommandLine;//д��־�ļ��ã�����ֲ�����Ƶ���Ĺ��졢����
	int m_nToday;
	CRITICAL_SECTION m_csLog_buffer;//���̱߳�����־���ݵ���־�������ٽ���

	//�洢��д��־�Ķ���
	typedef struct _LOG_DATA_QUEUE  
	{
		TCHAR Log_Array[BUFFER_BIG_SIZE][BUFFER_SMALL_SIZE];
		DWORD Head;
		DWORD Tail;

		_LOG_DATA_QUEUE::_LOG_DATA_QUEUE()
		{
			Head=0;
			Tail=0;
		};	
	}LOG_DATA_QUEUE,*PLOG_DATA_QUEUE;
	PLOG_DATA_QUEUE m_pLogQueue;
	//д��־����
	//д��־�̺߳���
	static DWORD __stdcall WriteLogThread(void* p_OWriteLog);//�߳̿�,Ϊ���ܽ��ຯ����Ϊ�̺߳�������
	void TrueWriteLogThread();//�̺߳���
	//�������ģʽ�����Զ��ͷŵ�����
	class Cleaner 
	{  
	public:  
	  Cleaner(){}  
	  ~Cleaner()  
	  {  
	   
	   if(CWriteLog::Instance())  
	   delete CWriteLog::Instance();  
	  }  
	};  

protected:
	CWriteLog();
public:
	~CWriteLog();
	static CWriteLog* Instance();
	//д��־����
	void WriteLog(CString remark);//fym void WriteLog(const CString remark);
};



#endif
