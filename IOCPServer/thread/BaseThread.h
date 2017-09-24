// **********************************************************************
// 作者: 傅佑铭
// 版本: 1.0
// 日期: 2011-01 ~ 2011-11
// 修改历史记录: 
// 日期, 作者, 变更内容
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
	// 线程基类
	class CBaseThread
	{
	public:
		static void initialize();

		CBaseThread();
		virtual ~CBaseThread();

		SS_Error start(void* parent);
		SS_Error shutdown();

		//返回线程类型
		virtual THREAD_TYPE get_type() = 0;

	protected:
		void join();

		virtual void on_start() = 0;

		virtual void on_close() = 0;

		//线程入口函数
		static unsigned int WINAPI _entry(LPVOID inThread);
		//线程实际入口函数，重载用
		virtual void entry() = 0;

	protected:
		//static DWORD _thread_storage_index;//线程局部存储索引

		void* _parent_ptr;//线程启动者指针

		bool _joined;//线程是否即将关闭

		HANDLE _thread_handle;

		//void* _thread_data_ptr;

		//static void* _main_thread_data_ptr;

		CSSMutex _mutex; //互斥操作信号量

	};
}

#endif  // _BASE_THREAD_H_      
