#include "pch.h"
#include "Circle4Array.h"

CCircleDetail::CCircleDetail()
{
	m_lCenterW = 0;
	m_lCenterH = 0;
	m_lRadius = 0;
	m_nLeft = 0;
	m_nRight = 0;
	m_nTop = 0;
	m_nBottom = 0;
}

CCircleDetail::~CCircleDetail()
{
}

CCircle4Array::CCircle4Array()
{
	m_sz4WhichFile = _T("");
	m_lListSize = LIST_INIT_SIZE;
	m_lCircSum = 0;
	m_pCircParam = new CCircleDetail[LIST_INIT_SIZE];
}

CCircle4Array::~CCircle4Array()
{
	// 析构函数中要释放内存
	delete[] m_pCircParam;
}
