
// CircTgtVSView.h: CCircTgtVSView 类的接口
//

#pragma once


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
	afx_msg LRESULT OnDlgSelFile(WPARAM wParam, LPARAM lParam);
	CString m_csPath;
	CString* m_csFileList;
};

#ifndef _DEBUG  // CircTgtVSView.cpp 中的调试版本
inline CCircTgtVSDoc* CCircTgtVSView::GetDocument() const
   { return reinterpret_cast<CCircTgtVSDoc*>(m_pDocument); }
#endif

