
// CircTgtVSView.cpp: CCircTgtVSView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "CircTgtVS.h"
#endif

#include "CircTgtVSDoc.h"
#include "CircTgtVSView.h"
#include "DlgFileList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCircTgtVSView

IMPLEMENT_DYNCREATE(CCircTgtVSView, CView)

BEGIN_MESSAGE_MAP(CCircTgtVSView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_PATH_SEL, &CCircTgtVSView::OnPathSel)
	ON_MESSAGE(USER_MSG_SELFILE, &CCircTgtVSView::OnDlgSelFile)
END_MESSAGE_MAP()

// CCircTgtVSView 构造/析构

CCircTgtVSView::CCircTgtVSView() noexcept
{
	// TODO: 在此处添加构造代码
	m_csPath = _T("");
	m_csFileList = nullptr;
	m_nIndex = 0;
	m_bOpen = FALSE;
	m_lWidth = 0;
	m_lHeight = 0;
}

CCircTgtVSView::~CCircTgtVSView()
{
}

BOOL CCircTgtVSView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CCircTgtVSView 绘图

void CCircTgtVSView::OnDraw(CDC* pDC)
{
	CCircTgtVSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	CBrush bkBrush;
	CBitmap* pOldBitmap, memBitmap;
	CRect rect;
	CDC memDC;
	int nShowW = 0;		// nShowW: 计算图片加背景的宽度
	int nShowH = 0;		// nShowH: 计算图片加背景的高度
	int ntop = 0;		// ntop:   图片左上角在背景中的 top 坐标
	int nleft = 0;		// nleft:  图片左上角在背景中的 left 坐标
	int nScanLines;

	// 创建一个与屏幕显示兼容的 memory DC
	// 如果入参为 NULL，则创建一个与应用程序的当前显示器兼容的 memory DC
	memDC.CreateCompatibleDC(pDC);

	// 设定 bitmap 画纸的大小
	// 位图的大小可以用窗口的大小，也可以自己定义
	// e.g. 有滚动条时就要大于当前窗口的大小，在 BitBlt 时决定拷贝 memory 的哪部分到屏幕上
	GetClientRect(&rect);
	if (rect.Width() > m_lWidth)
		nShowW = rect.Width();
	else
		nShowW = m_lWidth;
	if (rect.Height() > m_lHeight)
		nShowH = rect.Height();
	else
		nShowH = m_lHeight;
	memBitmap.CreateCompatibleBitmap(pDC, nShowW, nShowH);

	// 将位图选入到 memory DC 中
	// 只有选入了位图的 memory DC 才有地方绘图，画到指定的位图上
	pOldBitmap = memDC.SelectObject(&memBitmap);

	// 先用背景颜色将位图清除干净
	bkBrush.CreateHatchBrush(7, RGB(255, 255, 255));		// 设置背景颜色
	memDC.FillRect(CRect(0, 0, nShowW, nShowH), &bkBrush);

	// 绘图
	if (rect.Width() > m_lWidth)
		nleft = (rect.Width() - m_lWidth) / 2;
	else
		nleft = 0;
	if (rect.Height() > m_lHeight)
		ntop = (rect.Height() - m_lHeight) / 2;
	else
		ntop = 0;

	nScanLines = StretchDIBits(memDC.m_hDC, nleft, ntop, m_lWidth, m_lHeight, 0, 0, m_lWidth, m_lHeight, pDoc->m_lpData, pDoc->m_lpInfo, DIB_RGB_COLORS, SRCCOPY);
	
	// 将 memory 中的位图拷贝到屏幕上进行显示
	pDC->BitBlt(0, 0, nShowW, nShowH, &memDC, 0, 0, SRCCOPY);

	// 绘图完成后的清理
	memBitmap.DeleteObject();
	memDC.DeleteDC();
}


// CCircTgtVSView 打印

BOOL CCircTgtVSView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CCircTgtVSView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CCircTgtVSView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CCircTgtVSView 诊断

#ifdef _DEBUG
void CCircTgtVSView::AssertValid() const
{
	CView::AssertValid();
}

void CCircTgtVSView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCircTgtVSDoc* CCircTgtVSView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCircTgtVSDoc)));
	return (CCircTgtVSDoc*)m_pDocument;
}
#endif //_DEBUG


// CCircTgtVSView 消息处理程序


void CCircTgtVSView::OnPathSel()
{
	// TODO: 在此添加命令处理程序代码
	wchar_t pBuf[_MAX_PATH];
	BROWSEINFO bi;
	LPITEMIDLIST pidl;
	CString csPath;
	CFileFind finder;
	BOOL bWorking;
	int fileSum = 0;
	int i = 0;

	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.pszDisplayName = pBuf;
	bi.lpszTitle = _T("指定源文件所在的文件夹");
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.hwndOwner = this->m_hWnd;
	if ((pidl = SHBrowseForFolder(&bi)) != NULL)
	{
		SHGetPathFromIDList(pidl, pBuf);
	}

	// 将指定文件夹下的文件名存入 csFileList 中
	m_csPath = pBuf;
	csPath = m_csPath + _T("\\*.*");
	bWorking = finder.FindFile(csPath);
	// 计算文件数
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (!finder.IsDirectory() && !finder.IsDots())
		{
			fileSum++;
		}
	}
	// 存入动态 csFileList
	m_csFileList = new CString[fileSum];
	bWorking = finder.FindFile(csPath);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (!finder.IsDirectory() && !finder.IsDots())
		{
			m_csFileList[i] = finder.GetFileName();
			i++;
		}
	}

	// 将内容传递给 FileList 非模态对话框并调用之
	CDlgFileList* pDlg = new CDlgFileList;
	pDlg->m_fileList = new CString[fileSum];
	pDlg->m_listLen = fileSum;
	for (i = 0; i < fileSum; i++)
	{
		pDlg->m_fileList[i] = m_csFileList[i];
	}
	pDlg->m_hView = this->m_hWnd;
	pDlg->Create(IDD_DIALOG_FILELIST, this);
	pDlg->ShowWindow(TRUE);

	// delete[] m_csFileList;
	// delete[] pDlg->m_fileList;
	// delete pDlg;
}

LRESULT CCircTgtVSView::OnDlgSelFile(WPARAM wParam, LPARAM lParam)
{
	CCircTgtVSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// 参数 lParam 为选中项的 index
	CString csFile;
	// 获取 index 对应的文件全路径
	csFile = m_csPath + "\\" + m_csFileList[lParam];
	// 打开这个文件
	pDoc->OpenFile(csFile);
	m_bOpen = TRUE;
	// 显示到客户区
	m_lWidth = (int)pDoc->m_lpInfo->bmiHeader.biWidth;
	m_lHeight = (int)pDoc->m_lpInfo->bmiHeader.biHeight;
	return 0;
}
