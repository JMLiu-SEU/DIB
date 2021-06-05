
// dipDoc.cpp: CdipDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "dip.h"
#endif

#include "dipDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "CDigHistogram.h"
#include "Dib.h"
// CdipDoc

IMPLEMENT_DYNCREATE(CdipDoc, CDocument)

BEGIN_MESSAGE_MAP(CdipDoc, CDocument)
	ON_COMMAND(ID_32771, &CdipDoc::On_invert)
	ON_COMMAND(ID_32773, &CdipDoc::OnTurnRed)
	ON_COMMAND(ID_32775, &CdipDoc::OnWindow)
	ON_COMMAND(ID_32772, &CdipDoc::OnTurnBlue)
	ON_COMMAND(ID_32774, &CdipDoc::OnTurnGreen)
	ON_COMMAND(ID_32776, &CdipDoc::OnHistogram)
	ON_COMMAND(ID_32777, &CdipDoc::OnHistogramEqualization)
	ON_COMMAND(ID_32778, &CdipDoc::Onboxfilter)
	ON_COMMAND(ID_32779, &CdipDoc::OnRestoration)
	ON_COMMAND(ID_32780, &CdipDoc::On_littlebox)
	ON_COMMAND(ID_FREQUENCYDOMAIN_LOWPASSIDEAL, &CdipDoc::On_low_pass_ideal)
	ON_COMMAND(ID_FREQUENCYDOMAIN_HIGHPASSIDEAL, &CdipDoc::OnFrequencydomainHighpassideal)
	ON_COMMAND(ID_FREQUENCYDOMAIN_LOWPASSGAUSSIAN, &CdipDoc::OnFrequencydomainLowpassgaussian)
	ON_COMMAND(ID_FREQUENCYDOMAIN_HIGHPASSGAUSSIAN, &CdipDoc::OnFrequencydomainHighpassgaussian)
	ON_COMMAND(ID_FREQUENCYDOMAIN_LOWPASSBUTTERWORTH, &CdipDoc::OnFrequencydomainLowpassbutterworth)
	ON_COMMAND(ID_FREQUENCYDOMAIN_HIGHPASSBUTTERWORTH, &CdipDoc::OnFrequencydomainHighpassbutterworth)
	ON_COMMAND(ID_FREQUENCYDOMAIN_LOWPASSIDEALPADDING, &CdipDoc::OnFrequencydomainLowpassidealpadding)
	ON_COMMAND(ID_FREQUENCYDOMAIN_HIGHPASSIDEALPADDING, &CdipDoc::OnFrequencydomainHighpassidealpadding)
	ON_COMMAND(ID_FREQUENCYDOMAIN_LOWPASSGAUSSIANPADDING, &CdipDoc::OnFrequencydomainLowpassgaussianpadding)
	ON_COMMAND(ID_FREQUENCYDOMAIN_HIGHPASSGAUSSIANPADDING, &CdipDoc::OnFrequencydomainHighpassgaussianpadding)
	ON_COMMAND(ID_FREQUENCYDOMAIN_LOWPASSBUTTERWORTHPADDING, &CdipDoc::OnFrequencydomainLowpassbutterworthpadding)
	ON_COMMAND(ID_FREQUENCYDOMAIN_HIGHPASSBUTTERWORTHPADDING, &CdipDoc::OnFrequencydomainHighpassbutterworthpadding)
	ON_COMMAND(ID_FREQUENCYDOMAIN_SPECTRUMANALYSE, &CdipDoc::OnFrequencydomainSpectrumanalyse)
	ON_COMMAND(ID_32794, &CdipDoc::On32794)
	ON_COMMAND(ID_32795, &CdipDoc::On32795)
	ON_COMMAND(ID_32796, &CdipDoc::On32796)
	ON_COMMAND(ID_32797, &CdipDoc::On32797)
	ON_COMMAND(ID_32798, &CdipDoc::On32798)
	ON_COMMAND(ID_SPACIALDOMAIN_LAPLCIAN, &CdipDoc::OnSpacialdomainLaplcian)
	ON_COMMAND(ID_32800, &CdipDoc::On32800)
	ON_COMMAND(ID_32801, &CdipDoc::On32801)
	ON_COMMAND(ID_32802, &CdipDoc::On32802)
	ON_COMMAND(ID_32803, &CdipDoc::On32803)

	ON_COMMAND(ID_32805, &CdipDoc::On32805)
	ON_COMMAND(ID_32806, &CdipDoc::On32806)
	ON_COMMAND(ID_32807, &CdipDoc::Onzoom)
	ON_COMMAND(ID_32808, &CdipDoc::On32808)
	ON_COMMAND(ID_32809, &CdipDoc::Onbinary)
	ON_COMMAND(ID_32810, &CdipDoc::Oninversecolor)
	ON_COMMAND(ID_32811, &CdipDoc::Onerrod)
	ON_COMMAND(ID_Menu, &CdipDoc::Ondilation)
	ON_COMMAND(ID_32813, &CdipDoc::Onharr)
	ON_COMMAND(ID_32814, &CdipDoc::Onharr2)
	ON_COMMAND(ID_32815, &CdipDoc::On点便)
	ON_COMMAND(ID_32816, &CdipDoc::OnDB4_2)
	ON_COMMAND(ID_32817, &CdipDoc::Onhole)
	ON_COMMAND(ID_32818, &CdipDoc::Onboundextract)
	ON_COMMAND(ID_32819, &CdipDoc::On_enhance)
	ON_COMMAND(ID_CANNY_32820, &CdipDoc::OnCannygauss)
	ON_COMMAND(ID_CANNY_32821, &CdipDoc::OnCannysuppres)
	ON_COMMAND(ID_CANNY_32822, &CdipDoc::OnCannythresh)
END_MESSAGE_MAP()


// CdipDoc 构造/析构

CdipDoc::CdipDoc() noexcept
{
	// TODO: 在此添加一次性构造代码
	m_pDib = NULL;
	m_pDib_Buffer = NULL;
}

CdipDoc::~CdipDoc()
{
	if (m_pDib != NULL)
	{
		delete m_pDib;
		m_pDib = NULL;
	}
	if (m_pDib_Buffer != NULL)
	{
		delete m_pDib_Buffer;
		m_pDib_Buffer = NULL;
	}
}

BOOL CdipDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CdipDoc 序列化

void CdipDoc::Serialize(CArchive& ar)
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
void CdipDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CdipDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CdipDoc::SetSearchContent(const CString& value)
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

// CdipDoc 诊断

#ifdef _DEBUG
void CdipDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CdipDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CdipDoc 命令


BOOL CdipDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  在此添加您专用的创建代码
	if (m_pDib != NULL) // before open new file, delete the last file pointer m_pDib
	{
		delete m_pDib;
		m_pDib = NULL;
	}
	if (m_pDib_Buffer != NULL) // before open new file, delete the last file pointer m_pDib
	{
		delete m_pDib_Buffer;
		m_pDib_Buffer = NULL;
	}
	m_pDib = new CDib;  //my pointer Dib
	m_pDib->LoadFile(lpszPathName);
	m_pDib_Buffer = new CDib(*m_pDib);//pointer :CDib( CDib &Dib )
	UpdateAllViews(NULL);
	return TRUE;
}


void CdipDoc::On_invert()
{
	if (m_pDib != NULL)
	{
		m_pDib->Invert();
		UpdateAllViews(NULL);
	}
	// TODO: 在此添加命令处理程序代码
}





void CdipDoc::OnTurnRed()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->TurnRed();
		UpdateAllViews(NULL);
	}
}


void CdipDoc::OnWindow()
{
	if (m_pDib != NULL)
	{
		m_pDib->Window();
		UpdateAllViews(NULL);
	}
}


void CdipDoc::OnTurnBlue()
{
	if (m_pDib != NULL)
	{
		m_pDib->TurnBlue();
		UpdateAllViews(NULL);
	}
}


void CdipDoc::OnTurnGreen()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->TurnGreen();
		UpdateAllViews(NULL);
	}
}


void CdipDoc::OnHistogram()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		CDigHistogram dlg(m_pDib);
		dlg.DoModal();
	}
}


void CdipDoc::OnHistogramEqualization()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->Histogramequlization();
		UpdateAllViews(NULL);
	}
}


void CdipDoc::Onboxfilter()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		CDib *filter_temp =new  CDib(*m_pDib);
		m_pDib->Spacial_Filter(filter_temp, 1);
		UpdateAllViews(NULL);

		delete filter_temp;
		filter_temp = NULL;
	}
	
}


void CdipDoc::OnRestoration()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		delete m_pDib;
		m_pDib = NULL;
	}
	if (m_pDib_Buffer != NULL)
	{
		m_pDib = new CDib(*m_pDib_Buffer);

	}
	UpdateAllViews(NULL);
	
}


void CdipDoc::On_littlebox()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		delete m_pDib;
		m_pDib = NULL;
		delete m_pDib_Buffer;
		m_pDib_Buffer = NULL;
	}
	m_pDib = new CDib();
	m_pDib->littlewhite(60,60);
	m_pDib_Buffer = new CDib(*m_pDib);   // 小白块也需要图像恢复
	UpdateAllViews(NULL);
}


void CdipDoc::On_low_pass_ideal()
{
	// TODO: 在此添加命令处理程序代码
		// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->filter_freq_domain_unpadded(50,1);
		UpdateAllViews(NULL);
	}
}



void CdipDoc::OnFrequencydomainHighpassideal()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->filter_freq_domain_unpadded(20, 2);
		UpdateAllViews(NULL);
	}
}



void CdipDoc::OnFrequencydomainLowpassgaussian()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->filter_freq_domain_unpadded(50, 3);
		UpdateAllViews(NULL);
	}
}


void CdipDoc::OnFrequencydomainHighpassgaussian()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->filter_freq_domain_unpadded(20, 4);
		UpdateAllViews(NULL);
	}
}


void CdipDoc::OnFrequencydomainLowpassbutterworth()
{
	if (m_pDib != NULL)
	{
		m_pDib->filter_freq_domain_unpadded(50, 5);
		UpdateAllViews(NULL);
	}
}


	void CdipDoc::OnFrequencydomainHighpassbutterworth()
	{
		if (m_pDib != NULL)
		{
			m_pDib->filter_freq_domain_unpadded(20, 6);
			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::OnFrequencydomainLowpassidealpadding()
	{
		if (m_pDib != NULL)
		{
			m_pDib->filter_freq_domain_padded(50, 1);
			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::OnFrequencydomainHighpassidealpadding()
	{
		if (m_pDib != NULL)
		{
			m_pDib->filter_freq_domain_padded(20, 2);
			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::OnFrequencydomainLowpassgaussianpadding()
	{
		if (m_pDib != NULL)
		{
			m_pDib->filter_freq_domain_padded(50, 3);
			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::OnFrequencydomainHighpassgaussianpadding()
	{
		if (m_pDib != NULL)
		{
			m_pDib->filter_freq_domain_padded(20, 4);
			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::OnFrequencydomainLowpassbutterworthpadding()
	{
		if (m_pDib != NULL)
		{
			m_pDib->filter_freq_domain_padded(50, 5);
			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::OnFrequencydomainHighpassbutterworthpadding()
	{
		if (m_pDib != NULL)
		{
			m_pDib->filter_freq_domain_padded(20, 6);
			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::OnFrequencydomainSpectrumanalyse()
	{
		// TODO: 在此添加命令处理程序代码
		if (m_pDib != NULL)
		{
			m_pDib->spectrum_analyze();
			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::On32794()
	{
		// TODO: 在此添加命令处理程序代码
		if (m_pDib != NULL)
		{
			CDib *filter_temp = new  CDib(*m_pDib);
			m_pDib->Spacial_Filter(filter_temp, 2);
			UpdateAllViews(NULL);

			delete filter_temp;
			filter_temp = NULL;
		}
	}


	void CdipDoc::On32795()
	{
		if (m_pDib != NULL)
		{
			m_pDib->filter_freq_domain_unpadded(20, 7);
			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::On32796()
	{
		// TODO: 在此添加命令处理程序代码
		if (m_pDib != NULL)
		{
			m_pDib->filter_freq_domain_unpadded(150, 8);
			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::On32797()
	{
		// TODO: 在此添加命令处理程序代码
		if (m_pDib != NULL)
		{
			m_pDib->filter_freq_domain_unpadded(150, 9);
			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::On32798()  //sobel
	{
		if (m_pDib != NULL)
		{
			CDib *filter_temp = new  CDib(*m_pDib);
			m_pDib->Spacial_Filter(filter_temp, 3);
			UpdateAllViews(NULL);

			delete filter_temp;
			filter_temp = NULL;
		}
	}


	void CdipDoc::OnSpacialdomainLaplcian()
	{
		if (m_pDib != NULL)
		{
			CDib *filter_temp = new  CDib(*m_pDib);
			m_pDib->Spacial_Filter(filter_temp, 4);
			UpdateAllViews(NULL);

			delete filter_temp;
			filter_temp = NULL;
		}
	}


	void CdipDoc::On32800()
	{
		if (m_pDib != NULL) // before open new file, delete the last file pointer m_pDib
		{
			delete m_pDib;
			m_pDib = NULL;
		}
		if (m_pDib_Buffer != NULL) // before open new file, delete the last file pointer m_pDib
		{
			delete m_pDib_Buffer;
			m_pDib_Buffer = NULL;
		}
		m_pDib = new CDib;  //my pointer Dib
		m_pDib->LoadFromTxtFile();
		m_pDib_Buffer = new CDib(*m_pDib);//para: pointer :CDib( CDib &Dib )
		UpdateAllViews(NULL);
	}


	void CdipDoc::On32801()
	{
		if (m_pDib != NULL)
		{

			m_pDib->fbp();
			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::On32802()
	{
		if (m_pDib != NULL)
		{
			delete m_pDib;
			m_pDib = NULL;
			delete m_pDib_Buffer;
			m_pDib_Buffer = NULL;
		}
		m_pDib = new CDib();
		m_pDib->colorcircle();
		m_pDib_Buffer = new CDib(*m_pDib);   // 小白块也需要图像恢复
		UpdateAllViews(NULL);
	}


	void CdipDoc::On32803()
	{
		if (m_pDib != NULL)
		{

			m_pDib->intensity();
			UpdateAllViews(NULL);
		}
	}




	void CdipDoc::On32805()
	{
		if (m_pDib != NULL)
		{

			m_pDib->sat();
			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::On32806()
	{
		if (m_pDib != NULL)
		{

			m_pDib->hue();
			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::Onzoom()
	{
		if (m_pDib != NULL)
		{
			

			m_pDib->Zoom(m_pDib_Buffer, 1.1);

			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::On32808()
	{
		if (m_pDib != NULL)
		{


			m_pDib->Zoom(m_pDib_Buffer, 1/1.1);

			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::Onbinary()
	{
		if (m_pDib != NULL)
		{


			m_pDib->binarization();

			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::Oninversecolor()
	{
		if (m_pDib != NULL)
		{


			m_pDib->inverse_colour();

			UpdateAllViews(NULL);
		}

	}


	void CdipDoc::Onerrod()
	{
		if (m_pDib != NULL)
		{


			m_pDib->errosion();

			UpdateAllViews(NULL);
		}

	}


	void CdipDoc::Ondilation()
	{
		if (m_pDib != NULL)
		{


			m_pDib->Dilation();

			UpdateAllViews(NULL);
		}

	}


	void CdipDoc::Onharr()
	{
		if (m_pDib != NULL)
		{


			m_pDib->wavlet_2D(512,512,1);

			UpdateAllViews(NULL);
		}
		
	}


	void CdipDoc::Onharr2()
	{
		if (m_pDib != NULL)
		{


			m_pDib->wavlet_2D(256, 256,1);

			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::On点便()
	{
		if (m_pDib != NULL)
		{


			m_pDib->wavlet_2D(512, 512,2);

			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::OnDB4_2()
	{
		if (m_pDib != NULL)
		{


			m_pDib->wavlet_2D(256, 256, 2);

			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::Onhole()
	{
		if (m_pDib != NULL)
		{


			m_pDib->holefilling();

			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::Onboundextract()
	{
		if (m_pDib != NULL)
		{


			m_pDib->bound();

			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::On_enhance()
	{
		if (m_pDib != NULL)
		{


			m_pDib->enhance(m_pDib_Buffer);

			UpdateAllViews(NULL);
		}
	}


	void CdipDoc::OnCannygauss()
	{
		if (m_pDib != NULL)
		{
			CDib *filter_temp = new  CDib(*m_pDib);
			m_pDib->GuassianFilter(filter_temp,3);
			UpdateAllViews(NULL);

			delete filter_temp;
			filter_temp = NULL;
		}
	}


	void CdipDoc::OnCannysuppres()
	{
		if (m_pDib != NULL)
		{
			CDib *filter_temp = new  CDib(*m_pDib);
			m_pDib->non_maxsuppresion(filter_temp);
			UpdateAllViews(NULL);

			delete filter_temp;
			filter_temp = NULL;
		}
	}


	void CdipDoc::OnCannythresh()
	{
		if (m_pDib != NULL)
		{
			
			m_pDib->bithreshhoding();
			UpdateAllViews(NULL);
		}
	}
