#pragma once
#include "atlimage.h"

#include <complex>
using namespace std;

#define  MAX_SIZE 1000
const double box[9] = {1,1,1,1,1,1,1,1,1};
const double gx[9] = { -1,-2,-1,0,0,0,1,2,1 };
const double gy[9] = { -1,0,1,-2,0,2,-1,0,1 };
const double laplacian[9] = { 1, 1, 1, 1, -8, 1, 1, 1, 1 };  //diognal form
#define Pi 3.1415926535897932354626
#define Ei 2.71828

class CDib :
	public CImage
{
public:
	CDib(void);
	CDib(CDib &Dib);
	~CDib(void);
	RGBQUAD *origin_palette = NULL;
public:
	enum chan_color {chan_red=1,chan_green,chan_blue,chan_black};
	enum HSIChannel{HUE=0,SATURATION,INTENSITY};

public:
	void LoadFile(LPCTSTR lpszPathName);
	void LoadFromTxtFile();
public:
	void Invert();
	void TurnRed();
	void TurnBlue();
	void TurnGreen();
	void Window();
	void Histogramequlization();
	void Spacial_Filter(CDib *, int type);
	void littlewhite(int block_width, int block_height);
	void intensity();
	void sat();
	void hue();
	void colorcircle();
	void filter_freq_domain_unpadded(int D0, int type);
	void filter_freq_domain_padded(int D0, int type);
	void spectrum_analyze();
	void fbp();
	void Zoom(CDib *Dib, double factor);
	void binarization();
	void inverse_colour();
	void errosion();
	void Dilation();
	void holefilling();
	void bound();
	void enhance(CDib*buffer);
	void wavlet_2D(int m_nWidth, int m_nHeight,int type);
	void wavlet_1Dharr(double *linedata,int m_nWidth);
	void wavlet_1Ddb4(double *linedata, int m_nWidth);
	template <typename T> void transpose(T *array, int width, int height);
	void GuassianFilter(CDib *buffer, double sigma);
	void non_maxsuppresion(CDib*buffer);
	void bithreshhoding();
public:	
	long* GrayValueCount();

public:
	BOOL FFT(unsigned char*, long, long);
	void FFT_2D(complex<double> * , int, int , complex<double> * );
	void FFT_1D(complex<double> * , complex<double> * , int);
	void IFFT_2D(complex<double> * , complex<double> * , int, int);
	void IFFT_1D(complex<double> *, complex<double> *, int);

private:
	long m_nWidth;
	long m_nHeight;
	int m_nWidthBytes;
	int m_nBitCount;
	unsigned char *m_pDibBits;
	
private:
	long *m_pGrayValueCount;
	double *m_pRawdata;
};

