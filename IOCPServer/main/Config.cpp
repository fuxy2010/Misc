#include "stdafx.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define    BUFFER_LITTLE_SIZE        512//缓存512

CConfig* CConfig::_instance=0;

CConfig* CConfig::Instance()
{
	if (0==_instance )
	{
		_instance=new CConfig();
		static Cleaner cl;//此类和此变量均为了单件对象能自动释放
	}
	return _instance;
}

CConfig::CConfig()//不论是增加或者修改配置项,还是多个配置文件,均在此函数中实现,
				  //则不同应用只需该此函数及对应数据成员
{
	//取得绝对路径
	
	CString tmpStr;
	char IniName[MAX_PATH + 1];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	ZeroMemory(IniName,MAX_PATH + 1);
	GetModuleFileName(NULL, IniName, MAX_PATH);
	_splitpath(IniName, drive, dir, fname, ext );
	ZeroMemory(IniName,MAX_PATH + 1);
	lstrcat(IniName,drive);
	lstrcat(IniName,dir);
	lstrcat(IniName,"GPSServer.ini");//置文件名

	//读取配置文件内容
	GetPrivateProfileString("Server","Ip","127.0.0.1",m_sServerIp.GetBuffer(BUFFER_LITTLE_SIZE),BUFFER_LITTLE_SIZE,IniName);  //通讯服务器IP 
	GetPrivateProfileString("Server", "Port","4660",tmpStr.GetBuffer(BUFFER_LITTLE_SIZE),BUFFER_LITTLE_SIZE,IniName); //通讯服务器port  
	m_iServerPort = atoi(tmpStr); 
	GetPrivateProfileString("Server", "AccN","20",tmpStr.GetBuffer(BUFFER_LITTLE_SIZE),BUFFER_LITTLE_SIZE,IniName); //一次post连接的socket个数  
	m_iAccN = atoi(tmpStr); 
	GetPrivateProfileString("SYS", "WorkThirds","1",tmpStr.GetBuffer(BUFFER_LITTLE_SIZE),BUFFER_LITTLE_SIZE,IniName); 
	m_iWorkThirds = atoi(tmpStr); 
	GetPrivateProfileString("SYS", "WriteLog","1",tmpStr.GetBuffer(BUFFER_LITTLE_SIZE),BUFFER_LITTLE_SIZE,IniName); //是否写日志 
	if( tmpStr=="1")
		m_bLog=TRUE;
	else
		m_bLog=FALSE;
	
}
CConfig::~CConfig()
{
 
}