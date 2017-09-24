// FrmAlarm.cpp : implementation file
//

#include "stdafx.h"
#include "IOCPServer.h"
#include "FrmAlarm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrmAlarm dialog


CFrmAlarm::CFrmAlarm(CWnd* pParent /*=NULL*/)
	: CDialog(CFrmAlarm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrmAlarm)
	m_ShuJuDianID = 0;
	m_IDMiaoShu = _T("");
	m_AlarmLive = _T("");
	m_AlarmFlag = _T("");
	m_UserName = _T("");
	m_AlarmText = _T("");
	m_AlarmXuHao = 0;
	m_AlarmID = 0;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDI_Alarm);
	
}


void CFrmAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrmAlarm)
	DDX_Text(pDX, IDC_EDIT1, m_ShuJuDianID);
	DDX_Text(pDX, IDC_EDIT4, m_IDMiaoShu);
	DDX_CBString(pDX, IDC_COMBO1, m_AlarmLive);
	DDX_CBString(pDX, IDC_COMBO2, m_AlarmFlag);
	DDX_Text(pDX, IDC_EDIT3, m_UserName);
	DDX_Text(pDX, IDC_EDIT8, m_AlarmText);
	DDX_Text(pDX, IDC_EDIT2, m_AlarmXuHao);
	DDX_Text(pDX, IDC_EDIT6, m_AlarmID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrmAlarm, CDialog)
	//{{AFX_MSG_MAP(CFrmAlarm)
	ON_BN_CLICKED(IDC_BUTTONEXIT, OnButtonexit)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTNSENDALARM, OnBtnsendalarm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmAlarm message handlers



void CFrmAlarm::OnButtonexit() 
{
	// TODO: Add your control notification handler code here
	PostMessage(WM_CLOSE, 0, 0);
}

BOOL CFrmAlarm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	m_pAlarm=(PSEND_ALARM)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(SEND_ALARM));
	m_pClient=NULL;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFrmAlarm::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	HeapFree(GetProcessHeap(), 0, m_pAlarm);

	CDialog::OnClose();
}

void CFrmAlarm::OnBtnsendalarm() 
{
	// TODO: Add your control notification handler code here
	CString tmpStr;
	CWnd::UpdateData();
	m_pAlarm->SendAlarm=503;
	m_pAlarm->Cnt=1;
	m_pAlarm->Values.ID=m_ShuJuDianID;
	m_pAlarm->Values.Status=NORMAL;

	_tcsncpy(m_pAlarm->Values.Start,"[",1);
	tmpStr.Format("%06d",m_AlarmXuHao);
	_tcsncpy(m_pAlarm->Values.Number,tmpStr,6);
	_tcsncpy(m_pAlarm->Values.JG1,"	",1);
	_tcsncpy(m_pAlarm->Values.Name,"                                          ",42);
	_tcsncpy(m_pAlarm->Values.Name,m_UserName,_tcslen(m_UserName));
	_tcsncpy(m_pAlarm->Values.JG2,"	",1);
	tmpStr=CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
	_tcsncpy(m_pAlarm->Values.Time,tmpStr,19);
	_tcsncpy(m_pAlarm->Values.JG3,"	",1);
	_tcsncpy(m_pAlarm->Values.IDMiaoShu,"             ",13);
	_tcsncpy(m_pAlarm->Values.IDMiaoShu,m_IDMiaoShu,_tcslen(m_IDMiaoShu));
	_tcsncpy(m_pAlarm->Values.JG4,"	",1);
	_tcsncpy(m_pAlarm->Values.AlarmLive,"    ",4);
	_tcsncpy(m_pAlarm->Values.AlarmLive,m_AlarmLive,_tcslen(m_AlarmLive));
	_tcsncpy(m_pAlarm->Values.JG5,"	",1);
	tmpStr.Format("%06d",m_AlarmID);
	_tcsncpy(m_pAlarm->Values.AlarmID,tmpStr,_tcslen(tmpStr));
	_tcsncpy(m_pAlarm->Values.JG6,"	",1);
	_tcsncpy(m_pAlarm->Values.AlarmFlag,"    ",4);
	_tcsncpy(m_pAlarm->Values.AlarmFlag,m_AlarmFlag,_tcslen(m_AlarmFlag));
	_tcsncpy(m_pAlarm->Values.JG7,"	",1);
	_tcsncpy(m_pAlarm->Values.Alarm,"                                ",32);
	_tcsncpy(m_pAlarm->Values.Alarm,m_AlarmText,_tcslen(m_AlarmText));
	_tcsncpy(m_pAlarm->Values.End,"]",1);
	_tcsncpy(m_pAlarm->Values.Space,"                       ",23);
	_tcsncpy(m_pAlarm->Values.Enter,"\r",1);
	_tcsncpy(m_pAlarm->Values.Line,"\n",1);
	if(m_pClient){
		m_pClient->SendData((void*)m_pAlarm,sizeof(SEND_ALARM));}
}
void CFrmAlarm::SetClient(CClient* pC)
{
	m_pClient=pC;
}