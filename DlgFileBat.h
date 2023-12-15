#pragma once
#include "afxdialogex.h"
#include "Circle4Array.h"


// CDlgFileBat 对话框

class CDlgFileBat : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFileBat)

public:
	CFont* m_pBatFont;
	int m_nFiles;
	BOOL m_bBat;
	CCircle4Array* m_pContent;
	CDlgFileBat(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgFileBat();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FILEBAT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lstBat;
	CStatic m_lStaticIsBat;
	virtual BOOL OnInitDialog();
};
