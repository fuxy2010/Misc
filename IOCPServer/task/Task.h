// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2011-01 ~ 2011-11
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#ifndef _TASK_H_
#define _TASK_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "GeneralDef.h"

namespace ScheduleServer
{
	//�������
	class CTask
	{

	public:
		CTask() : _is_done(false), _owner(NULL) {};
		virtual ~CTask() {};

		virtual SS_Error run() = 0;
		bool is_done() { return _is_done; };

		void set_owner(void* owner) { _owner = owner; };

	protected:
		virtual SS_Error on_done() = 0;

		virtual SS_Error on_exception() = 0;

	protected:
		bool _is_done;
		
		void* _owner;

	};
}

#endif  // _TASK_H_      
