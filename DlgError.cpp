// DlgError.cpp: 实现文件
//

#include "pch.h"
#include "CircTgtVS.h"
#include "afxdialogex.h"
#include "DlgError.h"


// CDlgError 对话框

IMPLEMENT_DYNAMIC(CDlgError, CDialogEx)

CDlgError::CDlgError(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ERROR, pParent)
{
	m_pErrorFont = new CFont;
	m_nErrorType = 0;
}

CDlgError::~CDlgError()
{
	delete m_pErrorFont;
}

void CDlgError::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ERROR, m_StaticError);
}


BEGIN_MESSAGE_MAP(CDlgError, CDialogEx)
END_MESSAGE_MAP()


// CDlgError 消息处理程序


BOOL CDlgError::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_pErrorFont->CreatePointFont(110, _T("宋体"));	// 11 号字体，Arial
	GetDlgItem(IDC_STATIC_ERROR)->SetFont(m_pErrorFont);
	switch (m_nErrorType)
	{
		case OPEN_FILE_ERROR:
			m_StaticError.SetWindowTextW(_T("文件打开失败！"));
			break;
		case NOT_BAT_ERROR:
			m_StaticError.SetWindowTextW(_T("未批量处理完成！"));
			break;
		default:
			m_StaticError.SetWindowTextW(_T("错误类型不对啦！"));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
