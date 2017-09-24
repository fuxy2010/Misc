#if !defined(Config_2010_06_11_09_17_40_mxj)
#define Config_2010_06_11_09_17_40_mxj

#if _MSC_VER >1000
#pragma once
#endif

#include "StdAfx.h"

class CConfig
{
private:
	//成员数据
	static CConfig* _instance;//唯一对象
	
    //成员函数
	CConfig();

	//解决单件模式对象自动释放的问题
	class Cleaner 
	{  
	public:  
	  Cleaner(){}  
	  ~Cleaner()  
	  {  
	   if(CConfig::Instance())  
	   delete CConfig::Instance();  
	  }  
	};  
public:
	//配置文件中各配置项数据,不论是增加或者修改配置项,还是多个配置文件
	//则不同应用只需在此处修改或增加对应数据成员

	CString   m_sServerIp;//记录从配置文件读取到的通讯服务器IP
	int       m_iServerPort;  //记录从配置文件读取到的通讯服务器port	
	int       m_iAccN;  //一次AcceptEx的数量
	int		  m_iWorkThirds;//工作者线程数量
	BOOL	  m_bLog;//是否写日志
	//////////////////////////////////////////////////////
	~CConfig();
	static CConfig* Instance();
};

#endif