#pragma once

#define LIST_INIT_SIZE 100

/**
* @class CCircleDetail
* @brief Բ����ϸ����.
* 
* ������Բ�����꣬�뾶���������ұ߽�.
*/
class CCircleDetail
{
public:
	long m_lCenterW;		// Բ�� Width(x) ����
	long m_lCenterH;		// Բ�� Height(y) ����
	long m_lRadius;			// Բ�뾶
	int m_nLeft;			// ���Ե
	int m_nRight;			// �ұ�Ե
	int m_nTop;				// �ϱ�Ե
	int m_nBottom;			// �±�Ե

	CCircleDetail();
	~CCircleDetail();
};


/**
* @class CCircle4Array
* @brief һ��ͼƬ�ļ��е�ԲĿ��.
* 
* �����Ա�ṹ˳��洢.
* ���ڶ���Բ�ļ�⣬ֻ��Ҫʵ�����Ԫ�أ��ҵ���ͼƬ��Բ�������٣�
* �ʹ涨�洢����Ϊ LIST_INIT_SIZE��Ҳδʵ�����Ա�ĸ��ֲ���.
*/
class CCircle4Array
{
public:
	CString m_sz4WhichFile;			// �ļ���
	long m_lListSize;				// Բ�Ĵ洢����
	long m_lCircSum;				// ���ļ���Բ����
	CCircleDetail* m_pCircParam;	// ��Բ�Ĳ���

	CCircle4Array();
	~CCircle4Array();
};
