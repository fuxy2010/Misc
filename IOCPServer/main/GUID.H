#if !defined(GUID_2010_07_07_09_03_01_mxj)
#define GUID_2010_07_07_09_03_01_mxj

#if _MSC_VER >1000
#pragma once
#endif

class CGUID//用于产生各种id,比如警情编号,发送协议编号等
{
private:
	//各种id
	CString m_AlarmId;//警情编号取计算机当前年月日时分秒毫秒
	long m_XieYiId;//协议编号0-10000
	

	CRITICAL_SECTION				m_CriAid;//生成警情编号的关键代码段
	CRITICAL_SECTION				m_CriXid;//生成协议编号的关键代码段
	//成员数据
	static CGUID* _instance;//唯一对象	
    //成员函数
	CGUID();

	//解决单件模式对象自动释放的问题
	class Cleaner 
	{  
	public:  
	  Cleaner(){}  
	  ~Cleaner()  
	  {  
	   if(CGUID::Instance())  
	   delete CGUID::Instance();  
	  }  
	};  
public:
	//对外接口
	CString GetAlarmId(CString sIn="");//传入一个字符串,函数返回当前时间+传入参数
	CString GetXieYiId();

	////////////////////
	~CGUID();
	static CGUID* Instance();
};


#endif