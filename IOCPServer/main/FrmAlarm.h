#if !defined(AFX_FRMALARM_H__F82A63DF_1A08_4A6C_8950_1E780F02E56E__INCLUDED_)
#define AFX_FRMALARM_H__F82A63DF_1A08_4A6C_8950_1E780F02E56E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrmAlarm.h : header file
//
#include "resource.h"
#include "Client.h"
/////////////////////////////////////////////////////////////////////////////
// CFrmAlarm dialog

class CFrmAlarm : public CDialog
{
// Construction
private:
	typedef enum _EnumState//数据值的状态
		{
			NOALARM=0,//正常数据
			FATAL=1,//严重告警
			MAIN=2,//主要告警
			NORMAL=3,//一般告警
			OPEVENT=4,//操作事件
			INVALID5=5//无效数据
		}EnumState,EnumAlarmMode;
	typedef struct _TAlarm
	{
		long ID;
		EnumState Status;
		char Start[1];//开始符[
		char Number[6];//告警序号
		char JG1[1];
		char Name[42];//对象名称
		char JG2[1];
		char Time[19];//时间
		char JG3[1];
		char IDMiaoShu[13];//ID描述
		char JG4[1];
		char AlarmLive[4];//告警级别(严重/主要/一般)
		char JG5[1];
		char AlarmID[6];//告警号
		char JG6[1];
		char AlarmFlag[4];//告警标志(开始/结束/取消/确认)
		char JG7[1];
		char Alarm[32];//告警文本
		char End[1];//结束符]
		char Space[23];//空格
		char Enter[1];//回车
		char Line[1];//换行
	}TAlarm,*PTAlarm;
	typedef struct _SEND_ALARM
	{
		long SendAlarm;//503
		long Cnt;
		TAlarm Values;
	}SEND_ALARM,*PSEND_ALARM;
	PSEND_ALARM m_pAlarm;
public:
	CFrmAlarm(CWnd* pParent = NULL);   // standard constructor
	CClient* m_pClient;
	void SetClient(CClient* pC);
// Dialog Data
	//{{AFX_DATA(CFrmAlarm)
	enum { IDD = IDD_Alarm };
	long	m_ShuJuDianID;
	CString	m_IDMiaoShu;
	CString	m_AlarmLive;
	CString	m_AlarmFlag;
	CString	m_UserName;
	CString	m_AlarmText;
	long	m_AlarmXuHao;
	long	m_AlarmID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrmAlarm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	//{{AFX_MSG(CFrmAlarm)
	afx_msg void OnButtonexit();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnBtnsendalarm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMALARM_H__F82A63DF_1A08_4A6C_8950_1E780F02E56E__INCLUDED_)
