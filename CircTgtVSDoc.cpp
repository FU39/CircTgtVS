
// CircTgtVSDoc.cpp: CCircTgtVSDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "CircTgtVS.h"
#endif

#include "CircTgtVSDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

// CCircTgtVSDoc

IMPLEMENT_DYNCREATE(CCircTgtVSDoc, CDocument)

BEGIN_MESSAGE_MAP(CCircTgtVSDoc, CDocument)
END_MESSAGE_MAP()


// CCircTgtVSDoc 构造/析构

CCircTgtVSDoc::CCircTgtVSDoc() noexcept
{
	// TODO: 在此添加一次性构造代码
	m_hDIB = nullptr;
	m_lpInfo = nullptr;
	m_lpInfoHeader = nullptr;
	m_lpRGBQuad = nullptr;
	m_nTotalColors = 0;
	m_lpData = nullptr;

}

CCircTgtVSDoc::~CCircTgtVSDoc()
{
}

BOOL CCircTgtVSDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	m_hDIB = nullptr;
	m_lpInfo = nullptr;
	m_lpInfoHeader = nullptr;
	m_lpRGBQuad = nullptr;
	m_nTotalColors = 0;
	m_lpData = nullptr;

	return TRUE;
}




// CCircTgtVSDoc 序列化

void CCircTgtVSDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CCircTgtVSDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CCircTgtVSDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CCircTgtVSDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CCircTgtVSDoc 诊断

#ifdef _DEBUG
void CCircTgtVSDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCircTgtVSDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCircTgtVSDoc 命令


void CCircTgtVSDoc::OpenFile(CString szFileName)
{
	CString szBmpFileName;
	BITMAPFILEHEADER stuFHeader;
	CFile file;
	BYTE* pbDib;
	long lResult, lBmpWidthBytes, lSize;

	szBmpFileName = szFileName;

	file.Open(szBmpFileName, CFile::modeRead);	// 读模式打开文件
	try
	{
		// 读入位图文件头信息
		lResult = file.Read(&stuFHeader, sizeof(BITMAPFILEHEADER));
//		if (lResult != sizeof(BITMAPFILEHEADER))
//		{
//			gfunc_ErrMessage(ERRMSG_FILE_CANNOTREAD, (LPCTSTR)szBmpFileName, NULL);
//			return;
//		}
	}
	catch (CFileException* e)
	{
		// 对 e 的错误类型进行判断，分别处理。e->GetErrorMessage
		e->Delete();
	}

	AfxGetApp()->BeginWaitCursor();

	// 计算 DIB 内存大小
	lSize = (long)file.GetLength() - sizeof(BITMAPFILEHEADER);
	// 为 DIB 分配全局内存
//	if (!AllocDataBuf(lSize))
//	{
//		gfunc_ErrMessage(ERRMSG_MEM_NOTENOUGH, NULL, NULL);
//		return;
//	}
	if (m_hDIB != NULL)
		GlobalFree(m_hDIB);
	m_hDIB = (HGLOBAL) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, lSize);
//	if (m_hDIB == NULL)
//		return;

	// 指向 DIB 实际像素数据的指针
	pbDib = (BYTE*)::GlobalLock(m_hDIB);
//	if (pbDib == NULL)	
//		gfunc_ErrMessage(ERRMSG_MEM_NOTENOUGH,NULL,NULL);

	try
	{	
		// 锁定物理内存
		lResult = file.Read((void*)pbDib, lSize);
		file.Close();
//		if (lResult != lSize)
//		{
//			gfunc_ErrMessage(ERRMSG_FILE_CANNOTREAD, (LPCTSTR)szBmpFileName, NULL);
//			::GlobalUnlock(m_hDIB);
//			::GlobalFree(m_hDIB);
//			return;
//		}
	}
	catch (CFileException* e)
	{
		e->Delete();
	}
	// 获取位图文件数据信息指针和位图信息头指针
	m_lpInfo = (LPBITMAPINFO)pbDib;
	m_lpInfoHeader = (LPBITMAPINFOHEADER)pbDib;

	//11-29 
	// 颜色数
	if ((m_lpInfoHeader->biClrUsed == 0) && (m_lpInfoHeader->biBitCount < 9))
		m_nTotalColors = (UINT)pow(2, m_lpInfoHeader->biBitCount);
	else
		m_nTotalColors = (UINT)m_lpInfoHeader->biClrUsed;

	if (m_lpInfoHeader->biClrUsed == 0)
		m_lpInfoHeader->biClrUsed = m_nTotalColors;

	// 指向位图颜色索引表项的指针，如果没有颜色索引表，则该值为 nullptr
	m_lpRGBQuad = (RGBQUAD*)(pbDib + m_lpInfoHeader->biSize);
	// 颜色索引表的大小(字节)
	DWORD dwColorTableSize = m_nTotalColors * sizeof(RGBQUAD);
	// 指向位图数据的指针
	m_lpData = pbDib + m_lpInfoHeader->biSize + m_nTotalColors * sizeof(RGBQUAD);
	// 如果没有颜色索引表，则该值为 nullptr
	if ((m_lpRGBQuad == (RGBQUAD*)m_lpData) || (m_nTotalColors == 0))
		m_lpRGBQuad = nullptr;

	// 位图的大小(面积像素单位)
	::GlobalUnlock(m_hDIB);


	lBmpWidthBytes = WIDTHBYTES(m_lpInfoHeader->biWidth * m_lpInfoHeader->biBitCount);
	m_lpInfoHeader->biSizeImage = lBmpWidthBytes * m_lpInfoHeader->biHeight;

//	WriteData2GlobalBuf();

	AfxGetApp()->EndWaitCursor();
	/**/
	UpdateAllViews(NULL);
}
