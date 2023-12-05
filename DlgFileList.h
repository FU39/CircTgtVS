#pragma once
#include "afxdialogex.h"

#define USER_MSG_SELFILE (WM_USER + 1)

// CDlgFileList 对话框

class CDlgFileList : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFileList)

public:
	HWND m_hView;
	CString* m_fileList;
	int m_listLen;
	CDlgFileList(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgFileList();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FILELIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_lstFileList;
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList1();
};
