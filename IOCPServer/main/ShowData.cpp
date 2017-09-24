#include "stdafx.h"
#include "ShowData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CShowData* CShowData::_instance=0;
//构建函数
CShowData::CShowData(CListCtrl* pL,CImageList* pI)
{
	m_pShowQueue=(CShowData::PSHOW_DATA_QUEUE)HeapAlloc(
		GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CShowData::_SHOW_DATA_QUEUE));
	m_pListCtrl=pL;
	m_pImageList=pI;
	InitializeCriticalSection(&m_csShow_buffer);
	//初始化列表项
	m_pListCtrl->InsertColumn(0,"Address",LVCFMT_LEFT,160); //插入列
	//fym m_pListCtrl->InsertColumn(1,"Name",LVCFMT_LEFT,10); 
	m_pListCtrl->InsertColumn(1,"Connecton Time",LVCFMT_LEFT,140);
	m_pListCtrl->InsertColumn(2,"Recv Time",LVCFMT_LEFT,140);
	m_pListCtrl->InsertColumn(3,"Data",LVCFMT_LEFT,300);
	m_pListCtrl->SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES);
	m_pListCtrl->SetImageList(m_pImageList,LVSIL_SMALL);
}

//析构函数
CShowData::~CShowData()
{
	HeapFree(GetProcessHeap(), 0, m_pShowQueue);
	DeleteCriticalSection(&m_csShow_buffer);
}
//单件注册函数
CShowData* CShowData::Instance(CListCtrl* pL,CImageList* pI)
{
	if (0==_instance )
	{
		_instance=new CShowData(pL,pI);
		static Cleaner cl;//此类和此变量均为了单件对象能自动释放
	}
	return _instance;
}
//准备显示函数
void CShowData::ShowData(CString IP,CString Name,int TheState,CTime ConnTime,CTime ReTime,CString ReData)
{
	EnterCriticalSection(&m_csShow_buffer);	
		_tcsncpy(m_pShowQueue->DataQueue[m_pShowQueue->Head].IP,IP,30);
		_tcsncpy(m_pShowQueue->DataQueue[m_pShowQueue->Head].Name,Name,BUFFER_LITTLE_SIZE);
		m_pShowQueue->DataQueue[m_pShowQueue->Head].TheState=TheState;
		m_pShowQueue->DataQueue[m_pShowQueue->Head].ConnTime=ConnTime;
		m_pShowQueue->DataQueue[m_pShowQueue->Head].ReTime=ReTime;
		_tcsncpy(m_pShowQueue->DataQueue[m_pShowQueue->Head].ReData,ReData,BUFFER_SMALL_SIZE);
		m_pShowQueue->Head=(m_pShowQueue->Head+1)%BUFFER_BIG_SIZE;
	LeaveCriticalSection(&m_csShow_buffer);
}
void CShowData::DisplayData()
{
	CString IP;
	CString Name;
	int TheState;
	CTime ConnTime;
	CTime ReTime;
	CString ReData;

	LVITEM lvi;
	CString strText;
	while(m_pShowQueue->Tail!=m_pShowQueue->Head)
		{
			IP=m_pShowQueue->DataQueue[m_pShowQueue->Tail].IP;
			Name=m_pShowQueue->DataQueue[m_pShowQueue->Tail].Name;
			TheState=m_pShowQueue->DataQueue[m_pShowQueue->Tail].TheState;
			ConnTime=m_pShowQueue->DataQueue[m_pShowQueue->Tail].ConnTime;
			ReTime=m_pShowQueue->DataQueue[m_pShowQueue->Tail].ReTime;
			ReData=m_pShowQueue->DataQueue[m_pShowQueue->Tail].ReData;

			int nItem; 
			LV_FINDINFO FindInfo; 
			ZeroMemory( &FindInfo, sizeof(LV_FINDINFO) );
			FindInfo.flags  = LVFI_STRING; 
			FindInfo.psz = IP; 

			nItem=m_pListCtrl->FindItem(&FindInfo,-1); 
			if(nItem==-1 )//添加行
			{
				//Set subitem 0 ip
				ZeroMemory( &lvi, sizeof(LVITEM) );	
				lvi.mask =  LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE;
				lvi.pszText  = const_cast<char*>(IP.GetBuffer(0));//将该监控器的IP+Port后指定为该item的唯一标识
				lvi.iItem    = m_pListCtrl->GetItemCount();
				lvi.iSubItem = 0;
				lvi.iImage = TheState;
				m_pListCtrl->InsertItem(&lvi);
				lvi.mask =  LVIF_TEXT;

				//Set subitem 1 名称
				//fym lvi.iSubItem =1;
				//fym lvi.pszText = const_cast<char*>(Name.GetBuffer(0));
				//fym m_pListCtrl->SetItem(&lvi);

				// Set subitem 2 连接时间
				lvi.iSubItem =1;
				strText=ConnTime.Format("%Y-%m-%d %H:%M:%S ");
				lvi.pszText = const_cast<char*>(strText.GetBuffer(0));
				m_pListCtrl->SetItem(&lvi);

				// Set subitem 3 收发数据时间
				lvi.iSubItem =2;
				if (ReTime==0)
				{
					strText="";
				}
				else
				{
					strText=ReTime.Format("%Y-%m-%d %H:%M:%S ");
				}
				lvi.pszText = const_cast<char*>(strText.GetBuffer(0));
				m_pListCtrl->SetItem(&lvi);

				// Set subitem 4 备注
				lvi.iSubItem =3;
				lvi.pszText = const_cast<char*>(ReData.GetBuffer(0));
				m_pListCtrl->SetItem(&lvi);
			}
			else//修改行
			{
				m_pListCtrl->SetItem(nItem,0,LVIF_IMAGE,NULL,TheState,0,0,0);
				
				//fym m_pListCtrl->SetItem(nItem,1,LVIF_TEXT,const_cast<char*>(Name.GetBuffer(0)),TheState,0,0,0);

				strText=ConnTime.Format("%Y-%m-%d %H:%M:%S ");
				m_pListCtrl->SetItem(nItem,1,LVIF_TEXT,const_cast<char*>(strText.GetBuffer(0)),TheState,0,0,0);

				if (ReTime==0)
				{
					strText="";
				}
				else
				{
					strText=ReTime.Format("%Y-%m-%d %H:%M:%S ");
				}
				m_pListCtrl->SetItem(nItem,2,LVIF_TEXT,const_cast<char*>(strText.GetBuffer(0)),TheState,0,0,0);

				m_pListCtrl->SetItem(nItem,3,LVIF_TEXT,const_cast<char*>(ReData.GetBuffer(0)),TheState,0,0,0);
			}//if end
			m_pShowQueue->Tail=(m_pShowQueue->Tail+1)%BUFFER_BIG_SIZE;
		}//while end
}