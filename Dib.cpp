#include "pch.h"
#include "Dib.h"
#include <iostream>
#include<fstream>
#include<cstdlib>
CDib::CDib(void):m_pDibBits(NULL),m_pGrayValueCount(NULL),m_pRawdata(NULL)
{
}


CDib::CDib(CDib &Dib) : m_pDibBits(NULL), m_pGrayValueCount(NULL), m_pRawdata(NULL)
{
	if(&Dib == NULL)
	{
		return;
	}
	if(!IsNull())
	{
		Destroy();
	}
	Create(Dib.GetWidth(),Dib.GetHeight(),Dib.GetBPP(),0);
	m_nWidth = Dib.m_nWidth;
	m_nHeight = Dib.m_nHeight;
	if(IsIndexed()) //This method returns TRUE only if the bitmap is 8-bit (256 colors) or less.
	{
		int nColors=Dib.GetMaxColorTableEntries();
		if(nColors > 0)
		{
			RGBQUAD *pal = new RGBQUAD[nColors];
			Dib.GetColorTable(0,nColors,pal);
			SetColorTable(0,nColors,pal);
			delete[] pal;
		} 
	}
	m_nWidthBytes =abs(GetPitch()) ;
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits()+(m_nHeight-1)*GetPitch();
	memcpy(m_pDibBits,Dib.m_pDibBits,m_nHeight*m_nWidthBytes);
}

CDib::~CDib(void)
{
	m_pDibBits = NULL; // ָ��
	if (m_pGrayValueCount != NULL)  //��̬�����ڴ�
	{
		delete []m_pGrayValueCount;
		m_pGrayValueCount = NULL;
	}

	if (origin_palette)
		delete origin_palette;
	origin_palette = NULL;
	if (m_pRawdata)
		delete m_pRawdata;
	m_pRawdata = NULL;
}
template <typename T> void CDib::transpose(T *array, int width, int height)
{
	T *temp = new T[width * height];
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			temp[height * x + y] = *(array + width * y + x);
		}
	}
	memcpy(array, temp, width * height * sizeof(T));
	delete temp;
	temp = NULL;
}
void CDib::LoadFile( LPCTSTR lpszPathName )
{
	Load(lpszPathName);
	m_nWidth = GetWidth();//Retrieves the width, in pixels, of an image.
	m_nHeight = GetHeight();
	m_nWidthBytes =abs(GetPitch()) ;//BPP:The number of bits per pixel bottom up DIB: pitch negative else pitch positive
	m_nBitCount = GetBPP();//The pitch is the distance, in bytes, between two memory addresses that represent the beginning of one bitmap line and the beginning of the next bitmap line.
	m_pDibBits = (unsigned char*)GetBits()+(m_nHeight-1)*GetPitch(); //GetBits:Retrieves a pointer to the actual bit values of a given pixel in a bitmap
	//ע����һ��getpitch�Ľ���Ǹ���
	int color_channel = GetMaxColorTableEntries();//The RGBQUAD structure describes a color consisting of relative intensities of red, green, and blue
	if (color_channel)  //color_channel ����n_colors
	{
		origin_palette = new RGBQUAD[color_channel];
		if (origin_palette)
			GetColorTable(0, color_channel, origin_palette);
	}
}


void CDib::LoadFromTxtFile()
{
	streamsize strcount = 16;
	char buffer[16];
	string m;
	double ndata, maxdata = 0;
	ifstream out("C:\\Users\\J.M.Liu\\Desktop\\Radon.txt");
	//ifstream out("C:\\Users\\Shouhua Luo\\Desktop\\�μ�\\����ͼ����\\20-21-03ѧ��ҽѧͼ����\\����\\shepp-logan.txt");
	if (out.is_open())
	{
		////��ȡ���ݸ߶ȺͿ��
		//out.seekg(0, ios::beg);
		getline(out, m);
		int linewords = m.length();
		out.seekg(0, ios::end);
		int totalwords = out.tellg();
		int height = totalwords / linewords;
		long width = m.length() / strcount;

		////����һ���ڴ��Ŷ���ĸ�����
		m_pRawdata = new double[width*height];
		memset(m_pRawdata, 0.0, width*height);
		int j = 0;
		out.seekg(0, ios::beg);
		while (getline(out, m))
		{
			for (int i = 0; i < width; i++)
			{
				m.copy(buffer, strcount, i * strcount);
				ndata = atof(buffer);
				*(m_pRawdata + j * width + i) = ndata;
				if (maxdata < ndata)
				{
					maxdata = ndata;
				}
			}
			j++;
		}
		out.close();

		////������ת������ʾ����
		long BitCount = 8;
		BOOL Issuccess = Create(width, height, BitCount, 0);
		if (!Issuccess)
		{
			return;
		}
		if (IsIndexed())
		{
			int nColors = GetMaxColorTableEntries();
			if (nColors > 0)
			{
				RGBQUAD *pal = new RGBQUAD[nColors];
				for (int i = 0; i < nColors; i++)
				{
					pal[i].rgbBlue = i;
					pal[i].rgbGreen = i;
					pal[i].rgbRed = i;
				}
				SetColorTable(0, nColors, pal);
				delete[] pal;
			}
		}
		m_nWidth = GetWidth();
		m_nHeight = GetHeight();
		m_nWidthBytes = abs(GetPitch());
		m_nBitCount = GetBPP();
		m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1)*GetPitch();
		m_pGrayValueCount = NULL;

		for (int j = 0; j < m_nHeight; j++)
		{
			for (int i = 0; i < m_nWidth; i++)
			{
				ndata = (*(m_pRawdata + j * width + i) / maxdata)*255.0;
				*(m_pDibBits + j * m_nWidthBytes + i) = (BYTE)ndata;
			}
		}
	}
	else
	{
		cout << "Error: opening file." << endl;
	}
	return;
}
//С�׿� �� sinc һ�����ӦһƬ��

void CDib::Invert()
{
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)  //����lena m_nWidthBytes ����m_nHeight 512*512,û�л�����
		{
			*(m_pDibBits + i*m_nWidthBytes + j) = (unsigned char)(255 - *(m_pDibBits + i*m_nWidthBytes + j));//ָ���һ���ӵ���sizeof(ָ����ָ����)
		}
	}
}
//m_pDibBitsָ���������������ַ�ĵ�һ�е�һ�У�ʵ�ʶ�Ӧͼ������һ�е�һ�У����һ�д�����֮�󣬻ص������ڶ��дӵ�һ�п�ʼ����

template <typename T> void transpose(T *array, int width, int height)
{
	T *temp = new T[width * height];
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			temp[height * x + y] = *(array + width * y + x);
		}
	}
	memcpy(array, temp, width * height * sizeof(T));
	delete temp;
	temp = NULL;
}



void CDib::Window()
{
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			if (*(m_pDibBits + i * m_nWidthBytes + j) < 100)
				*(m_pDibBits + i * m_nWidthBytes + j) = 0;
			else if (*(m_pDibBits + i * m_nWidthBytes + j) > 200)
				*(m_pDibBits + i * m_nWidthBytes + j) = 255;
			else
				*(m_pDibBits + i * m_nWidthBytes + j) = (*(m_pDibBits + i * m_nWidthBytes + j) - 100)*2.55;
		}
	}
}




void CDib::TurnRed()
{
	int ncolors = GetMaxColorTableEntries();
	if (ncolors > 0)
	{
		RGBQUAD *temp = new RGBQUAD[ncolors];
		for (int i = 0; i < ncolors; i++)
		{
			temp[i].rgbRed = i;
			temp[i].rgbBlue = BYTE(0);
			temp[i].rgbGreen = BYTE(0);
			temp[i].rgbReserved = BYTE(0);
		}
		SetColorTable(0, ncolors, temp);//Sets the red, green, blue (RGB) color values for a range of entries in the palette of the DIB section.
		delete[]temp;

	}
}
void CDib::TurnBlue()
{
	int ncolors = GetMaxColorTableEntries();
	if (ncolors > 0)
	{
		RGBQUAD *temp = new RGBQUAD[ncolors];
		for (int i = 0; i < ncolors; i++)
		{
			temp[i].rgbRed = BYTE(0);
			temp[i].rgbBlue = i;
			temp[i].rgbGreen = BYTE(0);
			temp[i].rgbReserved = BYTE(0);
		}
		SetColorTable(0, ncolors, temp);
		delete[]temp;

	}
}
void CDib::TurnGreen()
{
	int ncolors = GetMaxColorTableEntries();
	if (ncolors > 0)
	{
		RGBQUAD *temp = new RGBQUAD[ncolors];
		for (int i = 0; i < ncolors; i++)
		{
			temp[i].rgbRed = BYTE(0);
			temp[i].rgbBlue = BYTE(0);
			temp[i].rgbGreen = i;
			temp[i].rgbReserved = BYTE(0);
		}
		SetColorTable(0, ncolors, temp);
		delete[]temp;

	}
}

void CDib::Histogramequlization()
{
	m_pGrayValueCount = GrayValueCount(); //���Ѿ�����õ�m_pGrayValueCount���������Ұָ�룬��Ϊ��grayvaluecount������û��delete�·��������
	long nColors = GetMaxColorTableEntries(); // longҲ���������ͱ���
	double *grayvalue_double = new double[nColors];  //������һ�����ֱ��ͼ
	double *equaled_grayvalue = new double[nColors];  
	memset(equaled_grayvalue, 0, nColors);
	for (int i = 0; i < nColors; i++)
		grayvalue_double[i] = double(m_pGrayValueCount[i]) / (m_nWidth*m_nHeight);
	
	for (int i = 0; i < nColors; i++)
	{
		double sum = 0;
		for (int j = 0; j <= i; j++)
			sum += grayvalue_double[j];
		sum *= 255;
		equaled_grayvalue[i] = sum;
	}
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			*(m_pDibBits + i * m_nWidthBytes + j) = (unsigned char)equaled_grayvalue[ *(m_pDibBits + i * m_nWidthBytes + j)];
		}
	}
	delete grayvalue_double;
	grayvalue_double = NULL;
	delete equaled_grayvalue;
	equaled_grayvalue = NULL;

}



void CDib::Spacial_Filter(CDib *buffer, int type)  // m_pDib and m_pDib_Buffer belong to class dipDoc
{
	if (m_nBitCount == 8)
	{
		for (int i = 1; i < m_nHeight - 1; i++)
		{
			for (int j = 1; j < m_nWidth - 1; j++)
			{
				int filterval_temp[9];
				filterval_temp[0] = *(buffer->m_pDibBits + (i - 1) * m_nWidth + j - 1);
				filterval_temp[1] = *(buffer->m_pDibBits + (i - 1) * m_nWidth + j);
				filterval_temp[2] = *(buffer->m_pDibBits + (i - 1) * m_nWidth + j + 1);
				filterval_temp[3] = *(buffer->m_pDibBits + (i)* m_nWidth + j - 1);
				filterval_temp[4] = *(buffer->m_pDibBits + (i)* m_nWidth + j);
				filterval_temp[5] = *(buffer->m_pDibBits + (i)* m_nWidth + j + 1);
				filterval_temp[6] = *(buffer->m_pDibBits + (i + 1)* m_nWidth + j - 1);
				filterval_temp[7] = *(buffer->m_pDibBits + (i + 1)* m_nWidth + j);
				filterval_temp[8] = *(buffer->m_pDibBits + (i + 1)* m_nWidth + j + 1);
				double filtered = 0;
				double temp = 0;
				if (type == 1) //��ֵ
				{
					for (int m = 0; m < 9; m++)
					{
						filtered += ((double)filterval_temp[m] * box[m]);
					}
					*(m_pDibBits + i * m_nWidth + j) = filtered / 9;
				}
				else if (type == 2) //��ֵ
				{
					if (m_nBitCount == 8)
					{
						for (int i = 0; i < 9; i++)
						{
							for (int j = 0; j < 8; j++)
							{
								if (filterval_temp[j] < filterval_temp[j + 1])
									swap(filterval_temp[j], filterval_temp[j + 1]);
							}
						}
						*(m_pDibBits + i * m_nWidth + j) = filterval_temp[4];
					}

				}
				else if (type == 3) //sobel
				{
					for (int m = 0; m < 9; m++)
					{
						filtered += ((double)filterval_temp[m] * gx[m]);
						temp += ((double)filterval_temp[m] * gy[m]);
					}
					filtered = abs(filtered);
					temp = abs(temp);
					filtered += temp;
					if (filtered < 255)
						*(m_pDibBits + i * m_nWidth + j) = filtered;
					else
						*(m_pDibBits + i * m_nWidth + j) = 255;
				}
				else if (type == 4) //laplacian
				{
					for (int m = 0; m < 9; m++)
					{
						filtered += ((double)filterval_temp[m] * laplacian[m]);
					}


					*(m_pDibBits + i * m_nWidth + j) += filtered;

				}
			}
		}
	}
	else if (m_nBitCount == 24)
	{
		
		if (type == 1)
		{
			for (int i = 1; i < m_nHeight - 1; i++)
			{
				for (int j = 1; j < m_nWidth - 1; j++)
				{

					int R[9], G[9], B[9];
					int k = 0;
					for (int m = -1; m <= 1; m++)
					{
						for (int n = -1; n <= 1; n++)
						{
							RGBTRIPLE *tempRGB = (RGBTRIPLE*)(buffer->m_pDibBits + (i + m) * buffer->m_nWidthBytes + (j + n) * 3);
							R[k] = tempRGB->rgbtRed;
							G[k] = tempRGB->rgbtGreen;
							B[k] = tempRGB->rgbtBlue;
							k++;
						}
					}
					double tempR = 0, tempG = 0, tempB = 0;

					for (int m = 0; m < 9; m++)
					{
						tempR += R[m] * box[m];
						tempG += G[m] * box[m];
						tempB += B[m] * box[m];
					}
					tempR /= 9.0;
					tempG /= 9.0;
					tempB /= 9.0;
					RGBTRIPLE *pivot = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
					pivot->rgbtRed = (BYTE)tempR;
					pivot->rgbtGreen = (BYTE)tempG;
					pivot->rgbtBlue = (BYTE)tempB;

				}
			}

		}
		else if (type == 2)
		{
			for (int i = 1; i < m_nHeight - 1; i++)
			{
				for (int j = 1; j < m_nWidth - 1; j++)
				{

					int R[9], G[9], B[9];
					int k = 0;
					for (int m = -1; m <= 1; m++)
					{
						for (int n = -1; n <= 1; n++)
						{
							RGBTRIPLE *tempRGB = (RGBTRIPLE*)(buffer->m_pDibBits + (i + m) * buffer->m_nWidthBytes + (j + n) * 3);
							R[k] = tempRGB->rgbtRed;
							G[k] = tempRGB->rgbtGreen;
							B[k] = tempRGB->rgbtBlue;
							k++;
						}
					}
					for (int i = 0; i < 9; i++)
					{
						for (int j = 0; j < 8; j++)
						{
							if (R[j] < R[j + 1])
								swap(R[j], R[j + 1]);
						}
					}
					for (int i = 0; i < 9; i++)
					{
						for (int j = 0; j < 8; j++)
						{
							if (G[j] < G[j + 1])
								swap(G[j], G[j + 1]);
						}
					}
					for (int i = 0; i < 9; i++)
					{
						for (int j = 0; j < 8; j++)
						{
							if (B[j] < B[j + 1])
								swap(B[j], B[j + 1]);
						}
					}
					RGBTRIPLE *pivot = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
					pivot->rgbtRed = R[4];
					pivot->rgbtGreen = G[4];
					pivot->rgbtBlue = B[4];

				}
			}
		}
		else if (type == 3)
		{
			for (int i = 1; i < m_nHeight - 1; i++)
			{
				for (int j = 1; j < m_nWidth - 1; j++)
				{

					int R[9], G[9], B[9];
					int k = 0;
					for (int m = -1; m <= 1; m++)
					{
						for (int n = -1; n <= 1; n++)
						{
							RGBTRIPLE *tempRGB = (RGBTRIPLE*)(buffer->m_pDibBits + (i + m) * buffer->m_nWidthBytes + (j + n) * 3);
							R[k] = tempRGB->rgbtRed;
							G[k] = tempRGB->rgbtGreen;
							B[k] = tempRGB->rgbtBlue;
							k++;
						}
					}
					double tempR = 0, tempG = 0, tempB = 0;
					double TEMPR = 0, TEMPG = 0, TEMPB = 0;
					for (int m = 0; m < 9; m++)
					{
						tempR += (R[m] * gx[m]);
						tempG += (G[m] * gx[m]);
						tempB += (B[m] * gx[m]);
						TEMPR += (R[m] * gy[m]);
						TEMPG += (G[m] * gy[m]);
						TEMPB += (B[m] * gy[m]);
					}
					tempR = abs(tempR);
					tempG = abs(tempG);
					tempB = abs(tempB);
					TEMPR = abs(TEMPR);
					TEMPG = abs(TEMPG);
					TEMPB = abs(TEMPB);
					RGBTRIPLE *pivot = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
					pivot->rgbtRed = tempR+TEMPR;
					pivot->rgbtGreen = tempG+TEMPG;
					pivot->rgbtBlue = tempB+TEMPB;

				}
			}
		}
		else if (type == 4)
		{
			for (int i = 1; i < m_nHeight - 1; i++)
			{
				for (int j = 1; j < m_nWidth - 1; j++)
				{

					int R[9], G[9], B[9];
					int k = 0;
					for (int m = -1; m <= 1; m++)
					{
						for (int n = -1; n <= 1; n++)
						{
							RGBTRIPLE *tempRGB = (RGBTRIPLE*)(buffer->m_pDibBits + (i + m) * buffer->m_nWidthBytes + (j + n) * 3);
							R[k] = tempRGB->rgbtRed;
							G[k] = tempRGB->rgbtGreen;
							B[k] = tempRGB->rgbtBlue;
							k++;
						}
					}
					double tempR = 0, tempG = 0, tempB = 0;

					for (int m = 0; m < 9; m++)
					{
						tempR += (R[m] * laplacian[m]);
						tempG += (G[m] * laplacian[m]);
						tempB += (B[m] * laplacian[m]);
					}
					RGBTRIPLE *pivot = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
					pivot->rgbtRed = abs(tempR);
					pivot->rgbtGreen =abs(tempG);
					pivot->rgbtBlue = abs(tempB);

				}
			}
		}
		
	

		
	}
}
long* CDib::GrayValueCount()
{
	long nColors = GetMaxColorTableEntries(); // 256
	if (nColors == 0)
	{
		return NULL;
	}
	long *pGrayValueCount = new long[nColors];
	memset(pGrayValueCount,0,nColors*sizeof(long));
	for (int i=0;i<m_nHeight;i++)
	{
		for (int j=0;j<m_nWidth;j++)
		{
			pGrayValueCount[*(m_pDibBits + i*m_nWidthBytes +j)]++;
		}
	}
	return pGrayValueCount;
}

void CDib::littlewhite(int block_width, int block_height)
{
	if (!IsNull()) //Determines if a bitmap is currently loaded
	{
		Destroy();
	}
	long Width = 512;
	long Height = 512;
	int BPP = 8;
	Create(Width, Height, BPP, 0);

	if (IsIndexed())//Determines whether a bitmap's pixels are mapped to a color palette.
	{
		int nColors = GetMaxColorTableEntries();

		if (nColors > 0)
		{
			RGBQUAD *temp = new RGBQUAD[nColors];
			for (int i = 0; i < nColors; i++)
			{
				temp[i].rgbRed = i;
				temp[i].rgbGreen = i;
				temp[i].rgbBlue = i;
				temp[i].rgbReserved = 0;
			}
			SetColorTable(0, nColors, temp);
			delete[] temp;
		}
	}
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1)*GetPitch();

	for (int i = m_nWidth / 2 - block_width / 2; i < (m_nWidth / 2 + block_width / 2); i++)
	{
		for (int j = m_nHeight / 2 - block_height / 2; j < (m_nHeight / 2 + block_height / 2); j++)
		{
			*(m_pDibBits + i * m_nWidthBytes + j) = 255;
		}
	}

}
void CDib::colorcircle()//��ɫͼ��û�е�ɫ��
{
	if (!IsNull())
	{
		Destroy();
	}
	long Width = 512;
	long Height = 512;
	int BPP = 24;
	Create(Width, Height, BPP, 0);

	
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1)*GetPitch();

	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			if (pow(256 -70- j, 2) + pow(256  - i, 2) <= pow(110, 2))
			{
				RGBTRIPLE *pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);  //ǿ��ת��
				pRGB->rgbtBlue = 255;
			}
			if (pow(256 - j, 2) + pow(256 + 80 - i, 2) <= pow(110, 2))
			{
				RGBTRIPLE *pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
				pRGB->rgbtGreen = 255;
			}
			if (pow(256 + 70 - j, 2) + pow(256 - i, 2) <= pow(110, 2))
			{
				RGBTRIPLE *pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
				pRGB->rgbtRed = 255;
			}
		}
	}

}


void CDib::binarization()
{
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)   //width still num of pixel
		{

			RGBTRIPLE *pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
			if (pRGB->rgbtRed > 218 && pRGB->rgbtBlue > 218 && pRGB->rgbtGreen>218)
			{
				pRGB->rgbtRed = 255;
				pRGB->rgbtBlue = 255;
				pRGB->rgbtGreen = 255;
			}
			else
			{
				pRGB->rgbtRed = 0;
				pRGB->rgbtBlue = 0;
				pRGB->rgbtGreen = 0;
			}
			
		}
	}
}
void CDib::inverse_colour()
{
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)   //width still num of pixel
		{

			RGBTRIPLE *pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
			if (pRGB->rgbtRed ==255)
			{
				pRGB->rgbtRed = 0;
				pRGB->rgbtBlue = 0;
				pRGB->rgbtGreen = 0;
			}
			else
			{
				pRGB->rgbtRed = 255;
				pRGB->rgbtBlue = 255;
				pRGB->rgbtGreen = 255;
			}

		}
	}
}
void CDib::errosion()
{
	BYTE *storage = new BYTE[m_nHeight*m_nWidthBytes];
	memcpy(storage, m_pDibBits, m_nHeight*m_nWidthBytes);
	memset(m_pDibBits, 0, m_nHeight*m_nWidthBytes);//�쳣�ؼ�
	for (int i = 1; i < m_nHeight - 1; i++)
	{
		for (int j = 1; j < m_nWidth - 1; j++)
		{
			bool flag = true;
			for (int m = -1; m <= 1; m++)
			{
				for (int n = -1; n <= 1; n++)
				{
					
					RGBTRIPLE *tempRGB = (RGBTRIPLE *)&storage[(i + m) *m_nWidthBytes + (j + n) * 3];
					if (tempRGB->rgbtRed != 255)
					{
						flag = false;
					}
				}
			}
			if (flag)
			{
				*(m_pDibBits + i * m_nWidthBytes + 3 * j) = 255;
				*(m_pDibBits + i * m_nWidthBytes + 3 * j + 1) = 255;
				*(m_pDibBits + i * m_nWidthBytes + 3 * j + 2) = 255;
			}
		}
	}
	delete storage;
	storage = NULL;
	
}
void CDib::Dilation()
{
	BYTE *storage = new BYTE[m_nHeight*m_nWidthBytes];
	memcpy(storage, m_pDibBits, m_nHeight*m_nWidthBytes);
	for (int i = 1; i < m_nHeight - 1; i++)
	{
		for (int j = 1; j < m_nWidth - 1; j++)
		{
			for (int m = -1; m <= 1; m++)
			{
				for (int n = -1; n <= 1; n++)
				{
					RGBTRIPLE *tempRGB = (RGBTRIPLE *) &storage[(i + m) *m_nWidthBytes + (j + n) * 3];
					if (tempRGB->rgbtRed==255 )
					{
						*(m_pDibBits + i * m_nWidthBytes + 3*j) = 255; 
						*(m_pDibBits + i * m_nWidthBytes + 3*j+1) = 255;
						*(m_pDibBits + i * m_nWidthBytes + 3 * j + 2) = 255;
						break;
					}

				}
			}
		}
	}
	delete storage;
	storage = NULL;
	
}





void CDib::holefilling()
{
	BYTE *I = new BYTE[m_nHeight*m_nWidthBytes];
	BYTE *F = new BYTE[m_nHeight*m_nWidthBytes];
	for (int i = 0; i < m_nHeight*m_nWidthBytes; i++)
	{
		I[i] = ~*(m_pDibBits + i);

	}
	memcpy(F, I, m_nHeight*m_nWidthBytes);
	for (int i = 1; i < m_nHeight - 1; i++)
	{
		for (int j = 1; j < m_nWidthBytes-1; j++)
		{
			F[i*m_nWidthBytes + j] = 0;
		}
	}
	memcpy(m_pDibBits, F, m_nHeight*m_nWidthBytes);
	int j = 0;
	while (j < 200)
	{
		Dilation();
		for (int i = 0; i < m_nHeight*m_nWidthBytes; i++)
		{
			*(m_pDibBits+i) &= I[i];

		}
		j++;
	}
	for (int i = 0; i < m_nHeight*m_nWidthBytes; i++)
	{
		*(m_pDibBits + i) = ~*(m_pDibBits + i);

	}
	delete I;
	I = NULL;
	delete F;
	F = NULL;
}

void CDib::bound()
{
	BYTE *inital = new BYTE[m_nHeight*m_nWidthBytes];
	memcpy(inital, m_pDibBits, m_nHeight*m_nWidthBytes);
	errosion();
	for (int i = 0; i < m_nHeight*m_nWidthBytes; i++)
	{
		inital[i] -= *(m_pDibBits + i);

	}
	memcpy(m_pDibBits,inital, m_nHeight*m_nWidthBytes);
	delete inital;
	inital = NULL;
}

void CDib::enhance(CDib*buffer)
{
	BYTE *inital = new BYTE[m_nHeight*m_nWidthBytes];
	memcpy(inital, m_pDibBits, m_nHeight*m_nWidthBytes);
	errosion();
	for (int i = 0; i < m_nHeight*m_nWidthBytes; i++)
	{
		inital[i] -= *(m_pDibBits + i);

	}//inital ���Ǳ߽�ͼ��
	memcpy(m_pDibBits, buffer->m_pDibBits, m_nHeight*m_nWidthBytes);
	for (int i = 0; i < m_nHeight ; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			//RGBTRIPLE *tempRGB = (RGBTRIPLE *)&inital[i *m_nWidthBytes + j * 3];//��һ���ǳ�����
			RGBTRIPLE *tempRGB = (RGBTRIPLE *)(inital + i * m_nWidthBytes + j * 3);//��ע�͵������ǵȼ۵�
			if (tempRGB->rgbtRed == 255)
			{
				*(m_pDibBits + i * m_nWidthBytes + 3 * j) = 255;
				*(m_pDibBits + i * m_nWidthBytes + 3 * j + 1) = 255;
				*(m_pDibBits + i * m_nWidthBytes + 3 * j + 2) = 0;
			}

		}
	}
	delete inital;
	inital = NULL;
}

void CDib::intensity()
{
	

	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			
			RGBTRIPLE *pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
			double intensity = 0.33*pRGB->rgbtRed+ 0.33*pRGB->rgbtBlue+ 0.33*pRGB->rgbtGreen;
			pRGB->rgbtRed = intensity;
			pRGB->rgbtBlue = intensity;
			pRGB->rgbtGreen = intensity;
		}
	}
	
}
void CDib::sat()
{


	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{

			RGBTRIPLE *pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
			double intensity = min(pRGB->rgbtRed, pRGB->rgbtBlue);
			intensity = min(intensity, pRGB->rgbtGreen);
			double saturation = 1 - ((1.0*(3 * intensity)) / (pRGB->rgbtRed + pRGB->rgbtBlue + pRGB->rgbtGreen));

			pRGB->rgbtRed = 255*saturation;
			pRGB->rgbtBlue = 255*saturation;
			pRGB->rgbtGreen = 255 * saturation;
		}
	}

}
void CDib::hue()
{


	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{

			RGBTRIPLE *pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
			double intensity = (0.5*(pRGB->rgbtRed- pRGB->rgbtGreen+ pRGB->rgbtRed- pRGB->rgbtBlue))/(1.0*(pow((pRGB->rgbtRed- pRGB->rgbtBlue)*(pRGB->rgbtGreen- pRGB->rgbtBlue)+pow(pRGB->rgbtRed- pRGB->rgbtGreen,2),0.5)));
			intensity = acos(intensity);
			if (pRGB->rgbtGreen < pRGB->rgbtBlue)
				intensity = 2*3.14 - intensity;
			intensity = (intensity  / 6.28)*255;
			pRGB->rgbtRed = intensity;
			pRGB->rgbtBlue =intensity;
			pRGB->rgbtGreen = intensity;
		}
	}

}
void CDib::spectrum_analyze()
{
	unsigned char*	lpSrc;							// ָ��Դͼ���ָ��
	int y;										// ѭ�����Ʊ���
	int x;										// ѭ�����Ʊ���
	double dTmpOne;								//�����ʱ����
	double dTmpTwo;								//�����ʱ����
	int nTransWidth;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	complex<double> * pCTData;						// ָ��ʱ�����ݵ�ָ��
	complex<double> * pCFData;						// ָ��Ƶ�����ݵ�ָ��
	// ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	dTmpOne = log(1.0*m_nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	dTmpOne = log(1.0* m_nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;									// ����Ҷ�任��ʵ��
	double dImag;									// ����Ҷ�任���鲿

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	// ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y*nTransWidth + x] = complex<double>(0, 0);		// ����
		}
	}
	//��ͼ�����ݴ���pCTData
	for (y = 0; y < m_nHeight; y++)
	{
		for (x = 0; x < m_nWidth; x++)
		{
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)m_pDibBits + m_nWidth * (m_nHeight - 1 - y) + x;
			unchValue = (*lpSrc)*pow(-1.0, x + y);
			pCTData[y*nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData,m_nWidth, m_nHeight, pCFData);				// ����Ҷ���任


	for (y = 0; y < m_nHeight; y++)								// ���任�����ݴ���lpDIBBits
	{
		for (x = 0; x < m_nWidth; x++)
		{
			//��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			dReal = pCFData[y*nTransWidth + x].real();		// ʵ��
			dImag = pCFData[y*nTransWidth + x].imag();		// �鲿
			unchValue = sqrt(dImag*dImag + dReal * dReal) / 100  ;
			//unchValue = dReal*pow(-1.0,x+y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)m_pDibBits + m_nWidth * (m_nHeight - 1 - y) + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete pCTData;										// �ͷ��ڴ�
	delete pCFData;										// �ͷ��ڴ�
	pCTData = NULL;
	pCFData = NULL;
	// ���ؽ��
}

BOOL CDib::FFT(unsigned char* pDIBBits, long nWidth, long nHeight)
{ 
	unsigned char*	lpSrc;							// ָ��Դͼ���ָ��
	int y ;										// ѭ�����Ʊ���
	int x ;										// ѭ�����Ʊ���
	double dTmpOne ;								//�����ʱ����
	double dTmpTwo ;								//�����ʱ����
	int nTransWidth ;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	complex<double> * pCTData ;						// ָ��ʱ�����ݵ�ָ��
	complex<double> * pCFData ;						// ָ��Ƶ�����ݵ�ָ��
	// ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)	;
	dTmpTwo = pow(2,dTmpTwo);
	nTransWidth = (int) dTmpTwo;	
	// ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)	;
	dTmpTwo = pow(2,dTmpTwo);
	nTransHeight = (int) dTmpTwo;
	double dReal;									// ����Ҷ�任��ʵ��
	double dImag;									// ����Ҷ�任���鲿
	
	pCTData=new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	pCFData=new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	// ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	for(y=0; y<nTransHeight; y++)
	{
		for(x=0; x<nTransWidth; x++)
		{
			pCTData[y*nTransWidth + x]=complex<double>(0,0);		// ����
		}
	}
	//��ͼ�����ݴ���pCTData
	for(y=0; y<nHeight; y++)
	{
		for(x=0; x<nWidth; x++)
		{
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc)*pow(-1.0,x+y);
			pCTData[y*nTransWidth + x]=complex<double>(unchValue,0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData) ;				// ����Ҷ���任

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	//
	//
	//
	//
	///////////////////////////////////////////////////////////////////////////

	IFFT_2D(pCFData, pCTData,nHeight, nWidth); 				// ����Ҷ���任

	for(y=0; y<nHeight; y++)								// ���任�����ݴ���lpDIBBits
	{
		for(x=0; x<nWidth; x++)
		{
			//��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			dReal = pCTData[y*nTransWidth + x].real() ;		// ʵ��
			dImag = pCTData[y*nTransWidth + x].imag() ;		// �鲿
			unchValue = dReal*pow(-1.0,x+y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc =(BYTE)unchValue ;
		}
	}	
	
	delete pCTData;										// �ͷ��ڴ�
	delete pCFData;										// �ͷ��ڴ�
	pCTData = NULL;
	pCFData = NULL;	
	return (true);										//���ؽ��
}

/*************************************************************************
 *
 * \�������ƣ�
 *   FFT_1D()
 *
 * \�������:
 *   complex<double> * pCTData	- ָ��ʱ�����ݵ�ָ�룬�������Ҫ�任������
 *   complex<double> * pCFData	- ָ��Ƶ�����ݵ�ָ�룬����ľ����任������
 *   nLevel						������Ҷ�任�����㷨�ļ�����2��������
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   һά���ٸ���Ҷ�任��
 *
 *************************************************************************
 */
void CDib::FFT_1D(complex<double> * pCTData, complex<double> * pCFData, int nLevel)
{
		// ѭ�����Ʊ���
	int		i;
	int     j;
	int     k;

	// ����Ҷ�任����
	int	nCount =0 ;

	// ���㸵��Ҷ�任����
	nCount =(int)pow(2.0,nLevel) ;
	
	// ĳһ���ĳ���
	int		nBtFlyLen;
	nBtFlyLen = 0 ;
	
	// �任ϵ���ĽǶ� ��2 * PI * i / nCount
	double	dAngle;
	
	complex<double> *pCW ;
	
	// �����ڴ棬�洢����Ҷ�仯��Ҫ��ϵ����
	pCW  = new complex<double>[nCount / 2];

    // ���㸵��Ҷ�任��ϵ��
	for(i = 0; i < nCount / 2; i++)
	{
		dAngle = -2 * Pi * i / nCount;
		pCW[i] = complex<double> ( cos(dAngle), sin(dAngle) );
	}

	// �任��Ҫ�Ĺ����ռ�
	complex<double> *pCWork1,*pCWork2; 
	
	// ���乤���ռ�
	pCWork1 = new complex<double>[nCount];

	pCWork2 = new complex<double>[nCount];

	
	// ��ʱ����
	complex<double> *pCTmp;
	
	// ��ʼ����д������
	memcpy(pCWork1, pCTData, sizeof(complex<double>) * nCount);

	// ��ʱ����
	int nInter; 
	nInter = 0;

	// �����㷨���п��ٸ���Ҷ�任
	for(k = 0; k < nLevel; k++)
	{
		for(j = 0; j < (int)pow(2.0,k); j++)
		{
			//���㳤��
			nBtFlyLen = (int)pow( 2.0,(nLevel-k) );
			
			//�������ţ���Ȩ����
			for(i = 0; i < nBtFlyLen/2; i++)
			{
				nInter = j * nBtFlyLen;
				pCWork2[i + nInter] = 
					pCWork1[i + nInter] + pCWork1[i + nInter + nBtFlyLen / 2];
				pCWork2[i + nInter + nBtFlyLen / 2] =
					(pCWork1[i + nInter] - pCWork1[i + nInter + nBtFlyLen / 2]) 
					* pCW[(int)(i * pow(2.0,k))];
			}
		}

		// ���� pCWork1��pCWork2������
		pCTmp   = pCWork1	;
		pCWork1 = pCWork2	;
		pCWork2 = pCTmp		;
	}
	
	// ��������
	for(j = 0; j < nCount; j++)
	{
		nInter = 0;
		for(i = 0; i < nLevel; i++)
		{
			if ( j&(1<<i) )
			{
				nInter += 1<<(nLevel-i-1);
			}
		}
		pCFData[j]=pCWork1[nInter];
	}
	
	// �ͷ��ڴ�ռ�
	delete pCW;
	delete pCWork1;
	delete pCWork2;
	pCW		=	NULL	;
	pCWork1 =	NULL	;
	pCWork2 =	NULL	;

}
/*************************************************************************
 *
 * \�������ƣ�
 *    IFFT_1D()
 *
 * \�������:
 *   complex<double> * pCTData	- ָ��ʱ�����ݵ�ָ�룬�������Ҫ���任������
 *   complex<double> * pCFData	- ָ��Ƶ�����ݵ�ָ�룬����ľ������任������
 *   nLevel						������Ҷ�任�����㷨�ļ�����2��������
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   һά���ٸ���Ҷ���任��
 *
 ************************************************************************
 */
void CDib::IFFT_1D(complex<double> * pCFData, complex<double> * pCTData, int nLevel)
{
	
	// ѭ�����Ʊ���
	int		i;

	// ����Ҷ���任����
	int nCount;

	// ���㸵��Ҷ�任����
	nCount = (int)pow(2.0,nLevel) ;
	
	// �任��Ҫ�Ĺ����ռ�
	complex<double> *pCWork;	
	
	// ���乤���ռ�
	pCWork = new complex<double>[nCount];
	
	// ����Ҫ���任������д�빤���ռ�pCWork
	memcpy(pCWork, pCFData, sizeof(complex<double>) * nCount);
	
	// Ϊ�����ø���Ҷ���任,���԰Ѹ���ҶƵ�������ȡ����
	// Ȼ��ֱ���������任�����������Ǹ���Ҷ���任����Ĺ���
	for(i = 0; i < nCount; i++)
	{
		pCWork[i] = complex<double> (pCWork[i].real(), -pCWork[i].imag());
	}
	
	// ���ÿ��ٸ���Ҷ�任ʵ�ַ��任������洢��pCTData��
	FFT_1D(pCWork, pCTData, nLevel);
	
	// ��ʱ���Ĺ��������ս��
	// ���ݸ���Ҷ�任ԭ�����������ķ�����õĽ����ʵ�ʵ�ʱ������
	// ���һ��ϵ��nCount
	for(i = 0; i < nCount; i++)
	{
		pCTData[i] = 
			complex<double> (pCTData[i].real() / nCount, -pCTData[i].imag() / nCount);
	}
	
	// �ͷ��ڴ�
	delete pCWork;
	pCWork = NULL;
}

/*************************************************************************
 *
 * \�������ƣ�
 *   FFT_2D()
 *
 * \�������:
 *   complex<double> * pCTData	- ͼ������
 *   int    nWidth				- ���ݿ��
 *   int    nHeight				- ���ݸ߶�
 *   complex<double> * pCFData	- ����Ҷ�任��Ľ��
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   ��ά����Ҷ�任��
 *
 ************************************************************************
 */
void CDib::FFT_2D(complex<double> * pCTData, int nWidth, int nHeight, complex<double> * pCFData)
{
	
	// ѭ�����Ʊ���
	int	x;
	int	y;
	
	// ��ʱ����
	double	dTmpOne;
	double  dTmpTwo;
	
	// ���и���Ҷ�任�Ŀ�Ⱥ͸߶ȣ���2���������ݣ�
	// ͼ��Ŀ�Ⱥ͸߶Ȳ�һ��Ϊ2����������
	int		nTransWidth;
	int 	nTransHeight;

	// ������и���Ҷ�任�Ŀ��	��2���������ݣ�
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransWidth = (int) dTmpTwo	   ;
	
	// ������и���Ҷ�任�ĸ߶� ��2���������ݣ�
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransHeight = (int) dTmpTwo	   ;	
	
	// x��y�����У������ϵĵ�������
	int		nXLev;
	int		nYLev;

	// ����x��y�����У������ϵĵ�������
	nXLev = (int) ( log(1.0*nTransWidth)/log(2.0) +  0.5 );
	nYLev = (int) ( log(1.0*nTransHeight)/log(2.0) + 0.5 );
	
	for(y = 0; y < nTransHeight; y++)
	{
		// x������п��ٸ���Ҷ�任
		FFT_1D(&pCTData[nTransWidth * y], &pCFData[nTransWidth * y], nXLev);
	}
	
	// pCFData��Ŀǰ�洢��pCTData�����б任�Ľ��
	// Ϊ��ֱ������FFT_1D����Ҫ��pCFData�Ķ�ά����ת�ã���һ������FFT_1D����
	// ����Ҷ�б任��ʵ�����൱�ڶ��н��и���Ҷ�任��
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTData[nTransHeight * x + y] = pCFData[nTransWidth * y + x];
		}
	}
	
	for(x = 0; x < nTransWidth; x++)
	{
		// ��x������п��ٸ���Ҷ�任��ʵ�����൱�ڶ�ԭ����ͼ�����ݽ����з����
		// ����Ҷ�任
		FFT_1D(&pCTData[x * nTransHeight], &pCFData[x * nTransHeight], nYLev);
	}

	// pCFData��Ŀǰ�洢��pCTData������ά����Ҷ�任�Ľ��������Ϊ�˷����з���
	// �ĸ���Ҷ�任�����������ת�ã����ڰѽ��ת�û���
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTData[nTransWidth * y + x] = pCFData[nTransHeight * x + y];
		}
	}
}

/*************************************************************************
 *
 * \�������ƣ�
 *   IFFT_2D()
 *
 * \�������:
 *   complex<double> * pCFData	- Ƶ������
 *   complex<double> * pCTData	- ʱ������
 *   int    nWidth				- ͼ�����ݿ��
 *   int    nHeight				- ͼ�����ݸ߶�
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   ��ά����Ҷ���任��
 *
 ************************************************************************
 */
void CDib::IFFT_2D(complex<double> * pCFData, complex<double> * pCTData, int nWidth, int nHeight)
{
	// ѭ�����Ʊ���
	int	x;
	int	y;
	
	// ��ʱ����
	double	dTmpOne;
	double  dTmpTwo;
	
	// ���и���Ҷ�任�Ŀ�Ⱥ͸߶ȣ���2���������ݣ�
	// ͼ��Ŀ�Ⱥ͸߶Ȳ�һ��Ϊ2����������
	int		nTransWidth;
	int 	nTransHeight;

	// ������и���Ҷ�任�Ŀ��	��2���������ݣ�
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransWidth = (int) dTmpTwo	   ;
	
	// ������и���Ҷ�任�ĸ߶� ��2���������ݣ�
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransHeight = (int) dTmpTwo	   ;
	
	// ���乤����Ҫ���ڴ�ռ�
	complex<double> *pCWork= new complex<double>[nTransWidth * nTransHeight];

	//��ʱ����
	complex<double> *pCTmp ;
	
	// Ϊ�����ø���Ҷ���任,���԰Ѹ���ҶƵ�������ȡ����
	// Ȼ��ֱ���������任�����������Ǹ���Ҷ���任����Ĺ���
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTmp = &pCFData[nTransWidth * y + x] ;
			pCWork[nTransWidth * y + x] = complex<double>( pCTmp->real() , -pCTmp->imag() );
		}
	}

	// ���ø���Ҷ���任
	FFT_2D(pCWork, nWidth, nHeight, pCTData) ;
	
	// ��ʱ���Ĺ��������ս��
	// ���ݸ���Ҷ�任ԭ�����������ķ�����õĽ����ʵ�ʵ�ʱ������
	// ���һ��ϵ��
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTmp = &pCTData[nTransWidth * y + x] ;
			pCTData[nTransWidth * y + x] = 
				complex<double>( pCTmp->real()/(nTransWidth*nTransHeight),
								 -pCTmp->imag()/(nTransWidth*nTransHeight) );
		}
	}
	delete pCWork ;
	pCWork = NULL ;
}

void CDib::filter_freq_domain_unpadded(int D0, int type) // type =1 ,2,3,4,5,6
//means ideal low pass ,gaussian low pass, butterworth low pass, and high pass 
{
	// in the FFT function provided by supplemantory , paras are
	// (unsigned char* pDIBBits, long nWidth, long nHeight) 
	// for convenience, use the same name in this filtering code
	long nWidth = m_nWidth;
	long nHeight = m_nHeight;
	unsigned char*	lpSrc;							// ָ��Դͼ���ָ��
	int y;										// ѭ�����Ʊ���
	int x;										// ѭ�����Ʊ���
	double dTmpOne;								//�����ʱ����
	double dTmpTwo;								//�����ʱ����
	int nTransWidth;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	complex<double> * pCTData;						// ָ��ʱ�����ݵ�ָ��
	complex<double> * pCFData;						// ָ��Ƶ�����ݵ�ָ��
	// ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	dTmpOne = log(1.0*nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	dTmpOne = log(1.0*nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;

	double dReal;									// ����Ҷ�任��ʵ��
	double dImag;									// ����Ҷ�任���鲿

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	// ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y*nTransWidth + x] = complex<double>(0, 0);		// ����
		}
	}
	//��ͼ�����ݴ���pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)m_pDibBits + nWidth * y + x; // here uses m_pDibBits
			unchValue = (*lpSrc)*pow(-1.0, x + y);
			pCTData[y*nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData);				// ����Ҷ���任

	if (type == 1)
	{
		double *H = new double[nTransWidth*nTransHeight];
		for (y = 0; y < nTransHeight; y++)
		{
			for (x = 0; x < nTransWidth; x++)
			{
				if (sqrt(pow((y - nTransHeight / 2), 2) + pow((x - nTransWidth / 2), 2)) <= D0)
					H[y*nTransWidth + x] = 1;
				else
					H[y*nTransWidth + x] = 0;
				pCFData[y*nTransWidth + x] *= H[y*nTransWidth + x];
			}
		}
		delete[]H;

	}

	else if (type == 2)
	{
		double *H = new double[nTransWidth*nTransHeight];
		for (y = 0; y < nTransHeight; y++)
		{
			for (x = 0; x < nTransWidth; x++)
			{
				if (sqrt(pow((y - nTransHeight / 2), 2) + pow((x - nTransWidth / 2), 2)) <= D0)
					H[y*nTransWidth + x] = 0;
				else
					H[y*nTransWidth + x] = 1;
				pCFData[y*nTransWidth + x] *= H[y*nTransWidth + x];
			}
		}
		delete[]H;
	}

	else if (type == 3)
	{
		double *H = new double[nTransWidth*nTransHeight];
		for (y = 0; y < nTransHeight; y++)
		{
			for (x = 0; x < nTransWidth; x++)
			{
				
				H[y*nTransWidth + x] = exp((-pow((y - nTransHeight / 2), 2) - pow((x - nTransWidth / 2), 2))/(2*pow(D0,2)));
				pCFData[y*nTransWidth + x] *= H[y*nTransWidth + x];
			}
		}
		delete[]H;
	}

	else if (type == 4)
	{
		double *H = new double[nTransWidth*nTransHeight];
		for (y = 0; y < nTransHeight; y++)
		{
			for (x = 0; x < nTransWidth; x++)
			{

				H[y*nTransWidth + x] = 1-exp((-pow((y - nTransHeight / 2), 2) - pow((x - nTransWidth / 2), 2)) / (2 * pow(D0, 2)));
				pCFData[y*nTransWidth + x] *= H[y*nTransWidth + x];
			}
		}
		delete[]H;
	}

	else if (type == 5)
	{
		double *H = new double[nTransWidth*nTransHeight];
		for (y = 0; y < nTransHeight; y++)
		{
			for (x = 0; x < nTransWidth; x++)
			{

				H[y*nTransWidth + x] = 1/(1 + pow(sqrt(pow((y - nTransHeight / 2), 2) + pow((x - nTransWidth / 2), 2))/ D0 ,2));
				pCFData[y*nTransWidth + x] *= H[y*nTransWidth + x];
			}
		}
		delete[]H;
	}
	else if (type == 6)
	{
		double *H = new double[nTransWidth*nTransHeight];
		for (y = 0; y < nTransHeight; y++)
		{
			for (x = 0; x < nTransWidth; x++)
			{

				H[y*nTransWidth + x] = 1 / (1 + pow(D0 / sqrt(pow((y - nTransHeight / 2), 2) + pow((x - nTransWidth / 2), 2)), 2));
				pCFData[y*nTransWidth + x] *= H[y*nTransWidth + x];
			}
		}
		delete[]H;
	}
	else if (type == 7)   // degradation
	{
		double *H = new double[nTransWidth*nTransHeight];
		for (y = 0; y < nTransHeight; y++)
		{
			for (x = 0; x < nTransWidth; x++)
			{
				double xt = x - 0.5*nTransWidth;
				double yt = y - 0.5*nTransHeight;
				H[y*nTransWidth + x] = exp(-0.001*pow((xt*xt + yt * yt), 5.0 / 6.0));

				pCFData[y*nTransWidth + x] *= H[y*nTransWidth + x];
			}
		}
		delete[]H;
	}
	else if (type == 8) // inverse filtering 
	{
		double *H = new double[nTransWidth*nTransHeight];
		for (y = 0; y < nTransHeight; y++)
		{
			for (x = 0; x < nTransWidth; x++)
			{
				H[y*nTransWidth + x] = 1 / (1 + pow(sqrt(pow((y - nTransHeight / 2), 2) + pow((x - nTransWidth / 2), 2)) / D0, 10));  //buttorworth �������ϵ�����������һ����ͨ
				pCFData[y*nTransWidth + x] *= H[y*nTransWidth + x];
				double xt = x - 0.5*nTransWidth;
				double yt = y - 0.5*nTransHeight;
				H[y*nTransWidth + x] = exp(-0.001*pow((xt*xt + yt * yt), 5.0 / 6.0));

				pCFData[y*nTransWidth + x] /= H[y*nTransWidth + x];
				
			}
		}
		delete[]H;
	}
	else if (type == 9)// wiener filtering 
	{
		double *H = new double[nTransWidth*nTransHeight];
		for (y = 0; y < nTransHeight; y++)
		{
			for (x = 0; x < nTransWidth; x++)
			{
				double xt = x - 0.5*nTransWidth;
				double yt = y - 0.5*nTransHeight;
				H[y*nTransWidth + x] = exp(-0.001*pow((xt*xt + yt * yt), 5.0 / 6.0));
				double temp = H[y*nTransWidth + x];
				double weiner =(1.0*temp*temp) / (1.0*temp*(0.02 + temp * temp));
				/*dReal = pCFData[y*nTransWidth + x].real();
				dReal *= weiner;
				dImag = pCFData[y*nTransWidth + x].imag();
				dImag *=weiner;
				pCFData[y*nTransWidth + x] = complex<double>(dReal, dImag);*/
				pCFData[y*nTransWidth + x] *= weiner; // ʵ����ʵ�����鲿���鲿��ֱ�ӳ˵�Ч����һ����

			}
		}
	delete[]H;
    }


	IFFT_2D(pCFData, pCTData, nHeight, nWidth); 				// ����Ҷ���任

	for (y = 0; y < nHeight; y++)								// ���任�����ݴ���lpDIBBits
	{
		for (x = 0; x < nWidth; x++)
		{
			//��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			dReal = pCTData[y*nTransWidth + x].real();		// ʵ��
			dImag = pCTData[y*nTransWidth + x].imag();		// �鲿
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)m_pDibBits + nWidth * y + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete pCTData;										// �ͷ��ڴ�
	delete pCFData;										// �ͷ��ڴ�
	pCTData = NULL;
	pCFData = NULL;
	// ���ؽ��
}

void CDib::filter_freq_domain_padded(int D0, int type) //
// D0�� cutoff frequency   type =1,2,3,4,5,6
//means ideal low pass ,gaussian low pass, butterworth low pass, and high pass 
{
	// in the FFT function provided by supplemantory , paras are
	// (unsigned char* pDIBBits, long nWidth, long nHeight) 
	// for convenience, use the same name in this filtering code
	long nWidth = m_nWidth*2;
	long nHeight = m_nHeight*2;
	unsigned char*	lpSrc;							// ָ��Դͼ���ָ��
	int y;										// ѭ�����Ʊ���
	int x;										// ѭ�����Ʊ���
	double dTmpOne;								//�����ʱ����
	double dTmpTwo;								//�����ʱ����
	int nTransWidth;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	complex<double> * pCTData;						// ָ��ʱ�����ݵ�ָ��
	complex<double> * pCFData;						// ָ��Ƶ�����ݵ�ָ��
	// ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	dTmpOne = log(1.0*nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	dTmpOne = log(1.0*nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;

	double dReal;									// ����Ҷ�任��ʵ��
	double dImag;									// ����Ҷ�任���鲿

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	// ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y*nTransWidth + x] = complex<double>(0, 0);		// ����
		}
	}
	//��ͼ�����ݴ���pCTData
	for (y = 0; y < m_nHeight; y++)  // main difference between unpadded code
	{
		for (x = 0; x < m_nWidth; x++)
		{
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)m_pDibBits + m_nWidth * y + x; // here uses m_pDibBits
			
			unchValue = (*lpSrc)*pow(-1.0, x + y);
			pCTData[y*nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	



	FFT_2D(pCTData, nWidth, nHeight, pCFData);				// ����Ҷ���任

	if (type == 1)
	{
		double *H = new double[nTransWidth*nTransHeight];
		for (y = 0; y < nTransHeight; y++)
		{
			for (x = 0; x < nTransWidth; x++)
			{
				if (sqrt(pow((y - nTransHeight / 2), 2) + pow((x - nTransWidth / 2), 2)) <= D0)
					H[y*nTransWidth + x] = 1;
				else
					H[y*nTransWidth + x] = 0;
				pCFData[y*nTransWidth + x] *= H[y*nTransWidth + x];
			}
		}
		delete[]H;

	}

	else if (type == 2)
	{
		double *H = new double[nTransWidth*nTransHeight];
		for (y = 0; y < nTransHeight; y++)
		{
			for (x = 0; x < nTransWidth; x++)
			{
				if (sqrt(pow((y - nTransHeight / 2), 2) + pow((x - nTransWidth / 2), 2)) <= D0)
					H[y*nTransWidth + x] = 0;
				else
					H[y*nTransWidth + x] = 1;
				pCFData[y*nTransWidth + x] *= H[y*nTransWidth + x];
			}
		}
		delete[]H;
	}

	else if (type == 3)
	{
		double *H = new double[nTransWidth*nTransHeight];
		for (y = 0; y < nTransHeight; y++)
		{
			for (x = 0; x < nTransWidth; x++)
			{

				H[y*nTransWidth + x] = exp((-pow((y - nTransHeight / 2), 2) - pow((x - nTransWidth / 2), 2)) / (2 * pow(D0, 2)));
				pCFData[y*nTransWidth + x] *= H[y*nTransWidth + x];
			}
		}
		delete[]H;
	}

	else if (type == 4)
	{
		double *H = new double[nTransWidth*nTransHeight];
		for (y = 0; y < nTransHeight; y++)
		{
			for (x = 0; x < nTransWidth; x++)
			{

				H[y*nTransWidth + x] = 1 - exp((-pow((y - nTransHeight / 2), 2) - pow((x - nTransWidth / 2), 2)) / (2 * pow(D0, 2)));
				pCFData[y*nTransWidth + x] *= H[y*nTransWidth + x];
			}
		}
		delete[]H;
	}

	else if (type == 5)
	{
		double *H = new double[nTransWidth*nTransHeight];
		for (y = 0; y < nTransHeight; y++)
		{
			for (x = 0; x < nTransWidth; x++)
			{

				H[y*nTransWidth + x] = 1 / (1 + pow(sqrt(pow((y - nTransHeight / 2), 2) + pow((x - nTransWidth / 2), 2)) / D0, 2));
				pCFData[y*nTransWidth + x] *= H[y*nTransWidth + x];
			}
		}
		delete[]H;
	}
	else if (type == 6)
	{
		double *H = new double[nTransWidth*nTransHeight];
		for (y = 0; y < nTransHeight; y++)
		{
			for (x = 0; x < nTransWidth; x++)
			{

				H[y*nTransWidth + x] = 1 / (1 + pow(D0 / sqrt(pow((y - nTransHeight / 2), 2) + pow((x - nTransWidth / 2), 2)), 2));
				pCFData[y*nTransWidth + x] *= H[y*nTransWidth + x];
			}
		}
		delete[]H;
	}
	IFFT_2D(pCFData, pCTData, nHeight, nWidth); 				// ����Ҷ���任

	for (y = 0; y < m_nHeight; y++)								// ���任�����ݴ���lpDIBBits
	{
		for (x = 0; x <m_nWidth; x++)
		{
			//��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			dReal = pCTData[y*nTransWidth + x].real();		// ʵ��
			dImag = pCTData[y*nTransWidth + x].imag();		// �鲿
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)m_pDibBits + m_nWidth * y + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete pCTData;										// �ͷ��ڴ�
	delete pCFData;										// �ͷ��ڴ�
	pCTData = NULL;
	pCFData = NULL;
	// ���ؽ��
}


//it is not safe to assume that pitch is simply the width multiplied by the number of bytes per pixel
//sometimes more cache allocated
//������Ҫת��Ҳ����һ��һ�з���


void CDib::fbp()
// sinogram:Ptheta(t)
{
	int num_angles = m_nWidth;
	int num_t = m_nHeight;

	
	int nLevel = ceil(log(1.0*m_nHeight) / log(2.0));
	int FFTLen = pow(2.0, nLevel);
	
	complex<double>* wFData = new complex<double>[FFTLen];
	complex<double>* wTData = new complex<double>[FFTLen];

	double* filterdata = new double[m_nWidth*m_nHeight];

	
	for (int lineX = 0; lineX < m_nWidth; lineX++)
	{
		unsigned char tempPixel;
		for (int lineY = 0; lineY < FFTLen; lineY++)
		{
			tempPixel = lineY < m_nHeight ? m_pDibBits[lineX + lineY * m_nWidth] : 0;
			wTData[lineY] = complex<double>(tempPixel, 0);
			wFData[lineY] = complex<double>(0, 0);
		}
		
		FFT_1D(wTData, wFData, nLevel);

		double filter;
		
		for (int w = 0; w < FFTLen / 2; w++)
		{
			filter = (w *1.0 / FFTLen * 2);     
			wFData[w] = complex<double>(wFData[w].real()*filter, wFData[w].imag()*filter);
			wFData[FFTLen - w - 1] = complex<double>
				(wFData[FFTLen - w - 1].real()*filter, wFData[FFTLen - w - 1].imag()*filter);
		}

		IFFT_1D(wFData, wTData, nLevel);

		//�ӹ��˲���������ݷŻ�buffer��ԭλ��
		for (int lineY = 0; lineY < m_nHeight; lineY++)
		{
			filterdata[lineY*m_nWidth + lineX] = wTData[lineY].real();
		}
	}

	int m_nWidth = 256;
	int m_nHeight = 256; //phantom256 ���ͶӰ���� �ؽ�ͼ��Ĵ�С����,���޸�,���Ϊż����pitch��width��ͬ

	if (!IsNull())  // important 
	{
		Destroy();
	}

	Create(m_nWidth, m_nHeight, 8, 0);

	if (IsIndexed())
	{
		int nColors = GetMaxColorTableEntries();

		if (nColors > 0)
		{
			RGBQUAD *temp = new RGBQUAD[nColors];
			for (int i = 0; i < nColors; i++)
			{
				temp[i].rgbRed = i;
				temp[i].rgbGreen = i;
				temp[i].rgbBlue = i;
				temp[i].rgbReserved = 0;
			}
			SetColorTable(0, nColors, temp);
			delete[] temp;
		}
	}
	
	m_nWidthBytes = abs(GetPitch());
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1)*GetPitch();

	double *recon = new double[m_nHeight*m_nWidthBytes];
	memset(recon, 0, sizeof(double)*m_nHeight*m_nWidthBytes);
	double max,min;
	max = min = 0;

	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)  //����bpp = 8;j����������   m_nWidthBytes = 368  m_nHeight = 367
		{
			for (int k = 0; k < num_angles; k++)
			{
				double t = (i - m_nHeight / 2)*cos(k*Pi/num_angles) + (j - m_nHeight / 2)*sin(k*Pi / num_angles)+num_t/2;
				if (t >= 0 && t < num_t)
				{
					int t1 = floor(t);
					int t2 = ceil(t);
					double ratio = (t - t1);
					recon[j*m_nWidthBytes + i] += (filterdata[k + t1 * num_angles]*ratio+ filterdata[k + t2 * num_angles]*(1-ratio));		//���Բ�ֵ,�˴���ת�ø�ֵ			
				}
			}
			max = (max < recon[j*m_nWidthBytes + i]) ? recon[j*m_nWidthBytes + i] : max;
			min = (min > recon[j*m_nWidthBytes + i]) ? recon[j*m_nWidthBytes + i] : min;
		}
	}

	
	double range = max - min;

	for (int i = 0; i < m_nHeight*m_nHeight; i++)
	{		
		recon[i] = (recon[i] - min) / range * 255; 
		*(m_pDibBits + i) = recon[i];
	}

	delete recon;
	delete filterdata;
	delete wFData;
	delete wTData;
	wFData = NULL;
	wTData = NULL;
	recon = NULL;
	filterdata = NULL;
}

void CDib::Zoom(CDib *Dib, double factor)
{
	long pastWidth = GetWidth();//ԭͼ�ߴ�
	long pastHeight = GetHeight();
	if (!IsNull())
	{
		Destroy();
	}

	m_nHeight = pastHeight * factor;
	m_nWidth = pastWidth * factor;
	factor = (double)m_nHeight / Dib->m_nHeight;	
	Create(m_nWidth, m_nHeight, m_nBitCount, 0);
	if (IsIndexed())
	{
		int nColors = Dib->GetMaxColorTableEntries();

		if (nColors > 0)
		{
			RGBQUAD *pal = new RGBQUAD[nColors];
			for (int i = 0; i < nColors; i++)
			{
				pal[i].rgbRed = i;
				pal[i].rgbGreen = i;
				pal[i].rgbBlue = i;
				pal[i].rgbReserved = 0;
			}
			SetColorTable(0, nColors, pal);
			delete[] pal;
		}
	}
	m_nWidthBytes = abs(GetPitch());
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1)*GetPitch();
	m_nBitCount = 8;
	double pastx;    //˫���Բ�ֵ
	double pasty;

	for (int x = 0; x < m_nHeight; x++)
	{
		for (int y = 0; y < m_nWidth; y++)
		{
			pastx = x  / factor ;
			pasty = y / factor ;
			int i =pastx;
			int j = pasty;
			double dx = pastx - i;
			double dy = pasty - j;
			if (i > Dib->m_nHeight-2 )
				i = Dib->m_nHeight-2 ;
			if (j > Dib->m_nWidth-2 ) 
				j = Dib->m_nWidth-2 ;


			int temp1 = *(Dib->m_pDibBits + i * Dib->m_nWidthBytes + j);
			int temp2 = *(Dib->m_pDibBits + i * Dib->m_nWidthBytes + (j + 1));
			int temp3 = *(Dib->m_pDibBits + (i + 1) * Dib->m_nWidthBytes + j);
			int temp4 = *(Dib->m_pDibBits + (i + 1) * Dib->m_nWidthBytes + (j + 1));
			int value = (1 - dx) * (1 - dy) * temp1 + (1 - dx) * dy * temp2 + dx * (1 - dy) * temp3 + dx * dy * temp4;
			*(m_pDibBits + x * m_nWidthBytes + y) = value;
		}
	}


}

void CDib::wavlet_2D(int m_nWidth, int m_nHeight,int type)
{
	// new work place
	//ͬһ���͵ı�������ʹ��memcpy 
	double *workplace = new double[m_nHeight*m_nWidth];

	for (int y = 0; y < m_nHeight; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			workplace[m_nWidth * y + x] = *(m_pDibBits + 512 * y + x);
		}
	}
	//row harr
	if (type == 1)
	{
		for (int i = 0; i < m_nHeight; i++)
		{
			wavlet_1Dharr(&workplace[m_nWidth * i], m_nWidth);
		}
	}

	else
	{
		for (int i = 0; i < m_nHeight; i++)
		{
			wavlet_1Ddb4(&workplace[m_nWidth * i], m_nWidth);
		}
	}
	//transpose

	transpose(workplace,m_nWidth,m_nHeight);
	//colum 
	if (type == 1)
	{
		for (int i = 0; i < m_nHeight; i++)
		{
			wavlet_1Dharr(&workplace[m_nWidth * i],  m_nHeight);
		}
	}
	else
	{
		for (int i = 0; i < m_nHeight; i++)
		{
			wavlet_1Ddb4(&workplace[m_nWidth * i], m_nHeight);
		}
	}
	
	transpose(workplace, m_nHeight, m_nWidth);
	int max = 0;
	int min = 0;
	for (int y = 0; y < m_nHeight/2 ; y++)
	{
		for (int x = 0; x < m_nWidth/2 ; x++)
		{
			double inch = workplace[m_nWidth*y + x];
			if (max < inch)
				max = inch;
			if (min > inch)
				min = inch;
		}
	}
	//�ֿ��һ�� ����û��
	for (int y = 0; y < m_nHeight/2 ; y++)
	{
		for (int x = 0; x < m_nWidth/2 ; x++)
		{
			workplace[m_nWidth*y + x] = (workplace[m_nWidth*y + x] - min)*1.0 / ((max - min)*1.0) * 255;
		}
	}

	for (int y = 0; y < m_nHeight; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			*(m_pDibBits + 512 * y + x)=workplace[m_nWidth * y + x] ;
		}
	}
	
	delete workplace;
	workplace = NULL;
}
//����ʧ��Ϊ4��dbС����ע��ʹ��double �������㣬�����¹���������Ϊcharֻ�ܱ�ʾ��������double�õ���ֵ���й�һ������ת��char��ǳ��ؼ�
void CDib::wavlet_1Dharr(double *linedata, int m_nWidth)
{
	double *store = new double[m_nWidth];
	//for (int i = 0; i < m_nWidth; i++)
	//{
	//	store[i] = *(linedata + i);
	//}
	memcpy(store, linedata, m_nWidth * sizeof(double));
	for (int i = 0; i < m_nWidth / 2; i++)
	{
		*(linedata + i) = (1.0 / 1.414)*store[2 * i] + (1.0 / 1.414)*store[2 * i + 1];
	}
	for (int i = m_nWidth / 2; i < m_nWidth; i++)
	{

		*(linedata + i) = (1.0 /1.414)*store[2 * (i - m_nWidth / 2)] - ((1.0 /1.414)*store[2 * (i - m_nWidth / 2) + 1]);
	}
	delete store;
	store = NULL;
}

void CDib::wavlet_1Ddb4(double *linedata, int m_nWidth)
{
	double db4L[8] = { -0.01059,0.03288,0.03084,-0.18703,-0.02798,0.63088,0.71484,0.23037 };
	double db4H[8] = { -0.23037,0.71484,-0.63088,-0.02798,0.18703,0.03084,-0.03288,-0.01059 };

	double *store = new double[m_nWidth];
	memcpy(store, linedata, m_nWidth * sizeof(double));
	for (int i = 0; i < m_nWidth / 2; i++)
	{
		*(linedata + i) = 0;
		for (int j = 0; j < 8; j++)
		{
			
			*(linedata + i) += db4L[7-j]*store[(2*i+1-(7-j)+m_nWidth)% m_nWidth];
		}
		
	}

	for (int i = m_nWidth / 2; i < m_nWidth; i++)
	{
		*(linedata + i) = 0;
		for (int j = 0; j < 8; j++)
		{
			
			*(linedata + i) += db4H[7-j] * store[(2 * (i-m_nWidth/2) + 1 - (7 - j)+m_nWidth)%m_nWidth];
		}
		
	}
	delete store;
	store = NULL;
}

void CDib::GuassianFilter(CDib *buffer, double sigma)
{
	int size = 6 * sigma + 1;
	double *filter = new double[size];
	double filterval = 0;
	double *rowresult = new double[m_nWidth*m_nHeight];
	memset(rowresult, 0, sizeof(double)*m_nWidth*m_nHeight);
	double sum = 0;

	for (int i = 0; i < size; i++)
	{
		filter[i] = 1 / (sqrt(2 * Pi)*sigma)*exp(-(i - size / 2.0)*(i - size / 2.0) / (2 * sigma*sigma));
		sum += filter[i];
	}	
	for (int i = 0; i < size; i++)
	{
		filter[i] /= sum;
		
	}
	for (int i = 0; i < m_nHeight ; i++)
	{
		for (int j = 0; j < m_nWidth ; j++)
		{
			filterval = 0;
			for (int k = 0; k < size; k++)
			{
				int t = j + k - size / 2;
				
				t += m_nWidth;
				t%=m_nWidth;
				filterval += *(buffer->m_pDibBits + (i)* m_nWidth + t)*filter[k];
			}
				
			*(rowresult + (i)* m_nWidth + j) = filterval;
		}
	}
	
	//�о�� ���Ƚ�filterval��ֵ����buffer
	for (int i = 0; i < m_nWidth ; i++)
	{
		for (int j = 0; j < m_nHeight ; j++)
		{
			filterval = 0;
			for (int k = 0; k < size; k++)
			{
				int t = j + k - size / 2;
				
				t += m_nHeight;
				
				t %= m_nHeight;
				filterval += *(rowresult + (t)* m_nWidth + i)*filter[k];
			}
							
			*(m_pDibBits + (j)* m_nWidth + i) = filterval;
		}
	}

	delete filter;
	filter = NULL;
	delete rowresult;
	rowresult = NULL;

}


void CDib::non_maxsuppresion(CDib*buffer)
{
	BYTE *temp1 = new BYTE[m_nHeight*m_nWidth];//�洢�ݶ�ֵ
	memset(temp1, 0, m_nHeight*m_nWidth);
	BYTE *temp2 = new BYTE[m_nHeight*m_nWidth];//�洢�ݶȽ�
	memset(temp2, 0, m_nHeight*m_nWidth);
	memset(m_pDibBits, 0, m_nHeight*m_nWidth);//�����ʾ����
	for (int i = 1; i < m_nHeight - 1; i++)
	{
		for (int j = 1; j < m_nWidth - 1; j++)
		{
			int filterval_temp[9];
			filterval_temp[0] = *(buffer->m_pDibBits + (i - 1) * m_nWidth + j - 1);
			filterval_temp[1] = *(buffer->m_pDibBits + (i - 1) * m_nWidth + j);
			filterval_temp[2] = *(buffer->m_pDibBits + (i - 1) * m_nWidth + j + 1);
			filterval_temp[3] = *(buffer->m_pDibBits + (i)* m_nWidth + j - 1);
			filterval_temp[4] = *(buffer->m_pDibBits + (i)* m_nWidth + j);
			filterval_temp[5] = *(buffer->m_pDibBits + (i)* m_nWidth + j + 1);
			filterval_temp[6] = *(buffer->m_pDibBits + (i + 1)* m_nWidth + j - 1);
			filterval_temp[7] = *(buffer->m_pDibBits + (i + 1)* m_nWidth + j);
			filterval_temp[8] = *(buffer->m_pDibBits + (i + 1)* m_nWidth + j + 1);
			////////////////////////////////////////////////////////////////////////
			double xdir, ydir,xdira,ydira,gradient,angle;
			xdir = ydir = xdira = ydira =angle= 0;//special case:xdira=ydira=0, angle Ĭ��Ϊ0
			for (int m = 0; m < 9; m++)
			{
				xdir += filterval_temp[m] * gx[m];
				ydir += filterval_temp[m] * gy[m];
			}
			xdira = abs(xdir);
			ydira = abs(ydir);
			
			//gradient = xdira+ydira; 
			gradient = sqrt(xdira*xdira + ydira * ydira);
			temp1[(i)* m_nWidth + j] = (gradient>255)?255:gradient;
			///////////////////////////////////////////////////////////////////
			if ( ydira / xdira < tan(22.5*Pi / 180))
			{
				angle = 0;
			}
			else if (ydira / xdira>=tan(22.5*Pi / 180)&& ydira / xdira < tan(67.55*Pi / 180)&&xdir*ydir>=0)
			{
				angle = 1;
			}
			else if (ydira / xdira>= tan(22.5*Pi / 180) && ydira / xdira < tan(67.55*Pi / 180) && xdir*ydir<0)
			{
				angle = 2;
			}
			else  //(ydira / xdira >= tan(67.55*Pi / 180) )
			{
				angle = 3;
			}
			temp2[(i)* m_nWidth + j] = angle;			
		}
	}
	///////////////////////////////////////////////
	for (int i = 1; i < m_nHeight - 1; i++)
	{
		for (int j = 1; j < m_nWidth - 1; j++)
		{
			int gradient_temp[9]; //���Ƚ��ݶ�����
			gradient_temp[0] = temp1[(i - 1) * m_nWidth + j - 1];
			gradient_temp[1] = temp1[(i - 1) * m_nWidth + j];
			gradient_temp[2] = temp1[(i - 1) * m_nWidth + j + 1];
			gradient_temp[3] = temp1[(i)* m_nWidth + j - 1];
			gradient_temp[4] = temp1[(i)* m_nWidth + j];
			gradient_temp[5] = temp1[(i)* m_nWidth + j + 1];
			gradient_temp[6] = temp1[(i + 1)* m_nWidth + j - 1];
			gradient_temp[7] = temp1[(i + 1)* m_nWidth + j];
			gradient_temp[8] = temp1[(i + 1)* m_nWidth + j + 1];
			if (temp2[(i)* m_nWidth + j] == 0)
			{
				if (gradient_temp[4] > gradient_temp[1] && gradient_temp[4] > gradient_temp[7])
					m_pDibBits[(i)* m_nWidth + j] = gradient_temp[4];
			}
			if (temp2[(i)* m_nWidth + j] == 1)
			{
				if (gradient_temp[4] > gradient_temp[0] && gradient_temp[4] > gradient_temp[8])
					m_pDibBits[(i)* m_nWidth + j] = gradient_temp[4];
			}
			if (temp2[(i)* m_nWidth + j] == 2)
			{
				if (gradient_temp[4] > gradient_temp[2] && gradient_temp[4] > gradient_temp[6])
					m_pDibBits[(i)* m_nWidth + j] = gradient_temp[4];
			}
			if (temp2[(i)* m_nWidth + j] == 3)
			{
				if (gradient_temp[4] >= gradient_temp[3] && gradient_temp[4] > gradient_temp[5])
					m_pDibBits[(i)* m_nWidth + j] = gradient_temp[4];
			}
		}
	}
	delete temp1;
	delete temp2;
	temp2 = NULL;
	temp1 = NULL;
}


void CDib::bithreshhoding()
{
	BYTE max = 0;
	for (int i = 0; i < m_nHeight*m_nWidth; i++)
		max = (max < m_pDibBits[i])? m_pDibBits[i] : max;
	double th1 = 0.1*max;
	double th2 = 0.2*max;
	BYTE *edge = new BYTE[m_nHeight*m_nWidth];//�洢edge pixel
	memset(edge, 0, m_nHeight*m_nWidth);
	
	for (int i = 1; i < m_nHeight - 1; i++)
	{
		for (int j = 1; j < m_nWidth - 1; j++)
		{
			BYTE mytemp = 0;
			if (m_pDibBits[(i)* m_nWidth + j] > th2)
			{
				edge[(i)* m_nWidth + j] = 255;
				for (int m = -1; m < 2; m++)
				{
					for (int n = -1; n < 2; n++)
					{
						mytemp = m_pDibBits[(i + m) * m_nWidth + j + n];
						if (mytemp>th1)
							edge[(i + m) * m_nWidth + j + n] = 255;
					}
				}
			}			
		}
	}
	memset(m_pDibBits, 0, m_nHeight*m_nWidth);
	memcpy(m_pDibBits, edge, m_nHeight*m_nWidth);
	delete edge;
	edge = NULL;
}