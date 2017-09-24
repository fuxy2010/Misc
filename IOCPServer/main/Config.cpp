#include "stdafx.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define    BUFFER_LITTLE_SIZE        512//����512

CConfig* CConfig::_instance=0;

CConfig* CConfig::Instance()
{
	if (0==_instance )
	{
		_instance=new CConfig();
		static Cleaner cl;//����ʹ˱�����Ϊ�˵����������Զ��ͷ�
	}
	return _instance;
}

CConfig::CConfig()//���������ӻ����޸�������,���Ƕ�������ļ�,���ڴ˺�����ʵ��,
				  //��ͬӦ��ֻ��ô˺�������Ӧ���ݳ�Ա
{
	//ȡ�þ���·��
	
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
	lstrcat(IniName,"GPSServer.ini");//���ļ���

	//��ȡ�����ļ�����
	GetPrivateProfileString("Server","Ip","127.0.0.1",m_sServerIp.GetBuffer(BUFFER_LITTLE_SIZE),BUFFER_LITTLE_SIZE,IniName);  //ͨѶ������IP 
	GetPrivateProfileString("Server", "Port","4660",tmpStr.GetBuffer(BUFFER_LITTLE_SIZE),BUFFER_LITTLE_SIZE,IniName); //ͨѶ������port  
	m_iServerPort = atoi(tmpStr); 
	GetPrivateProfileString("Server", "AccN","20",tmpStr.GetBuffer(BUFFER_LITTLE_SIZE),BUFFER_LITTLE_SIZE,IniName); //һ��post���ӵ�socket����  
	m_iAccN = atoi(tmpStr); 
	GetPrivateProfileString("SYS", "WorkThirds","1",tmpStr.GetBuffer(BUFFER_LITTLE_SIZE),BUFFER_LITTLE_SIZE,IniName); 
	m_iWorkThirds = atoi(tmpStr); 
	GetPrivateProfileString("SYS", "WriteLog","1",tmpStr.GetBuffer(BUFFER_LITTLE_SIZE),BUFFER_LITTLE_SIZE,IniName); //�Ƿ�д��־ 
	if( tmpStr=="1")
		m_bLog=TRUE;
	else
		m_bLog=FALSE;
	
}
CConfig::~CConfig()
{
 
}