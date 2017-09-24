// FrmMain.cpp : implementation file
//

#include "stdafx.h"
#include "IOCPServer.h"
#include "FrmMain.h"

#include "WriteLog.h"
#include "Config.h"
#include "ShowData.h"
#include "GUID.h"

#include "Client.h"
#include "ClientSocket.h"

#include "ScheduleServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ScheduleServer;

/////////////////////////////////////////////////////////////////////////////
// CFrmMain dialog
void CFrmMain::EnableSendAlarm()
{
	m_SendAlarm.ShowWindow(1);
}
CFrmMain::CFrmMain(CWnd* pParent /*=NULL*/)
	: CDialog(CFrmMain::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrmMain)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAIN);
}

void CFrmMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrmMain)
	DDX_Control(pDX, IDC_SendAlarm, m_SendAlarm);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFrmMain, CDialog)
	//{{AFX_MSG_MAP(CFrmMain)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTONEXIT, OnButtonexit)
	ON_BN_CLICKED(IDC_SendAlarm, OnSendAlarm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmMain message handlers

BOOL CFrmMain::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	//////////////////////////////////////////////////////////////////////////
	char cur_path[MAX_PATH];

	::ZeroMemory(cur_path, sizeof(cur_path));
	::GetModuleFileName(NULL, cur_path, sizeof(cur_path));

	std::string cur_path_str(cur_path);
	cur_path_str = cur_path_str.substr(0, cur_path_str.rfind("\\")) + "\\";

	::SetCurrentDirectory(cur_path_str.c_str());

	//开启日志
	std::map<int, std::string> log_file_map;
	log_file_map[0] = "GPSServer.log";	
	SINGLETON(CLogger).open("", log_file_map, cur_path_str + "log");
	LOG_SET_LEVEL(HIGHEST_LOG_LEVEL);

	//读取配置文件
	SINGLETON(CConfigBox).load("GPSServer.ini");

	SINGLETON(CScheduleServer).start(cur_path_str);
	//////////////////////////////////////////////////////////////////////////

	m_pFrmAlarm=NULL;

	//记录日志
	CWriteLog* OLog=CWriteLog::Instance();
	OLog->WriteLog("程序开始运行");
	//读配置文件
	CConfig* OConfig=CConfig::Instance();
	SetDlgItemText(IDC_STATIC_IP,OConfig->m_sServerIp);
	SetDlgItemInt(IDC_STATIC_Port,OConfig->m_iServerPort);
	//界面显示
	m_ImageList.Create(16,16,ILC_COLOR32,2,2);
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_GREEN));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_GRAY));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_YELLOW));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_RED));
	CShowData* OShow=CShowData::Instance(&m_ListCtrl,&m_ImageList);
	//创建状态栏	
	static UINT indicators[]=
	{
		IDS_PROGRESS,
		IDS_TIMER,
	};
	m_wndStatusBar.Create(this);
	m_wndStatusBar.SetIndicators(indicators,sizeof(indicators)/sizeof(UINT));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);
	UINT nID=0;        //控制状态栏里面的分栏
	m_wndStatusBar.SetPaneInfo(0,nID,SBPS_NORMAL,700);
	m_wndStatusBar.SetPaneText(0,"GPS Server");
	m_wndStatusBar.SetPaneInfo(1,nID,SBPS_NORMAL,130);
	CTime t=CTime::GetCurrentTime();
	CString str=t.Format("%Y-%m-%d %H:%M:%S");
	m_wndStatusBar.SetPaneText(1,str);

	//设置定时器
	SetTimer(THETIMER,1000,NULL);
	SetTimer(TIMERSHOW,200,NULL);
	SetTimer(TIMERCONNECT,10000,NULL);//定时检测是否存在连接正常但没数据传输的情况
	//创建完成端口对象
	m_pOWIocp=new CIOCPServer();
	if(!m_pOWIocp->initIocp(OConfig->m_sServerIp,OConfig->m_iServerPort,OConfig->m_iWorkThirds,OConfig->m_iAccN))
	{
		AfxMessageBox("完成端口对象初始化错误,详细请查看日志文件!",MB_OK);
		PostMessage(WM_CLOSE, 0, 0);
	}

	//返回
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFrmMain::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFrmMain::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFrmMain::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (TIMERSHOW==nIDEvent ) //刷新界面
	{	
		CShowData* OShow=CShowData::Instance();
		OShow->DisplayData();
	}
	if (THETIMER==nIDEvent)
	{
		//显示当前时间
		CTime t=CTime::GetCurrentTime();
		CString str=t.Format("%Y-%m-%d %H:%M:%S");
		m_wndStatusBar.SetPaneText(1,str);
	}
	if(TIMERCONNECT==nIDEvent)//检查有未连接的客户端自动重连,暂时不做
	{
		
	}	
	CDialog::OnTimer(nIDEvent);
}

void CFrmMain::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CWriteLog* OLog=CWriteLog::Instance();
	OLog->WriteLog("程序退出");

	//清除对象
	delete m_pOWIocp;
	if (!m_pFrmAlarm)
	{
		delete m_pFrmAlarm;
	}
	//////////////
	Sleep(10);
	//程序退出
	CDialog::OnClose();
}

void CFrmMain::OnButtonexit() 
{
	// TODO: Add your control notification handler code here
	SINGLETON(CScheduleServer).shutdown();

	PostMessage(WM_CLOSE, 0, 0);
}

void CFrmMain::OnSendAlarm() 
{
	// TODO: Add your control notification handler code here
	m_pFrmAlarm=new CFrmAlarm();
	m_pFrmAlarm->Create(IDD_Alarm);
	m_pFrmAlarm->ShowWindow(SW_SHOW); 
	m_pFrmAlarm->SetClient(m_pClient);
}
void CFrmMain::SetClient(CClient* pC)
{
	m_pClient=pC;
}