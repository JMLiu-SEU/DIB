
// dipDoc.h: CdipDoc 类的接口
//


#pragma once
class CDib;

class CdipDoc : public CDocument
{
protected: // 仅从序列化创建
	CdipDoc() noexcept;
	DECLARE_DYNCREATE(CdipDoc)

// 特性
public:

// 操作
public:

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
	virtual ~CdipDoc();
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

public:
	CDib *m_pDib;
	CDib *m_pDib_Buffer;
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg void On_invert();
	afx_msg void OnTurnRed();
	afx_msg void OnWindow();
	afx_msg void OnTurnBlue();
	afx_msg void OnTurnGreen();
	afx_msg void OnHistogram();
	afx_msg void OnHistogramEqualization();
	afx_msg void Onboxfilter();
	afx_msg void OnRestoration();
	afx_msg void On_littlebox();
	afx_msg void On_low_pass_ideal();
	afx_msg void OnFrequencydomainHighpassideal();
	afx_msg void OnFrequencydomainLowpassgaussian();
	afx_msg void OnFrequencydomainHighpassgaussian();
	afx_msg void OnFrequencydomainLowpassbutterworth();
	afx_msg void OnFrequencydomainHighpassbutterworth();
	afx_msg void OnFrequencydomainLowpassidealpadding();
	afx_msg void OnFrequencydomainHighpassidealpadding();
	afx_msg void OnFrequencydomainLowpassgaussianpadding();
	afx_msg void OnFrequencydomainHighpassgaussianpadding();
	afx_msg void OnFrequencydomainLowpassbutterworthpadding();
	afx_msg void OnFrequencydomainHighpassbutterworthpadding();
	afx_msg void OnFrequencydomainSpectrumanalyse();
	afx_msg void On32794();
	afx_msg void On32795();
	afx_msg void On32796();
	afx_msg void On32797();
	afx_msg void On32798();
	afx_msg void OnSpacialdomainLaplcian();
	afx_msg void On32800();
	afx_msg void On32801();
	afx_msg void On32802();
	afx_msg void On32803();

	afx_msg void On32805();
	afx_msg void On32806();
	afx_msg void Onzoom();
	afx_msg void On32808();
	afx_msg void Onbinary();
	afx_msg void Oninversecolor();
	afx_msg void Onerrod();
	afx_msg void Ondilation();
	afx_msg void Onharr();
	afx_msg void Onharr2();
	afx_msg void On点便();
	afx_msg void OnDB4_2();
	afx_msg void Onhole();
	afx_msg void Onboundextract();
	afx_msg void On_enhance();
	afx_msg void OnCannygauss();
	afx_msg void OnCannysuppres();
	afx_msg void OnCannythresh();
};



