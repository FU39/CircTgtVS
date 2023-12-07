
// CircTgtVSDoc.h: CCircTgtVSDoc 类的接口
//


#pragma once

#include "Circle4Array.h"


class CCircTgtVSDoc : public CDocument
{
protected: // 仅从序列化创建
	CCircTgtVSDoc() noexcept;
	DECLARE_DYNCREATE(CCircTgtVSDoc)

// 特性
public:

// 操作
public:
	HGLOBAL m_hDIB;
	LPBITMAPINFO m_lpInfo;					// 图像 InfoHeader+index 内存起始地址
	LPBITMAPINFOHEADER m_lpInfoHeader;		// 图像 InfohHeader 内存起始地址
	RGBQUAD* m_lpRGBQuad;					// 图像如果使用颜色索引，该指针指向内存中颜色索引部分的起始地址
	UINT m_nTotalColors;					// 图像的色深度,单位 bits
	BYTE* m_lpData;							// 图像 data 内存起始地址

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CCircTgtVSDoc();
	void OpenFile(CString szFileName);
	void Image2BlackWhite();
	BYTE* RGB2TwoValue(BYTE* pbDib);
	CCircle4Array* BatchDetectArray(CString szPath, CString* szFileList, int nFileSum);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
