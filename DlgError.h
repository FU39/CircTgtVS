﻿#pragma once
#include "afxdialogex.h"

#define OPEN_FILE_ERROR 10
#define NOT_BAT_ERROR   11


// CDlgError 对话框

class CDlgError : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgError)

public:
	CFont* m_pErrorFont;		// 字体
	int m_nErrorType;			// 错误类型，宏定义见上
	CDlgError(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgError();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ERROR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_StaticError;		// 静态文本: 错误类型
	virtual BOOL OnInitDialog();
};
