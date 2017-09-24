// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2011-01 ~ 2011-11
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#ifndef _TASK_THREAD_H_      
#define _TASK_THREAD_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "GeneralDef.h"
#include "BaseThread.h"
#include "Task.h"

namespace ScheduleServer
{
	//�������߳�
	class CTaskThread : public CBaseThread
	{
	public:
		CTaskThread() : _parallel_task_num(10) {};//��ಢ��ִ��10������
		virtual ~CTaskThread() {};
		
		SS_Error add_task(CTask* task);

		SS_Error remove_all_tasks();

		unsigned long get_task_num();

		static void sleep_ms(unsigned long interval);

		//�����߳�����
		virtual THREAD_TYPE get_type()
		{
			return TASK_THREAD;
		}

	protected:
		// �߳���ں���
		virtual void entry();

		virtual void on_start();

		virtual void on_close();

	protected:
		std::list<CTask*> _task_queue;

		const unsigned long _parallel_task_num;//����ִ������������

	public:
		//CRTPSendSession _rtp_send_session;//RTP����session
	};

	class CTaskThreadPool
	{
	public:
		static SS_Error add_threads(unsigned long task_thread_num, void* parent);

		static void remove_threads();

		static CTaskThread* select_thread(unsigned long index = 0);

		//��ȡ�����߳���
		static unsigned long get_task_thread_num() { return _task_thread_num; };

		//���ӳ�פ�������
		static void increase_resident_task_count(unsigned long num) { _resident_task_cout += num; };

		//���ٳ�פ�������
		static void decrease_resident_task_count(unsigned long num) { _resident_task_cout = (num > _resident_task_cout) ? 0 : (_resident_task_cout - num); };

		//��ȡ��פ�������
		static unsigned long get_resident_task_count() { return _resident_task_cout; };

	private:

		static CTaskThread** _task_thread_array;//�����߳�ָ�뼯��

		static unsigned long _task_thread_num;//�����߳�����

		static bool _is_firstly_select;//�Ƿ��״ε���select_thread

		static HANDLE _create_task_thread_handle;//�����߳��Ƿ񴴽���־���

		static unsigned long _resident_task_cout;//��פ�������
	};
}

#endif  // _TASK_THREAD_H_   
