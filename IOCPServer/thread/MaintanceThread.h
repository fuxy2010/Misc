// **********************************************************************
// 作者: FYM
// 版本: 1.0
// 日期: 2010-01 ~ 2010-03
// 修改历史记录: 
// 日期, 作者, 变更内容
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

		//返回线程类型
		virtual THREAD_TYPE get_type()
		{
			return MAINTANCE_THREAD;
		}

	protected:
		// 线程入口函数
		virtual void entry();

		virtual void on_start();

		virtual void on_close();

	private:
		unsigned long _ua_statistics_timestamp;//UA统计的时戳

		unsigned long _performance_statistics_timestamp;//服务器资源占用统计时戳

		unsigned long _check_sip_server_timestamp;//检查SIP服务器时戳

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