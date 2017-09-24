#include "stdafx.h"
#include "GUID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGUID* CGUID::_instance=0;

CGUID* CGUID::Instance()
{
	if (0==_instance )
	{
		_instance=new CGUID();
		static Cleaner cl;//此类和此变量均为了单件对象能自动释放
	}
	return _instance;
}

CGUID::CGUID()
{
	InitializeCriticalSection(&m_CriAid);
	InitializeCriticalSection(&m_CriXid);
	m_XieYiId=0;
}
CGUID::~CGUID()
{
	DeleteCriticalSection(&m_CriAid);
	DeleteCriticalSection(&m_CriXid);

}
CString CGUID::GetAlarmId(CString sIn)
{
	EnterCriticalSection(&m_CriAid);
		SYSTEMTIME   st; 
		GetSystemTime(&st); 
		m_AlarmId.Format("%.4d%.2d%.2d%.2d%.2d%.2d%.3d"+sIn,st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
		return m_AlarmId;
	LeaveCriticalSection(&m_CriAid);
}
CString CGUID::GetXieYiId()
{
	EnterCriticalSection(&m_CriXid);
		m_XieYiId=(m_XieYiId+1)%10000;
		CString sId;
		sId.Format("%.5d",m_XieYiId);
		return sId;
	LeaveCriticalSection(&m_CriXid);
}



