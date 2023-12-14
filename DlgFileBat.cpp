// DlgFileBat.cpp: 实现文件
//

#include "pch.h"
#include "CircTgtVS.h"
#include "afxdialogex.h"
#include "DlgFileBat.h"


// CDlgFileBat 对话框

IMPLEMENT_DYNAMIC(CDlgFileBat, CDialogEx)

CDlgFileBat::CDlgFileBat(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FILEBAT, pParent)
{
	m_pBatFont = new CFont;
	m_nFiles = 0;
	m_bBat = FALSE;
	m_pContent = nullptr;
}

CDlgFileBat::~CDlgFileBat()
{
	delete m_pBatFont;
}

void CDlgFileBat::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BAT, m_lstBat);
	DDX_Control(pDX, IDC_STATIC_BAT, m_lStaticIsBat);
}


BEGIN_MESSAGE_MAP(CDlgFileBat, CDialogEx)
END_MESSAGE_MAP()


// CDlgFileBat 消息处理程序


BOOL CDlgFileBat::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString str[] = { _T("文件名"), _T("圆序号"), _T("圆心x坐标"), _T("圆心y坐标"), _T("圆半径") };
	TCHAR buffer[_MAX_U64TOSTR_BASE2_COUNT] = {};
	int nRowIndex = 0;		// 列表行索引

	// 设置字体
	m_pBatFont->CreatePointFont(100, _T("Arial"));	// 10 号字体，Arial
	GetDlgItem(IDC_STATIC_BAT)->SetFont(m_pBatFont);
	GetDlgItem(IDC_STATIC_DETAIL)->SetFont(m_pBatFont);
	GetDlgItem(IDC_LIST_BAT)->SetFont(m_pBatFont);
	if (m_bBat)
		m_lStaticIsBat.SetWindowTextW(_T("批量处理：成功！"));
	else
		m_lStaticIsBat.SetWindowTextW(_T("批量处理：失败！"));
	// 设置目标信息
	for (int i = 0; i < 5; i++)
	{
		if (i == 0)
			m_lstBat.InsertColumn(i, str[i], LVCFMT_LEFT, 450);
		else
			m_lstBat.InsertColumn(i, str[i], LVCFMT_LEFT, 120);
	}
	if (m_pContent != nullptr)
	{
		nRowIndex = 0;
		for (int i = 0; i < m_nFiles; i++)
		{
			for (int j = 0; j < m_pContent[i].m_lCircSum; j++)
			{
				m_lstBat.InsertItem(nRowIndex, m_pContent[i].m_sz4WhichFile);
				_itow_s(j, buffer, 10);
				m_lstBat.SetItemText(nRowIndex, 1, buffer);
				_ltow_s(m_pContent[i].m_pCircParam[j].m_lCenterW, buffer, 10);
				m_lstBat.SetItemText(nRowIndex, 2, buffer);
				_ltow_s(m_pContent[i].m_pCircParam[j].m_lCenterH, buffer, 10);
				m_lstBat.SetItemText(nRowIndex, 3, buffer);
				_ltow_s(m_pContent[i].m_pCircParam[j].m_lRadius, buffer, 10);
				m_lstBat.SetItemText(nRowIndex, 4, buffer);
				nRowIndex++;
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
