// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2011-01 ~ 2011-11
// �޸���ʷ��¼: 
// ����, ����, �������
// **********************************************************************
#include <iostream>
#include "GeneralDef.h"
#include "TaskThread.h"

using namespace ScheduleServer;

void CTaskThread::sleep_ms(unsigned long interval)
{
#if 1
	unsigned begin, end;

	begin = timeGetTime();

	do 
	{
		end = timeGetTime();
	}
	while (interval > end - begin);
#else
	LARGE_INTEGER litmp;
	LONGLONG quad_part1, quad_part2;
	double dif_minus, freq, dif_sec;

	QueryPerformanceFrequency(&litmp);
	freq = (double)litmp.QuadPart;// ��ü�������ʱ��Ƶ��

	QueryPerformanceCounter(&litmp);
	quad_part1 = litmp.QuadPart;// ��ó�ʼֵ

	do
	{
		QueryPerformanceCounter(&litmp);
		quad_part2 = litmp.QuadPart;//�����ֵֹ

		dif_minus = (double)(quad_part2 - quad_part1);
		dif_sec = dif_minus / freq;// ��ö�Ӧ��ʱ��ֵ����λΪ��
	}
	while((interval / 1000) > dif_sec);
#endif
}

SS_Error CTaskThread::add_task(CTask* task)
{
	if(NULL  == task || true == _joined)
		return SS_InvalidTask;

	CSSLocker lock(&_mutex);

	task->set_owner(this);

	_task_queue.push_back(task);

	return SS_NoErr;
}

SS_Error CTaskThread::remove_all_tasks()
{
	while(_task_queue.size())
	{
		CTask* task = NULL;
		{
			CSSLocker lock(&_mutex);

			task  = *(_task_queue.begin());
			_task_queue.pop_front();
		}

		delete task;
		task = NULL;		
	}

	_task_queue.clear();

	return SS_NoErr;
}

unsigned long CTaskThread::get_task_num()
{
	CSSLocker lock(&_mutex);
	
	return static_cast<unsigned long>(_task_queue.size());
}

void CTaskThread::entry()
{
	//statistic_enter('T');

	unsigned long start = timeGetTime();//�����߳�ѭ��ִ����ʼʱ��
	bool has_run = false;//�̱߳���ѭ���Ƿ�ִ�й�����

#ifdef _DEBUG
	//SINGLETON(CScheduleServer).singleton_test();
	//std::cout << "\nTask thread " << reinterpret_cast<unsigned long>(this) << " has " << _task_queue.size() << " tasks.";
	//if(_task_queue.size())
	//	std::cout << "\n<" << reinterpret_cast<unsigned long>(this) << ", " << _task_queue.size() << ">";
#endif

	//���β���ִ��������
	unsigned long parallel_task_num = 100;//_parallel_task_num + (CTaskThreadPool::get_resident_task_count() % ((!CTaskThreadPool::get_task_thread_num()) ? 1 : CTaskThreadPool::get_task_thread_num()));

	unsigned long i = 0;
	while(i < parallel_task_num)//�����߳�ÿ��ִ���������parallel_task_num������
	{
		if(true == _joined)
			break;

		CTask* task = NULL;

		//�ҵ������е�i����Ч����
		{
			//�����ڼ����ķ�Χ������run
			CSSLocker lock(&_mutex);

			//�������Ϊ��ʱ�̱߳���ִ�н���
			if(true == _task_queue.empty())
				break;

			//�ѱ���һ������ʱ�̱߳���ִ�н���
			if(i >= _task_queue.size())
				break;

			//����_task_queue�е�i+1������
			std::list<CTask*>::iterator iter = _task_queue.begin();

#if 0
			unsigned long j = 0;
			while(i > j++)
			{
				++iter;
			}
#else
			advance(iter, i);
#endif

			++i;//�ٶ�iterָ���task����

			if(NULL != *iter)//iterָ��ǿ�����ָ��
			{
				task = *iter;

				if(true == task->is_done())
				{
					_task_queue.erase(iter);
					--i;//�Ʒ�֮ǰ�ļٶ�
				}
			}
			else//iterָ�������ָ��
			{
				_task_queue.erase(iter);
				--i;//�Ʒ�֮ǰ�ļٶ�
			}
		}

		//ִ�ж����е�i������
		if(NULL != task)//ע��task�����λ�ü����ֵ
		{
			if(true == task->is_done())//һ��task��is_done()Ϊtrue�����ٴ�run,�����ظ�����on_done()
			{
				delete task;
				task = NULL;
			}
			else
			{
				task->run();
				has_run = true;
			}
		}
		
	}

	//statistic_leave();

	//����̱߳���ִ��ʱ����30ms����������30ms������CPUռ���ʹ���
	//ע�ⲻ���ڼ�������ִ��Sleep����Ӱ��Ч��
	if(timeGetTime() == start)
		Sleep(1);
}

void CTaskThread::on_start()
{
	_task_queue.clear();
}

void CTaskThread::on_close()
{
	std::cout << "\nremove all tasks." << endl;
	remove_all_tasks();
}

CTaskThread** CTaskThreadPool::_task_thread_array = NULL;
unsigned long CTaskThreadPool::_task_thread_num = 0;
HANDLE CTaskThreadPool::_create_task_thread_handle = ::CreateEvent(NULL, TRUE, FALSE, NULL);
bool CTaskThreadPool::_is_firstly_select = true;
unsigned long CTaskThreadPool::_resident_task_cout = 0;

SS_Error CTaskThreadPool::add_threads(unsigned long task_thread_num, void* parent)
{
	if(NULL != _task_thread_array)
		return SS_StartTaskThreadFail;

	_task_thread_array = new CTaskThread*[task_thread_num];

	if(NULL == _task_thread_array)
		return SS_StartTaskThreadFail;

	for(unsigned short x = 0; x < task_thread_num; ++x)
	{
		_task_thread_array[x] = new CTaskThread();
		if(SS_StartTaskThreadFail == _task_thread_array[x]->start(parent))
			return SS_StartTaskThreadFail;
	}

	_task_thread_num = task_thread_num;

	::SetEvent(_create_task_thread_handle);//֪ͨselect_thread��������

	return SS_NoErr;
}

void CTaskThreadPool::remove_threads()
{
	for(unsigned short x = 0; x < _task_thread_num; ++x)
	{
		_task_thread_array[x]->shutdown();
	}

	for(unsigned short y = 0; y < _task_thread_num; ++y)
	{
		delete _task_thread_array[y];
		_task_thread_array[y] = NULL;
	}

	delete[] _task_thread_array;
	_task_thread_array = NULL;

	_task_thread_num = 0;
}

CTaskThread* CTaskThreadPool::select_thread(unsigned long index)
{
	if(true == _is_firstly_select)//�״ε��òż��
	{
		::WaitForSingleObject(_create_task_thread_handle, INFINITE);//�ȴ������̴߳������
		::CloseHandle(_create_task_thread_handle);

		_is_firstly_select = false;
	}

	if(!_task_thread_num)
		return NULL;

	//if(index)//index != 0ʱ��רΪ���DBUpdateTask��,����index�������߳���ȡģ�Ľ��ѡ���߳�
	//{
	//	return _task_thread_array[index % _task_thread_num];
	//}

	//ѡ�����������ٵ��߳�
	//����get_task_num���������
	/*CTaskThread* selected_thread = _task_thread_array[0];
	unsigned long task_num = _task_thread_array[0]->get_task_num();

	for(unsigned short x = 1; x < _task_thread_num; ++x)
	{
		selected_thread = (task_num < _task_thread_array[x]->get_task_num()) ? selected_thread : _task_thread_array[x];
	}

	return selected_thread;*/

	return _task_thread_array[index % _task_thread_num];

}
