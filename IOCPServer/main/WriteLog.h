#if !defined(WriteLog_2010_06_03_09_51_21_mxj)
#define WriteLog_2010_06_03_09_51_21_mxj

#if _MSC_VER >1000
#pragma once
#endif

#define    BUFFER_LITTLE_SIZE        512//缓存512
#define    BUFFER_SMALL_SIZE         1024//缓存1k
#define    BUFFER_BIG_SIZE           4096//缓存4k

class CWriteLog//SingLeton模式全局只有一个对象
{
private:
	static CWriteLog* _instance;//唯一对象

	HANDLE    m_hLogFile;//日志文件句柄
	HANDLE    m_hEvent_Exit_Log;       //通知写日志线程退出的事件对象 
	CTime     m_timeCurrent;//写日志文件用，避免局部变量频繁的构造、析构
	CString   m_strCommandLine;//写日志文件用，避免局部变量频繁的构造、析构
	int m_nToday;
	CRITICAL_SECTION m_csLog_buffer;//各线程保存日志内容到日志队列用临界区

	//存储待写日志的队列
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
	//写日志函数
	//写日志线程函数
	static DWORD __stdcall WriteLogThread(void* p_OWriteLog);//线程壳,为了能将类函数做为线程函数创建
	void TrueWriteLogThread();//线程函数
	//解决单件模式对象自动释放的问题
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
	//写日志函数
	void WriteLog(CString remark);//fym void WriteLog(const CString remark);
};



#endif
