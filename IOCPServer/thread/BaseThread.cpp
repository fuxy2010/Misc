// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2011-01 ~ 2011-11
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#include "GeneralDef.h"
#include "BaseThread.h"

//for statistics
#include "Logger.h"
#include "MiscTool.h"
#include "ScheduleServer.h"

using namespace ScheduleServer;

//void*   CBaseThread::_main_thread_data_ptr = NULL;

//DWORD   CBaseThread::_thread_storage_index = 0;

CBaseThread::CBaseThread() :
_thread_handle(NULL),
_parent_ptr(NULL),
_joined(false)
//_thread_data_ptr(NULL)
{
}

CBaseThread::~CBaseThread()
{
}

void CBaseThread::initialize()
{
	//_thread_storage_index = ::TlsAlloc();
}

SS_Error CBaseThread::start(void* parent)
{
	unsigned int id(0);

	on_start();//on_startҪ���ʼ����

	_thread_handle = (HANDLE)_beginthreadex( NULL,   // Inherit security
										0,      // Inherit stack size
										_entry, // Entry function
										(void*)this,    // Entry arg
										0,      // Begin executing immediately
										&id );

	SuspendThread(_thread_handle);//��ͣ(����)�߳� 
	SetThreadPriority(_thread_handle, THREAD_PRIORITY_HIGHEST);//�����߳����ȼ�
	ResumeThread(_thread_handle);//�ָ��߳�����

	_parent_ptr = parent;

	_joined = false;

	return SS_NoErr;
}

SS_Error CBaseThread::shutdown()
{
	_joined = true;

	join();

	on_close();//on_startҪ�����յ���

	return SS_NoErr;
}

void CBaseThread::join()
{
    DWORD theErr = ::WaitForSingleObject(_thread_handle, INFINITE);
}

unsigned int WINAPI CBaseThread::_entry(LPVOID inThread)
{
    CBaseThread* theThread = (CBaseThread*)inThread;

	if(NULL == theThread)
		return -1;

	/*if(FALSE == ::TlsSetValue(_thread_storage_index, theThread))
		return -1;*/

	while(false == theThread->_joined)
	{
		theThread->entry();
	}

    return 0;
}

/*CBaseThread* CBaseThread::get_current()
{
	return (CBaseThread *)::TlsGetValue(_thread_storage_index);
}*/
