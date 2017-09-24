#if !defined(ShowData_2010_06_23_09_42_50_mxj)
#define ShowData_2010_06_23_09_42_50_mxj

#if _MSC_VER >1000
#pragma once
#endif
//socket连接及数据接收发送显示类,其中包含一个List类对象指针,一个ImageList对象指针,
//list对象用来显示连接列表,ImageList对象包含对应不同连接状态的图标,0连接/1断开,
//即0索引的图标代表连接,1索引的图标代表断开,请在建立对象时将二者的指针传入.
//本类对象提供2个接口函数,ShowData():将准备显示的数据发送给本对象;DisplayData():显示数据
//具体应用方式,请参考本工程.
//考虑到实用本类对象的普遍性,此类仍然做成单件模式

#define    BUFFER_LITTLE_SIZE        512//缓存512
#define    BUFFER_SMALL_SIZE         1024//缓存1k
#define    BUFFER_BIG_SIZE           4096//缓存4k

class CShowData
{
private:
	//存储界面显示信息的队列
	typedef struct _SHOW_DATA
	{

		char IP[30];//ip:Port  ip地址和端口
		char Name[BUFFER_LITTLE_SIZE];//连接名称
		int  TheState;//当前状态:0连接/1断开
		CTime   ConnTime;//连接时间
		CTime   ReTime;//上次接收数据时间
		char ReData[BUFFER_SMALL_SIZE];//上次接收数据
	}SHOW_DATA;
	typedef struct _SHOW_DATA_QUEUE
	{
		SHOW_DATA DataQueue[BUFFER_BIG_SIZE];
		DWORD Head;
		DWORD Tail;
		_SHOW_DATA_QUEUE()
		{
			Head=0;
			Tail=0;
		}
	}SHOW_DATA_QUEUE,*PSHOW_DATA_QUEUE;

	//成员变量
	PSHOW_DATA_QUEUE m_pShowQueue;//数据存储队列
	CListCtrl*	m_pListCtrl;//显示列表
	CImageList* m_pImageList;//图像列表

	static CShowData* _instance;

	CRITICAL_SECTION m_csShow_buffer;//显示内容到显示队列用临界区
	//解决单件模式对象自动释放的问题
	class Cleaner 
	{  
	public:  
	  Cleaner(){}  
	  ~Cleaner()  
	  {  
	   if(CShowData::Instance())  
	   delete CShowData::Instance();  
	  }  
	};  
public:
	CShowData(CListCtrl* pL,CImageList* pI);
	~CShowData();
	
	static CShowData* Instance(CListCtrl* pL=NULL,CImageList* pI=NULL);

	void ShowData(CString IP,CString Name,int TheState,CTime ConnTime,CTime ReTime,CString ReData);
	void DisplayData();
};



#endif