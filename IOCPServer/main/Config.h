#if !defined(Config_2010_06_11_09_17_40_mxj)
#define Config_2010_06_11_09_17_40_mxj

#if _MSC_VER >1000
#pragma once
#endif

#include "StdAfx.h"

class CConfig
{
private:
	//��Ա����
	static CConfig* _instance;//Ψһ����
	
    //��Ա����
	CConfig();

	//�������ģʽ�����Զ��ͷŵ�����
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
	//�����ļ��и�����������,���������ӻ����޸�������,���Ƕ�������ļ�
	//��ͬӦ��ֻ���ڴ˴��޸Ļ����Ӷ�Ӧ���ݳ�Ա

	CString   m_sServerIp;//��¼�������ļ���ȡ����ͨѶ������IP
	int       m_iServerPort;  //��¼�������ļ���ȡ����ͨѶ������port	
	int       m_iAccN;  //һ��AcceptEx������
	int		  m_iWorkThirds;//�������߳�����
	BOOL	  m_bLog;//�Ƿ�д��־
	//////////////////////////////////////////////////////
	~CConfig();
	static CConfig* Instance();
};

#endif