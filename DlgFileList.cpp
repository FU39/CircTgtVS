// DlgFileList.cpp: 实现文件
//

#include "pch.h"
#include "CircTgtVS.h"
#include "afxdialogex.h"
#include "DlgFileList.h"


// CDlgFileList 对话框

IMPLEMENT_DYNAMIC(CDlgFileList, CDialogEx)

CDlgFileList::CDlgFileList(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FILELIST, pParent)
{
	m_hView = nullptr;
	m_fileList = nullptr;
	m_listLen = 0;
}

CDlgFileList::~CDlgFileList()
{
}

void CDlgFileList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lstFileList);
}


BEGIN_MESSAGE_MAP(CDlgFileList, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST1, &CDlgFileList::OnSelchangeList1)
END_MESSAGE_MAP()


// CDlgFileList 消息处理程序


BOOL CDlgFileList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	for (int i = 0; i < m_listLen; i++)
	{
		m_lstFileList.InsertString(i, m_fileList[i]);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgFileList::OnSelchangeList1()
{
	// TODO: 在此添加控件通知处理程序代码
	LPARAM nSelIndex;
	nSelIndex = m_lstFileList.GetCurSel();
	::SendMessage(m_hView, USER_MSG_SELFILE, 0, nSelIndex);
}
