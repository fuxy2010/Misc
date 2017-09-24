// FrmMain.h : header file
//

#if !defined(AFX_FRMMAIN_H__7EBD9F97_467B_4BE9_AA94_4C322D14ECA9__INCLUDED_)
#define AFX_FRMMAIN_H__7EBD9F97_467B_4BE9_AA94_4C322D14ECA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "iocp_server.h"
#include "FrmAlarm.h"
#include "Client.h"

const unsigned int THETIMER   = 1 ;//定时点名
const unsigned int TIMERSHOW  =2  ;//刷新列表
const unsigned int TIMERCONNECT=3;
/////////////////////////////////////////////////////////////////////////////
// CFrmMain dialog

class CFrmMain : public CDialog
{
// Construction
public:
	CFrmMain(CWnd* pParent = NULL);	// standard constructor

	CImageList m_ImageList;//列表图像列表对象
	CStatusBar m_wndStatusBar;

	CIOCPServer* m_pOWIocp;

	CFrmAlarm* m_pFrmAlarm;

	CClient* m_pClient;

	void SetClient(CClient* pC);
	void EnableSendAlarm();
// Dialog Data
	//{{AFX_DATA(CFrmMain)
	enum { IDD = IDD_FRMMAIN };
	CButton	m_SendAlarm;
	CListCtrl	m_ListCtrl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmMain)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFrmMain)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnButtonexit();
	afx_msg void OnSendAlarm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMMAIN_H__7EBD9F97_467B_4BE9_AA94_4C322D14ECA9__INCLUDED_)
