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
	typedef enum _EnumState//����ֵ��״̬
		{
			NOALARM=0,//��������
			FATAL=1,//���ظ澯
			MAIN=2,//��Ҫ�澯
			NORMAL=3,//һ��澯
			OPEVENT=4,//�����¼�
			INVALID5=5//��Ч����
		}EnumState,EnumAlarmMode;
	typedef struct _TAlarm
	{
		long ID;
		EnumState Status;
		char Start[1];//��ʼ��[
		char Number[6];//�澯���
		char JG1[1];
		char Name[42];//��������
		char JG2[1];
		char Time[19];//ʱ��
		char JG3[1];
		char IDMiaoShu[13];//ID����
		char JG4[1];
		char AlarmLive[4];//�澯����(����/��Ҫ/һ��)
		char JG5[1];
		char AlarmID[6];//�澯��
		char JG6[1];
		char AlarmFlag[4];//�澯��־(��ʼ/����/ȡ��/ȷ��)
		char JG7[1];
		char Alarm[32];//�澯�ı�
		char End[1];//������]
		char Space[23];//�ո�
		char Enter[1];//�س�
		char Line[1];//����
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
