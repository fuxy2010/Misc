#if !defined(GUID_2010_07_07_09_03_01_mxj)
#define GUID_2010_07_07_09_03_01_mxj

#if _MSC_VER >1000
#pragma once
#endif

class CGUID//���ڲ�������id,���羯����,����Э���ŵ�
{
private:
	//����id
	CString m_AlarmId;//������ȡ�������ǰ������ʱ�������
	long m_XieYiId;//Э����0-10000
	

	CRITICAL_SECTION				m_CriAid;//���ɾ����ŵĹؼ������
	CRITICAL_SECTION				m_CriXid;//����Э���ŵĹؼ������
	//��Ա����
	static CGUID* _instance;//Ψһ����	
    //��Ա����
	CGUID();

	//�������ģʽ�����Զ��ͷŵ�����
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
	//����ӿ�
	CString GetAlarmId(CString sIn="");//����һ���ַ���,�������ص�ǰʱ��+�������
	CString GetXieYiId();

	////////////////////
	~CGUID();
	static CGUID* Instance();
};


#endif