#if !defined(DisposeDecorator_2010_07_15_15_45_44_mxj)
#define DisposeDecorator_2010_07_15_15_45_44_mxj

#if _MSC_VER >1000
#pragma once
#endif

#include "Client.h"

class CDisposeDecorator:public CClient//���ݴ���װ���߳�����.������븲�����еĳ��󷽷���ſ��Ա�ʵ����,����������һ��������.
{	
protected:
	PRECEIVE_DATA_QUEUE       m_pRBuff;//�������ݴ洢����
	CClient*				  m_pClient;//�������Ҫ�����ʵ����CClientSocket�����
};

#endif