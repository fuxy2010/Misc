#if !defined(ShowData_2010_06_23_09_42_50_mxj)
#define ShowData_2010_06_23_09_42_50_mxj

#if _MSC_VER >1000
#pragma once
#endif
//socket���Ӽ����ݽ��շ�����ʾ��,���а���һ��List�����ָ��,һ��ImageList����ָ��,
//list����������ʾ�����б�,ImageList���������Ӧ��ͬ����״̬��ͼ��,0����/1�Ͽ�,
//��0������ͼ���������,1������ͼ�����Ͽ�,���ڽ�������ʱ�����ߵ�ָ�봫��.
//��������ṩ2���ӿں���,ShowData():��׼����ʾ�����ݷ��͸�������;DisplayData():��ʾ����
//����Ӧ�÷�ʽ,��ο�������.
//���ǵ�ʵ�ñ��������ձ���,������Ȼ���ɵ���ģʽ

#define    BUFFER_LITTLE_SIZE        512//����512
#define    BUFFER_SMALL_SIZE         1024//����1k
#define    BUFFER_BIG_SIZE           4096//����4k

class CShowData
{
private:
	//�洢������ʾ��Ϣ�Ķ���
	typedef struct _SHOW_DATA
	{

		char IP[30];//ip:Port  ip��ַ�Ͷ˿�
		char Name[BUFFER_LITTLE_SIZE];//��������
		int  TheState;//��ǰ״̬:0����/1�Ͽ�
		CTime   ConnTime;//����ʱ��
		CTime   ReTime;//�ϴν�������ʱ��
		char ReData[BUFFER_SMALL_SIZE];//�ϴν�������
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

	//��Ա����
	PSHOW_DATA_QUEUE m_pShowQueue;//���ݴ洢����
	CListCtrl*	m_pListCtrl;//��ʾ�б�
	CImageList* m_pImageList;//ͼ���б�

	static CShowData* _instance;

	CRITICAL_SECTION m_csShow_buffer;//��ʾ���ݵ���ʾ�������ٽ���
	//�������ģʽ�����Զ��ͷŵ�����
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