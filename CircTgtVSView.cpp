
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
	ON_COMMAND(ID_FILE_BAT, &CCircTgtVSView::OnFileBat)
	ON_COMMAND(ID_INFO_SHOW, &CCircTgtVSView::OnInfoShow)
END_MESSAGE_MAP()

// CCircTgtVSView 构造/析构

CCircTgtVSView::CCircTgtVSView() noexcept
{
	// TODO: 在此处添加构造代码
	m_csPath = _T("");
	m_csFileList = nullptr;
	m_nFileSum = 0;
	m_nIndex = 0;
	m_lWidth = 0;
	m_lHeight = 0;
	m_pDlgFileList = nullptr;
	m_pDlgFileBat = nullptr;
	m_bShowInfo = FALSE;
	m_bFileBat = FALSE;
}

CCircTgtVSView::~CCircTgtVSView()
{
	if (m_csFileList != nullptr)
		delete[] m_csFileList;
	if (m_pDlgFileList != nullptr)
		delete m_pDlgFileList;
	if (m_pDlgFileBat != nullptr)
		delete m_pDlgFileBat;
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
	CBrush* pOldBrush = nullptr;
	CBrush bkBrush;
	LOGBRUSH logBrush{0};
	CBitmap* pOldBitmap = nullptr;
	CBitmap memBitmap;
	CRect rect;
	CDC memDC;
	int nShowW = 0;		// nShowW: 计算图片加背景的宽度
	int nShowH = 0;		// nShowH: 计算图片加背景的高度
	int nTop = 0;		// nTop:   图片左上角在背景中的 top 坐标
	int nLeft = 0;		// nLeft:  图片左上角在背景中的 left 坐标
	int nScanLines = 0;
	int nNameIndex = 0;			// CString 内索引
	CString szName = _T("");	// 目标名称
	CPoint tail, head;			// 箭头的头和尾

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
		nLeft = (rect.Width() - m_lWidth) / 2;
	else
		nLeft = 0;
	if (rect.Height() > m_lHeight)
		nTop = (rect.Height() - m_lHeight) / 2;
	else
		nTop = 0;

	// 选择了"显示目标信息"菜单
	// 已批量处理完成
	if (m_bShowInfo && m_bFileBat)
	{
		for (int i = 0; i < m_nFileSum; i++)
		{
			// 获取目标名称
			szName = pDoc->m_pCircle[i].m_sz4WhichFile;
			nNameIndex = szName.Find('\\');
			while (nNameIndex != -1)
			{
				szName = szName.Mid(nNameIndex + 1);
				nNameIndex = szName.Find('\\');
			}
			nNameIndex = szName.ReverseFind('.');
			if (nNameIndex != -1)
			{
				szName = szName.Left(nNameIndex);
			}
			// 用于判断是否为多目标文件
			nNameIndex = szName.Find(_T("MOBJ"));

			// 使用透明画刷画圆
			logBrush.lbStyle = BS_HOLLOW;
			bkBrush.DeleteObject();
			bkBrush.CreateBrushIndirect(&logBrush);
			pOldBrush = memDC.SelectObject(&bkBrush);

			for (int j = 0; j < pDoc->m_pCircle[i].m_lCircSum; j++)
			{
				// 画圆目标外轮廓
				memDC.Ellipse( \
					(nLeft + pDoc->m_pCircle[i].m_pCircParam[j].m_nLeft), \
					(nTop + pDoc->m_pCircle[i].m_pCircParam[j].m_nTop), \
					(nLeft + pDoc->m_pCircle[i].m_pCircParam[j].m_nRight), \
					(nTop + pDoc->m_pCircle[i].m_pCircParam[j].m_nBottom) \
				);
				// 显示目标名称 (文件名)
				memDC.TextOutW( \
					(nLeft + pDoc->m_pCircle[i].m_pCircParam[j].m_nRight), \
					(nTop + pDoc->m_pCircle[i].m_pCircParam[j].m_nTop - 20), \
					szName \
				);

				// 画箭头
				if (i != 0)
				{
					// 上一个文件的圆形目标圆心
					tail.x = nLeft + pDoc->m_pCircle[i - 1].m_pCircParam[j].m_lCenterW;
					tail.y = nTop + pDoc->m_pCircle[i - 1].m_pCircParam[j].m_lCenterH;
					// 当前文件的圆形目标圆心
					head.x = nLeft + pDoc->m_pCircle[i].m_pCircParam[j].m_lCenterW;
					head.y = nTop + pDoc->m_pCircle[i].m_pCircParam[j].m_lCenterH;
					DrawArrow(&memDC, tail, head);
				}
			}
		}
	}

	// 图像文件在对话框中选中后且已打开
	if (pDoc->m_bOpen)
		nScanLines = StretchDIBits(memDC.m_hDC, nLeft, nTop, m_lWidth, m_lHeight, 0, 0, m_lWidth, m_lHeight, pDoc->m_lpData, pDoc->m_lpInfo, DIB_RGB_COLORS, SRCCOPY);

	// 将 memory 中的位图拷贝到屏幕上进行显示
	pDC->BitBlt(0, 0, nShowW, nShowH, &memDC, 0, 0, SRCCOPY);

	// 绘图完成后的清理
	memDC.SelectObject(pOldBitmap);
	memBitmap.DeleteObject();
	if (pOldBrush != nullptr)
		memDC.SelectObject(pOldBrush);
	bkBrush.DeleteObject();
	memDC.DeleteDC();

	// 复位标志位
	if (m_bShowInfo)
		m_bShowInfo = FALSE;
	// WM_PAINT 优先级低，故在绘图完成后才能进行释放读取内存等操作
	if (pDoc->m_bOpen)
	{
		pDoc->m_bOpen = FALSE;
		::GlobalFree(pDoc->m_hDIB);
		// 防止出现非法指针
		pDoc->m_hDIB = nullptr;
		pDoc->m_lpInfo = nullptr;
		pDoc->m_lpInfoHeader = nullptr;
		pDoc->m_lpRGBQuad = nullptr;
		pDoc->m_lpData = nullptr;
	}
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
	wchar_t pBuf[_MAX_PATH] = {};
	BROWSEINFO bi;
	LPITEMIDLIST pidl;
	CString csPath;
	CFileFind finder;
	BOOL bWorking;
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
	m_nFileSum = 0;
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (!finder.IsDirectory() && !finder.IsDots())
		{
			m_nFileSum++;
		}
	}
	// 存入动态 csFileList
	if (m_csFileList != nullptr)
	{
		delete[] m_csFileList;
		m_csFileList = nullptr;
	}
	m_csFileList = new CString[m_nFileSum];
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

	// 复位批量处理完成标志位，只有成功执行批量处理功能才能置位为 TRUE
	m_bFileBat = FALSE;

	// 将内容传递给 FileList 非模态对话框并调用之
	// 若已选择过目录，须关闭之前的对话框
	if (m_pDlgFileList != nullptr)
	{
		m_pDlgFileList->DestroyWindow();
		delete m_pDlgFileList;
		m_pDlgFileList = nullptr;
	}
	m_pDlgFileList = new CDlgFileList;
	m_pDlgFileList->m_pFileList = new CString[m_nFileSum];
	m_pDlgFileList->m_listLen = m_nFileSum;
	for (i = 0; i < m_nFileSum; i++)
	{
		m_pDlgFileList->m_pFileList[i] = m_csFileList[i];
	}
	m_pDlgFileList->m_hView = this->m_hWnd;
	m_pDlgFileList->Create(IDD_DIALOG_FILELIST, this);
	m_pDlgFileList->ShowWindow(TRUE);

	// 释放内存
	delete[] m_pDlgFileList->m_pFileList;
	m_pDlgFileList->m_pFileList = nullptr;
}

LRESULT CCircTgtVSView::OnDlgSelFile(WPARAM wParam, LPARAM lParam)
{
	CCircTgtVSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return 0;

	CString csFile;
	CDlgError dlgOpenFail;

	// 获取 index 对应的文件全路径
	// 参数 lParam 为选中项的 index
	csFile = m_csPath + "\\" + m_csFileList[lParam];

	// 打开这个文件
	/**
	* 关于如何调用 OnDraw
	* 
	* OpenFile 中调用了 UpdateAllViews
	* 故若未重载 OnUpdate，则无需在下方调用 Invalidate
	* 若已重载 OnUpdate，则在重载中须调用 Invalidate，否则在下方须调用 Invalidate
	* 此处选则在 OnUpdate 中使用之
	*/
	pDoc->OpenFile(csFile);
	// 未打开文件成功，弹出错误模态显示框
	if (!pDoc->m_bOpen)
	{
		dlgOpenFail.m_nErrorType = OPEN_FILE_ERROR;
		dlgOpenFail.DoModal();
	}

	// 显示到客户区: 在 OnUpdate 中完成

//	Invalidate(TRUE);

	return 0;
}


void CCircTgtVSView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// TODO: 在此添加专用代码和/或调用基类
	CCircTgtVSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	/**
	* 在 CCircTgtVSDoc 中添加 m_bOpen 成员变量，通过它判断是否执行下面代码段
	* DEMO 中在 CCircTgtVSView 添加 m_bOpen 成员变量，但调用 OnUpdate 是在 UpdateAllViews 中完成的
	* 这导致 DEMO 中 m_bOpen = TRUE 还未执行，就已经调用了 OnUpdate，故有一些问题
	*/
	if (pDoc->m_bOpen)
	{
		m_lWidth = (long)pDoc->m_lpInfoHeader->biWidth;
		m_lHeight = (long)pDoc->m_lpInfoHeader->biHeight;
	}
	else
	{
		m_lWidth = 0;
		m_lHeight = 0;
	}
	Invalidate(TRUE);
}


void CCircTgtVSView::OnFileBat()
{
	// TODO: 在此添加命令处理程序代码
	CCircTgtVSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// 批量处理
	pDoc->m_lShowW = 0;
	pDoc->m_lShowH = 0;
	m_bFileBat = pDoc->BatchDetectArray(m_csPath, m_csFileList, m_nFileSum);

	// 创建非模态对话框显示目标信息
	// 若已批量处理过，须关闭之前的对话框
	if (m_pDlgFileBat != nullptr)
	{
		m_pDlgFileBat->DestroyWindow();
		delete m_pDlgFileBat;
		m_pDlgFileBat = nullptr;
	}
	m_pDlgFileBat = new CDlgFileBat;

	m_pDlgFileBat->m_bBat = m_bFileBat;
	m_pDlgFileBat->m_nFiles = m_nFileSum;
	m_pDlgFileBat->m_pContent = pDoc->m_pCircle;
	m_pDlgFileBat->Create(IDD_DIALOG_FILEBAT, this);
	m_pDlgFileBat->ShowWindow(TRUE);
}


void CCircTgtVSView::OnInfoShow()
{
	// TODO: 在此添加命令处理程序代码
	CCircTgtVSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CDlgError dlgBatError;

	// 批量处理成功，显示目标信息
	if (m_bFileBat)
	{
		m_bShowInfo = TRUE;
		m_lWidth = pDoc->m_lShowW;
		m_lHeight = pDoc->m_lShowH;

		Invalidate(TRUE);
	}
	// 未批量处理或批量处理失败，弹出错误模态显示框
	else
	{
		m_bShowInfo = TRUE;
		m_lWidth = 0;
		m_lHeight = 0;
		dlgBatError.m_nErrorType = NOT_BAT_ERROR;
		dlgBatError.DoModal();

		Invalidate(TRUE);
	}
}


// 绘制图形函数


/**
* @brief 绘制箭头函数.
* 
* 传入DC，箭头和箭尾，在 DC 中绘制箭头.
*/
void CCircTgtVSView::DrawArrow(CDC* pDC, CPoint tail, CPoint head)
{
	// 入参检查
	if (pDC == nullptr)
		return;
	const double PI = acos(-1);
	double dbAngle = 0;				// 线段倾斜角
	CPoint rightEnd, leftEnd;		// 箭头左右端点
	CPen* pOldPen = nullptr;
	CPen pen;
	double dbLength = 0;			// 线段长度
	double dbArrowLength = 0;		// 箭头长度

	pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	pOldPen = pDC->SelectObject(&pen);
	// 计算倾斜角，注意 arctan 的值域
	if (head.x == tail.x && ((head.y - tail.y) > 0))
		dbAngle = PI / 2;
	else if (head.x == tail.x && ((head.y - tail.y) < 0))
		dbAngle = -PI / 2;
	else if ((head.x - tail.x) > 0)
		dbAngle = atan(1.0 * (head.y - tail.y) / (head.x - tail.x));
	else
		dbAngle = atan(1.0 * (head.y - tail.y) / (head.x - tail.x)) + PI;
	// 规定箭头大小
	dbLength = sqrt(pow((head.x - tail.x), 2) + pow((head.y - tail.y), 2));
	if (dbLength > 10)
		dbArrowLength = 10;
	else
		dbArrowLength = dbLength;
	rightEnd.x = (long)(head.x - dbArrowLength * cos(dbAngle + PI / 6));
	rightEnd.y = (long)(head.y - dbArrowLength * sin(dbAngle + PI / 6));
	leftEnd.x = (long)(head.x - dbArrowLength * cos(dbAngle - PI / 6));
	leftEnd.y = (long)(head.y - dbArrowLength * sin(dbAngle - PI / 6));
	// 画线和箭头
	pDC->MoveTo(tail);
	pDC->LineTo(head);
	pDC->MoveTo(rightEnd);
	pDC->LineTo(head);
	pDC->MoveTo(leftEnd);
	pDC->LineTo(head);

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}
