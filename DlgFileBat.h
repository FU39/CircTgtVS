#pragma once
#include "afxdialogex.h"
#include "Circle4Array.h"


// CDlgFileBat 对话框

class CDlgFileBat : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFileBat)

public:
	CFont* m_pBatFont;				// 字体
	int m_nFiles;					// 文件数量
	BOOL m_bBat;					// 批量处理完成标志位
	CCircle4Array* m_pContent;		// 圆目标数据列
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
	CListCtrl m_lstBat;				// 列表视图控件
	CStatic m_lStaticIsBat;			// 静态文本: 是否批量处理成功
	virtual BOOL OnInitDialog();
};
