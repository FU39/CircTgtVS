
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
	m_bOpen = FALSE;
	m_hDIB = nullptr;
	m_lpInfo = nullptr;
	m_lpInfoHeader = nullptr;
	m_lpRGBQuad = nullptr;
	m_nTotalColors = 0;
	m_lpData = nullptr;
	m_pCircle = nullptr;
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
	m_bOpen = FALSE;
	m_hDIB = nullptr;
	m_lpInfo = nullptr;
	m_lpInfoHeader = nullptr;
	m_lpRGBQuad = nullptr;
	m_nTotalColors = 0;
	m_lpData = nullptr;
	m_pCircle = nullptr;

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
	CFile file;
	LPBITMAPFILEHEADER lpFileHeader = nullptr;
	BYTE* pbDib = nullptr;
	long lResult, lBmpWidthBytes, lSize;

	m_bOpen = file.Open(szFileName, CFile::modeRead);	// 读模式打开文件
	try
	{
		// 读入位图文件头信息
		lpFileHeader = new BITMAPFILEHEADER;
		lResult = file.Read(lpFileHeader, sizeof(BITMAPFILEHEADER));
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
	if (m_hDIB != nullptr)
		GlobalFree(m_hDIB);
	m_hDIB = (HGLOBAL)::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, lSize);

	// 指向 DIB 实际像素数据的指针
	pbDib = (BYTE*)::GlobalLock(m_hDIB);
	try
	{	
		// 锁定物理内存
		lResult = file.Read((void*)pbDib, lSize);
		file.Close();
	}
	catch (CFileException* e)
	{
		e->Delete();
	}
	// 获取位图文件数据信息指针和位图信息头指针
	m_lpInfo = (LPBITMAPINFO)pbDib;
	m_lpInfoHeader = (LPBITMAPINFOHEADER)pbDib;

	// 11-29 
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

	AfxGetApp()->EndWaitCursor();
	// 由于未重载 CView 的 OnUpdate 成员函数，故调用 CView 的 OnUpdate 函数
	// 其中，已调用 Invalidate(TRUE); 故 OnDlgSelFile 中无需再调用之，即可重绘视图
	UpdateAllViews(NULL);

	delete lpFileHeader;
	lpFileHeader = nullptr;
	pbDib = nullptr;
}

/**
* @brief 转黑白图片函数
* 
* 测试函数，软件不需要实现输出黑白图片功能，只需要实现能获得二值化的像素数据
*/
void CCircTgtVSDoc::Image2BlackWhite()
{
	CFile srcFile, desFile;
	CString srcFileName, desFileName;
	LPBITMAPFILEHEADER fileHeader = nullptr;
	BYTE* pbDib = nullptr;
	LPBITMAPINFOHEADER infoHeader = nullptr;
	RGBQUAD* lpRGBQuad = nullptr;
	BYTE* sourceData = nullptr;
	RGBQUAD* ipRGB = nullptr;
	BYTE* desData = nullptr;
	long lResult, lSize, lSrcWidthBytes, lDesWidthBytes, lHeight, lWidth;
	UINT nTotalColors = 0;
	DWORD dwColorTableSize = 0;
	BOOL isSuccess = FALSE;
	int pixelValue = 0;
	int k = 0;

	srcFileName = _T("F:\\0_coding\\TestFiles\\MOBJ0.bmp");
	isSuccess = srcFile.Open(srcFileName, CFile::modeRead);	// 读模式打开文件
	fileHeader = new BITMAPFILEHEADER;
	lResult = srcFile.Read(fileHeader, sizeof(BITMAPFILEHEADER));
	lSize = (long)srcFile.GetLength() - sizeof(BITMAPFILEHEADER);
	pbDib = new BYTE[lSize];
	lResult = srcFile.Read(pbDib, lSize);
	srcFile.Close();
	infoHeader = (LPBITMAPINFOHEADER)pbDib;
	// 颜色数
	if ((infoHeader->biClrUsed == 0) && (infoHeader->biBitCount < 9))
		nTotalColors = (UINT)pow(2, infoHeader->biBitCount);
	else
		nTotalColors = (UINT)infoHeader->biClrUsed;

	if (infoHeader->biClrUsed == 0)
		infoHeader->biClrUsed = nTotalColors;

	// 指向位图颜色索引表项的指针，如果没有颜色索引表，则该值为 nullptr
	lpRGBQuad = (RGBQUAD*)(pbDib + infoHeader->biSize);
	// 颜色索引表的大小(字节)
	dwColorTableSize = nTotalColors * sizeof(RGBQUAD);
	// 指向位图数据的指针
	sourceData = pbDib + infoHeader->biSize + nTotalColors * sizeof(RGBQUAD);
	// 如果没有颜色索引表，则该值为 nullptr
	if ((lpRGBQuad == (RGBQUAD*)sourceData) || (nTotalColors == 0))
		lpRGBQuad = nullptr;

	lHeight = infoHeader->biHeight;
	lWidth = infoHeader->biWidth;
	lSrcWidthBytes = WIDTHBYTES(infoHeader->biWidth * infoHeader->biBitCount);

	ipRGB = new RGBQUAD[2];

	isSuccess = FALSE;
	desFileName = _T("F:\\0_coding\\MFCProjects\\测试文件\\0.bmp");
	isSuccess = desFile.Open(desFileName, CFile::modeWrite);		// 写模式打开文件

	// 修改信息头
	// 信息头共有 11 部分，灰度化时需要修改 4 部分
	infoHeader->biBitCount = 8;		// 转换成二值图后，颜色深度由 24 位变为 8 位
	lDesWidthBytes = WIDTHBYTES(infoHeader->biWidth * infoHeader->biBitCount);
	infoHeader->biSizeImage = lDesWidthBytes * infoHeader->biHeight;
	infoHeader->biClrUsed = 2;		// 颜色索引表数量，二值图为 2
	infoHeader->biClrImportant = 0;	// 重要颜色索引为 0，表示都重要
	// 修改文件头
	// 文件头共有 5 部分，灰度化时需要修改 2 部分
	fileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD);	// 数据区偏移量，等于文件头，信息头，索引表的大小之和
	fileHeader->bfSize = fileHeader->bfOffBits + infoHeader->biSizeImage;								// 文件大小，等于偏移量加上数据区大小
	ipRGB[1].rgbBlue = ipRGB[1].rgbGreen = ipRGB[1].rgbRed = ipRGB[1].rgbReserved = 0;					// 调色板颜色为黑色对应的索引为 0
	ipRGB[0].rgbBlue = ipRGB[0].rgbGreen = ipRGB[0].rgbRed = 255;										// 白色对应的索引为 150-255
	ipRGB[1].rgbReserved = 0;

	desFile.Write(fileHeader, sizeof(BITMAPFILEHEADER));
	desFile.Write(infoHeader, sizeof(BITMAPINFOHEADER));
	desFile.Write(ipRGB, 2 * sizeof(RGBQUAD));

	desData = new BYTE[lDesWidthBytes];

	// 24 位色深数据 转为 8 位色深数据，且将灰度图二值化
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			pixelValue = (int)(0.114 * (float)sourceData[i * lSrcWidthBytes + 3 * j] + 0.587 * (float)sourceData[i * lSrcWidthBytes + 3 * j + 1] + 0.299 * (float)sourceData[i * lSrcWidthBytes + 3 * j + 2]);
			// 将灰度值转化为二值，这里选取的阈值为 160-190 都可以
			if (pixelValue >= 160)
			{
				desData[j] = 0;	// 白色
			}
			else
			{
				desData[j] = 1;	// 黑色
			}
		}
		for (int k = lWidth; k < lDesWidthBytes; k++)
		{
			desData[k] = 0x00;
		}
		desFile.Write(desData, lDesWidthBytes);
	}
	desFile.Close();

	delete fileHeader;
	delete[] pbDib;
	delete[] ipRGB;
	delete[] desData;
	fileHeader = nullptr;
	pbDib = nullptr;
	infoHeader = nullptr;
	lpRGBQuad = nullptr;
	sourceData = nullptr;
	ipRGB = nullptr;
	desData = nullptr;
}

/**
* @brief 24 位色深图像数据二值化函数.
*
* @return 图像数据二值化数组
*/
BYTE* CCircTgtVSDoc::RGB2TwoValue(BYTE* pbDib)
{
	LPBITMAPINFOHEADER infoHeader = nullptr;
	RGBQUAD* lpRGBQuad = nullptr;
	BYTE* lpData = nullptr;				// 图像数据
	BYTE* lpTwoValueData = nullptr;		// 图像二值化数据
	long lWidthBytes, lHeight, lWidth;
	UINT nTotalColors = 0;
	DWORD dwColorTableSize = 0;
	BOOL isSuccess = FALSE;
	int pixelValue = 0;

	infoHeader = (LPBITMAPINFOHEADER)pbDib;
	// 颜色数
	if ((infoHeader->biClrUsed == 0) && (infoHeader->biBitCount < 9))
		nTotalColors = (UINT)pow(2, infoHeader->biBitCount);
	else
		nTotalColors = (UINT)infoHeader->biClrUsed;

	if (infoHeader->biClrUsed == 0)
		infoHeader->biClrUsed = nTotalColors;

	// 指向位图颜色索引表项的指针，如果没有颜色索引表，则该值为 nullptr
	lpRGBQuad = (RGBQUAD*)(pbDib + infoHeader->biSize);
	// 颜色索引表的大小(字节)
	dwColorTableSize = nTotalColors * sizeof(RGBQUAD);
	// 指向位图数据的指针
	lpData = pbDib + infoHeader->biSize + nTotalColors * sizeof(RGBQUAD);
	// 如果没有颜色索引表，则该值为 nullptr
	if ((lpRGBQuad == (RGBQUAD*)lpData) || (nTotalColors == 0))
		lpRGBQuad = nullptr;

	lHeight = infoHeader->biHeight;
	lWidth = infoHeader->biWidth;
	lWidthBytes = WIDTHBYTES(infoHeader->biWidth * infoHeader->biBitCount);

	// 若不是 24 位色深图，返回空指针
	if (lpRGBQuad != nullptr)
	{
		infoHeader = nullptr;
		lpRGBQuad = nullptr;
		lpData = nullptr;
		return nullptr;
	}

	lpTwoValueData = new BYTE[lHeight * lWidth];

	// 24 位色深数据 转为 8 位色深数据，且将灰度图二值化
	for (int i = 0; i < lHeight; i++)
	{
		for (int j = 0; j < lWidth; j++)
		{
			pixelValue = (int)(0.114 * (float)lpData[i * lWidthBytes + 3 * j] + 0.587 * (float)lpData[i * lWidthBytes + 3 * j + 1] + 0.299 * (float)lpData[i * lWidthBytes + 3 * j + 2]);
			// 将灰度值转化为二值，这里选取的阈值为 160-190 都可以
			if (pixelValue >= 160)
			{
				lpTwoValueData[i * lWidth + j] = 0;		// 白色
			}
			else
			{
				lpTwoValueData[i * lWidth + j] = 1;		// 黑色
			}
		}
	}

	infoHeader = nullptr;
	lpRGBQuad = nullptr;
	lpData = nullptr;

	return lpTwoValueData;
}

/**
* @brief 批量目标检测函数.
* 
* 检测原理: 通过行扫描作为切线寻找圆.
* 也可以使用 opencv 实现目标检测，detect-from-array branch 未使用，计划在 detect-by-opencv branch 中实现.
* 本检测方法以方块作为圆检测边缘，故若多个圆的方块边缘有重叠时无法检测，有一定局限性.
*/
void CCircTgtVSDoc::BatchDetectArray(CString szPath, CString* szFileList, int nFileSum)
{
	CString szFileName;
	CFile file;
	LPBITMAPFILEHEADER fileHeader = nullptr;
	BYTE* pbDib = nullptr;
	LPBITMAPINFOHEADER infoHeader = nullptr;
	long lResult, lSize, lHeight, lWidth;
	UINT nTotalColors = 0;
	DWORD dwColorTableSize = 0;
	BOOL isSuccess = FALSE;
	int pixelValue = 0;
	BYTE* lpBiData = nullptr;		// 图像二值化数据
	int startWidth, endWidth, startHeight, endHeight;
	BOOL bIndexPass = FALSE;		// 判断 index 是否要跳过
	BOOL inCirc = FALSE;			// 判断索引的 pixel 是否仍在圆内
	long lCenterH, lCenterW, lRadius;

	// 根据文件数量为 m_pCircle 分配空间
	if (m_pCircle != nullptr)
	{
		delete[] m_pCircle;
		m_pCircle = nullptr;
	}
	m_pCircle = new CCircle4Array[nFileSum];

	// 批量处理
	for (int i = 0; i < nFileSum; i++)
	{
		szFileName = szPath + "\\" + szFileList[i];
		isSuccess = file.Open(szFileName, CFile::modeRead);		// 读模式打开文件
		fileHeader = new BITMAPFILEHEADER;
		lResult = file.Read(fileHeader, sizeof(BITMAPFILEHEADER));
		lSize = (long)file.GetLength() - sizeof(BITMAPFILEHEADER);
		pbDib = new BYTE[lSize];
		lResult = file.Read(pbDib, lSize);
		file.Close();

		// 定义 m_pCircle[i] 对应的文件
		m_pCircle[i].m_sz4WhichFile = szFileName;

		// 获取图像二值化数据，注意 BMP 中行数据为倒序
		// 二值化数据中 0: 白色，1: 黑色
		lpBiData = RGB2TwoValue(pbDib);
		infoHeader = (LPBITMAPINFOHEADER)pbDib;
		lHeight = infoHeader->biHeight;
		lWidth = infoHeader->biWidth;

		// 目标检测
		startWidth = 0;
		endWidth = 0;
		startHeight = 0;
		endHeight = 0;

		for (int j = 0; j < lHeight; j++)
		{
			for (int k = 0; k < lWidth; k++)
			{
				// 检查 index 是否在已检测到的圆的范围内
				bIndexPass = FALSE;
				for (int l = 0; l < m_pCircle[i].m_lCircSum; l++)
				{
					if (j >= m_pCircle[i].m_pCircParam[l].m_nTop && j <= m_pCircle[i].m_pCircParam[l].m_nBottom && \
						k >= m_pCircle[i].m_pCircParam[l].m_nLeft && k <= m_pCircle[i].m_pCircParam[l].m_nRight)
					{
						bIndexPass = TRUE;
						break;
					}
				}
				// index 在已检测到的圆的范围内时，跳过
				if (bIndexPass)
					continue;

				// 检测到圆，且此圆的加入不会超过圆的存储容量
				if ((lpBiData[j * lWidth + k] == 1) && (m_pCircle[i].m_lCircSum < m_pCircle[i].m_lListSize))
				{
					startHeight = j;
					// 位图中显然"切点"不一定为 1 个 pixel，找到"切点"的始末 pixel 坐标
					startWidth = k;
					endWidth = k;
					while ((lpBiData[j * lWidth + endWidth] == 1) && (endWidth < lWidth))
						endWidth++;
					endWidth--;
					// 纵向移动行切线，寻找另一个切点
					endHeight = j;
					while (lpBiData[endHeight * lWidth + startWidth] == 1 || lpBiData[endHeight * lWidth + endWidth] == 1)
						endHeight++;
					// 若该行在 [startWidth, endWidth] 区间内含有该圆的 pixel，endHeight 即为所求
					for (int l = startWidth; l <= endWidth; l++)
					{
						if (lpBiData[endHeight * lWidth + l])
							inCirc = TRUE;
					}
					// 若该行在 [startWidth, endWidth] 区间内不含有该圆的 pixel，修正 endHeight
					if (!inCirc)
						endHeight--;

					// 获取圆的数据
					lCenterH = (startHeight + endHeight) / 2;
					lCenterW = (startWidth + endWidth) / 2;
					lRadius = (endHeight - startHeight + 1) / 2;
					// 修正 startWidth 为圆的左边缘
					startWidth = lCenterW - lRadius;
					while (lpBiData[lCenterH * lWidth + startWidth])
						startWidth--;
					while (!lpBiData[lCenterH * lWidth + startWidth])
						startWidth++;
					// 修正 endWidth 为圆的右边缘
					endWidth = lCenterW + lRadius;
					while (lpBiData[lCenterH * lWidth + endWidth])
						endWidth++;
					while (!lpBiData[lCenterH * lWidth + endWidth])
						endWidth--;
					// 存入 m_pCircle[i].m_pCircParam[m_pCircle[i].m_lCircSum] 处的数据
					// 注意，Bitmap 中图片数据从左下角开始，此数据还须做变换
					// 此处为方便检查 index，暂不变换，在检测完全部圆后再统一变换
					m_pCircle[i].m_pCircParam[m_pCircle[i].m_lCircSum].m_lCenterH = lCenterH;
					m_pCircle[i].m_pCircParam[m_pCircle[i].m_lCircSum].m_lCenterW = lCenterW;
					m_pCircle[i].m_pCircParam[m_pCircle[i].m_lCircSum].m_lRadius = lRadius;
					m_pCircle[i].m_pCircParam[m_pCircle[i].m_lCircSum].m_nTop = startHeight;
					m_pCircle[i].m_pCircParam[m_pCircle[i].m_lCircSum].m_nBottom = endHeight;
					m_pCircle[i].m_pCircParam[m_pCircle[i].m_lCircSum].m_nLeft = startWidth;
					m_pCircle[i].m_pCircParam[m_pCircle[i].m_lCircSum].m_nRight = endWidth;

					m_pCircle[i].m_lCircSum++;
				}
			}
		}

		// 数据变换
		// startHeight 为 Bitmap 数据中上切线所在行，对应 m_nBottom
		// endHeight 为 Bitmap 数据中下切线所在行，对应 m_nTop
		for (int j = 0; j < m_pCircle[i].m_lCircSum; j++)
		{
			lCenterH = m_pCircle[i].m_pCircParam[j].m_lCenterH;
			startHeight = m_pCircle[i].m_pCircParam[j].m_nTop;
			endHeight = m_pCircle[i].m_pCircParam[j].m_nBottom;
			m_pCircle[i].m_pCircParam[j].m_lCenterH = lHeight - lCenterH - 1;
			m_pCircle[i].m_pCircParam[j].m_nTop = lHeight - endHeight - 1;
			m_pCircle[i].m_pCircParam[j].m_nBottom = lHeight - startHeight - 1;
		}

		// 释放内存
		delete fileHeader;
		delete[] pbDib;
	}
}
