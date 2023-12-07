
// CircTgtVSView.h: CCircTgtVSView 类的接口
//

#pragma once

#include "Circle4Array.h"


class CCircTgtVSView : public CView
{
protected: // 仅从序列化创建
	CCircTgtVSView() noexcept;
	DECLARE_DYNCREATE(CCircTgtVSView)

// 特性
public:
	CCircTgtVSDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CCircTgtVSView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPathSel();
	afx_msg void OnFileBat();
	afx_msg LRESULT OnDlgSelFile(WPARAM wParam, LPARAM lParam);
	CString m_csPath;						// 目录路径
	CString* m_csFileList;					// 目录下文件列表
	int m_nFileSum;							// 目录下文件总数
	int m_nIndex;
	BOOL m_bOpen;
	long m_lWidth, m_lHeight;
	CCircle4Array* m_pCircle;
};

#ifndef _DEBUG  // CircTgtVSView.cpp 中的调试版本
inline CCircTgtVSDoc* CCircTgtVSView::GetDocument() const
   { return reinterpret_cast<CCircTgtVSDoc*>(m_pDocument); }
#endif

