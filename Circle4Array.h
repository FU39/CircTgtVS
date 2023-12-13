#pragma once

#define LIST_INIT_SIZE 100

/**
* @class CCircleDetail
* @brief 圆的详细参数.
* 
* 包含：圆心坐标，半径，上下左右边界.
*/
class CCircleDetail
{
public:
	long m_lCenterW;		// 圆心 Width(x) 坐标
	long m_lCenterH;		// 圆心 Height(y) 坐标
	long m_lRadius;			// 圆半径
	int m_nLeft;			// 左边缘
	int m_nRight;			// 右边缘
	int m_nTop;				// 上边缘
	int m_nBottom;			// 下边缘

	CCircleDetail();
	~CCircleDetail();
};


/**
* @class CCircle4Array
* @brief 一个图片文件中的圆目标.
* 
* 以线性表结构顺序存储.
* 由于对于圆的检测，只需要实现添加元素，且单张图片中圆数量较少，
* 故规定存储容量为 LIST_INIT_SIZE，也未实现线性表的各种操作.
*/
class CCircle4Array
{
public:
	CString m_sz4WhichFile;			// 文件名
	long m_lListSize;				// 圆的存储容量
	long m_lCircSum;				// 该文件内圆总数
	CCircleDetail* m_pCircParam;	// 各圆的参数

	CCircle4Array();
	~CCircle4Array();
};
