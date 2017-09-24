// **********************************************************************
// 作者: 傅佑铭
// 版本: 1.0
// 日期: 2011-01 ~ 2011-11
// 修改历史记录: 
// 日期, 作者, 变更内容
// **********************************************************************
#ifndef _PARSE_TASK_H_
#define _PARSE_TASK_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "GeneralDef.h"
#include "Task.h"
#include "time.h"
#include "ScheduleServer.h"

namespace ScheduleServer
{
	typedef enum
	{
		ParseTask_Begin = 0,//任务开始
		ParseTask_Process,//发送
		ParseTask_Done//任务完成
	}
	PARSE_TASK_STATUS;

	typedef struct tagPARSE_TASK_INFO
	{
		unsigned long	task_id;//任务ID
		char*			msg;
		int				length;

		tagPARSE_TASK_INFO() : task_id(0), msg(NULL), length(0)
		{
		}

		~tagPARSE_TASK_INFO()
		{
			//绝对不可在此释放video_packet_ptr分配的内存!!!
		}

		void clear()
		{
			delete msg;
			msg = NULL;
		}
	}
	PARSE_TASK_INFO;

	class CParseTask : public CTask
	{
	public:
		CParseTask(PARSE_TASK_INFO& task_info);
		virtual ~CParseTask();

		virtual SS_Error run();

	protected:
		virtual SS_Error on_done();

		virtual SS_Error on_exception();

	private:
		bool parse();
		bool wgs2mars();
		bool process();

	public:
		static string parse_id(const char* content);

	protected:
		/*volatile */PARSE_TASK_STATUS _status;

		PARSE_TASK_INFO _task_info;

		string _latitude_str;
		string _longitude_str;

		double _latitude_wgs;
		double _longitude_wgs;

		double _latitude_mars;
		double _longitude_mars;

		string _dev_id;

	};
}

#endif  //_PARSE_TASK_H_      
