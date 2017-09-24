// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2011-01 ~ 2011-11
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#ifndef _BASE_THREAD_H_     
#define _BASE_THREAD_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "GeneralDef.h"
#include "Locker.h"

namespace ScheduleServer
{
	// �̻߳���
	class CBaseThread
	{
	public:
		static void initialize();

		CBaseThread();
		virtual ~CBaseThread();

		SS_Error start(void* parent);
		SS_Error shutdown();

		//�����߳�����
		virtual THREAD_TYPE get_type() = 0;

	protected:
		void join();

		virtual void on_start() = 0;

		virtual void on_close() = 0;

		//�߳���ں���
		static unsigned int WINAPI _entry(LPVOID inThread);
		//�߳�ʵ����ں�����������
		virtual void entry() = 0;

	protected:
		//static DWORD _thread_storage_index;//�ֲ߳̾��洢����

		void* _parent_ptr;//�߳�������ָ��

		bool _joined;//�߳��Ƿ񼴽��ر�

		HANDLE _thread_handle;

		//void* _thread_data_ptr;

		//static void* _main_thread_data_ptr;

		CSSMutex _mutex; //��������ź���

	};
}

#endif  // _BASE_THREAD_H_      
