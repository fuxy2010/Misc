// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__21DD18E0_7146_4756_9FE7_F41AF8BD0BF6__INCLUDED_)
#define AFX_STDAFX_H__21DD18E0_7146_4756_9FE7_F41AF8BD0BF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

using namespace std;

#if defined(_WIN32)
#include <iphlpapi.h>
#include <Nb30.h>
#include <Winsock2.h>
#include <mmsystem.h>
#pragma comment(lib, "IPHlpApi.Lib")
#pragma comment(lib, "Netapi32.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Kernel32.lib")	
#endif

#pragma comment(lib, "../mysql/libmysql.lib")

//#pragma comment(lib, "../utility/rtsp/RTSPServer.lib")
//RTSPServer所生成的库文件均在../RTSPServer/bin/，头文件则在../RTSPServer/
#ifdef _DEBUG
//#pragma comment(lib, "../RTSPServer/bin/RTSPServerD.lib")
#else
//#pragma comment(lib, "../RTSPServer/bin/RTSPServer.lib")
#endif

#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "Psapi.lib")

#ifndef HIGHEST_LOG_LEVEL
#define HIGHEST_LOG_LEVEL 10
#endif

#ifdef _DEBUG
#define LOG_WRITE(log_str, log_level, show_on_screen) if (SINGLETON(CLogger).can_trace(log_level)) {std::ostringstream _oss; _oss << log_str << "\n{" << __FUNCTION__ << "(" << __LINE__ <<")}";\
	SINGLETON(CScheduleServer).write_log(_oss.str(), log_level, show_on_screen);}
#else
#define LOG_WRITE(log_str, log_level, show_on_screen) if (SINGLETON(CLogger).can_trace(log_level)) {std::ostringstream _oss; _oss << log_str << "\n{" << __FUNCTION__ << "(" << __LINE__ <<")}";\
	SINGLETON(CScheduleServer).write_log(_oss.str(), log_level, show_on_screen);}
#endif

#define LOG_SET_LEVEL(level) SINGLETON(CLogger).set_trace_level(level);
#define LOG_SET_ONSCREEN(level) SINGLETON(CLogger).set_on_screen(level);

#endif // !defined(AFX_STDAFX_H__21DD18E0_7146_4756_9FE7_F41AF8BD0BF6__INCLUDED_)
