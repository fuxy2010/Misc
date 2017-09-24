// **********************************************************************
// ����: FYM
// �汾: 1.0
// ����: 2010-01 ~ 2010-03
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#ifndef _MAINTANCE_THREAD_H_       
#define _MAINTANCE_THREAD_H_ 

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "GeneralDef.h"
#include "BaseThread.h"
#include "Task.h"

namespace ScheduleServer
{
	class CMaintanceThread : public CBaseThread
	{
	public:
		CMaintanceThread() {};

		virtual ~CMaintanceThread() {};

		//�����߳�����
		virtual THREAD_TYPE get_type()
		{
			return MAINTANCE_THREAD;
		}

	protected:
		// �߳���ں���
		virtual void entry();

		virtual void on_start();

		virtual void on_close();

	private:
		unsigned long _ua_statistics_timestamp;//UAͳ�Ƶ�ʱ��

		unsigned long _performance_statistics_timestamp;//��������Դռ��ͳ��ʱ��

		unsigned long _check_sip_server_timestamp;//���SIP������ʱ��

	public:
		//static string _imsi;
		//static string _latitude;
		//static string _longitude;
		static unsigned short _listen_port;
		//static unsigned long  _restart_interval;

		int _times;
		string _sql;
		SS_CTX _sql_ctx;
	};
}

#endif  // _MAINTANCE_THREAD_H_