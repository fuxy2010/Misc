// **********************************************************************
// 作者: 傅佑铭
// 版本: 1.0
// 日期: 2011-01 ~ 2011-11
// 修改历史记录: 
// 日期, 作者, 变更内容
// **********************************************************************
#ifndef _RPOCESS_INFO_H_       
#define _RPOCESS_INFO_H_

#include <TlHelp32.h>
#include <Windows.h>
#include <stdio.h>
#include <Psapi.h>

//微秒级计时精度
class CProcessInfo
{
public:
	CProcessInfo() :
	_schedule_server_cpu(0.0),
	_sip_server_cpu(0.0),
	_schedule_server_memory(0),
	_sip_server_memory(0)
	{
	}

	virtual ~CProcessInfo()
	{
	}

public:
	unsigned long get_process_id(string process_name)
	{
		unsigned long process_id = 0;

		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if(INVALID_HANDLE_VALUE == hSnapshot)
		{
			return 0;
		}

		PROCESSENTRY32 pe = { sizeof(pe) };
		BOOL fOk;

		for(fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
		{
			if (!_tcscmp(pe.szExeFile, process_name.c_str()))
			{
				CloseHandle(hSnapshot);
				return pe.th32ProcessID;
			}
		}
		return 0;
	}

	double get_cpu_usage(unsigned long process_id)
	{  
		//cpu数量
		static unsigned long processor_count = 0;
		//上一次的时间
		static __int64 last_time = 0;
		static __int64 last_system_time = 0;

		FILETIME now;
		FILETIME creation_time;
		FILETIME exit_time;
		FILETIME kernel_time;
		FILETIME user_time;
		__int64 system_time;
		__int64 time;

		double cpu_usage = -1;

		if(!processor_count)
		{
			processor_count = GetProcessNumber();
		}

		if(!processor_count)
		{
			return -1;
		}

		GetSystemTimeAsFileTime(&now);

		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION/*PROCESS_ALL_ACCESS*/, false, process_id);
		if (!hProcess)
		{
			return -1;
		}
		if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time))
		{
			return -1;
		}

		system_time = (FileTimeToInt64(kernel_time) + FileTimeToInt64(user_time)) / processor_count;  //CPU使用时间
		time = FileTimeToInt64(now);		//现在的时间

		last_system_time = system_time;
		last_time = time;
		CloseHandle( hProcess );

		Sleep(1000);

		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION/*PROCESS_ALL_ACCESS*/, false, process_id);
		if (!hProcess)
		{
			return -1;
		}
		if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time))
		{
			return -1;
		}
		GetSystemTimeAsFileTime(&now);
		system_time = (FileTimeToInt64(kernel_time) + FileTimeToInt64(user_time)) / processor_count;  //CPU使用时间
		time = FileTimeToInt64(now);		//现在的时间

		CloseHandle( hProcess );

		if(time == last_time)
			return -1;

		return ((double)(system_time - last_system_time) / (double)(time - last_time)) * 100;
	}

	size_t get_memory_usage(unsigned long process_id)
	{
		HANDLE hProcess;
		PROCESS_MEMORY_COUNTERS pmc;
		DWORD dwPriorityClass;
		size_t memory_usage = 0;

		// 获得m_ProcessID的句柄
		hProcess = OpenProcess(  
			PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,
			FALSE, process_id );


		dwPriorityClass = 0;
		dwPriorityClass = GetPriorityClass( hProcess );
		if( !dwPriorityClass )
		{
			return -1;
		}

		if(GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)))
		{
			memory_usage = pmc.WorkingSetSize/(1024 * 1024);//单位MB
		}

		CloseHandle( hProcess );

		return memory_usage;
	}

public:
	double _schedule_server_cpu;
	double _sip_server_cpu;
	size_t _schedule_server_memory;
	size_t _sip_server_memory;

	void performance_statistics()
	{
		//ScheduleServer
		unsigned long process_id = GetCurrentProcessId();

		_schedule_server_cpu = get_cpu_usage(process_id);
		_schedule_server_memory = get_memory_usage(process_id);

		//repro
		process_id = get_process_id("repro.exe");

		_sip_server_cpu = get_cpu_usage(process_id);
		_sip_server_memory = get_memory_usage(process_id);

	}

private:
	unsigned long GetProcessNumber()
	{
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		return (int)info.dwNumberOfProcessors;
	}

	__int64 FileTimeToInt64(const FILETIME& time)
	{
		ULARGE_INTEGER tt;
		tt.LowPart = time.dwLowDateTime;
		tt.HighPart = time.dwHighDateTime;
		return(tt.QuadPart);
	}

};

#endif//_RPOCESS_INFO_H_
