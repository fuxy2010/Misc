#if !defined(DisposeDecorator_2010_07_15_15_45_44_mxj)
#define DisposeDecorator_2010_07_15_15_45_44_mxj

#if _MSC_VER >1000
#pragma once
#endif

#include "Client.h"

class CDisposeDecorator:public CClient//数据处理装饰者抽象类.子类必须覆盖所有的抽象方法后才可以被实例化,否则它还是一个抽象类.
{	
protected:
	PRECEIVE_DATA_QUEUE       m_pRBuff;//接收数据存储队列
	CClient*				  m_pClient;//这里最初要保存的实际是CClientSocket类对象
};

#endif