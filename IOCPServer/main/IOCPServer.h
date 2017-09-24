// IOCPServer.h : main header file for the IOCPSERVER application
//

#if !defined(AFX_IOCPSERVER_H__58CDAA87_E17D_4D5B_A3AC_718F8C72BCB7__INCLUDED_)
#define AFX_IOCPSERVER_H__58CDAA87_E17D_4D5B_A3AC_718F8C72BCB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CIOCPServerApp:
// See IOCPServer.cpp for the implementation of this class
//

class CIOCPServerApp : public CWinApp
{
public:
	CIOCPServerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIOCPServerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CIOCPServerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IOCPSERVER_H__58CDAA87_E17D_4D5B_A3AC_718F8C72BCB7__INCLUDED_)
