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
	m_pDibBits = NULL; // 指针
	if (m_pGrayValueCount != NULL)  //动态分配内存
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
	//注意上一行getpitch的结果是负的
	int color_channel = GetMaxColorTableEntries();//The RGBQUAD structure describes a color consisting of relative intensities of red, green, and blue
	if (color_channel)  //color_channel 就是n_colors
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
	//ifstream out("C:\\Users\\Shouhua Luo\\Desktop\\课件\\数字图像处理\\20-21-03学期医学图像处理\\数据\\shepp-logan.txt");
	if (out.is_open())
	{
		////获取数据高度和宽度
		//out.seekg(0, ios::beg);
		getline(out, m);
		int linewords = m.length();
		out.seekg(0, ios::end);
		int totalwords = out.tellg();
		int height = totalwords / linewords;
		long width = m.length() / strcount;

		////申请一块内存存放读入的浮点数
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

		////把数据转换到显示缓存
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
//小白块 窗 sinc 一个点对应一片白

void CDib::Invert()
{
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)  //对于lena m_nWidthBytes 就是m_nHeight 512*512,没有缓冲区
		{
			*(m_pDibBits + i*m_nWidthBytes + j) = (unsigned char)(255 - *(m_pDibBits + i*m_nWidthBytes + j));//指针加一，加的是sizeof(指针所指对象)
		}
	}
}
//m_pDibBits指向像素数组物理地址的第一行第一列，实际对应图像的最后一行第一列，最后一行处理完之后，回到倒数第二行从第一列开始处理

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
	m_pGrayValueCount = GrayValueCount(); //用已经分配好的m_pGrayValueCount，否则产生野指针，因为在grayvaluecount函数中没有delete新分配的数组
	long nColors = GetMaxColorTableEntries(); // long也是整数类型变量
	double *grayvalue_double = new double[nColors];  //存贮归一化后的直方图
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
				if (type == 1) //均值
				{
					for (int m = 0; m < 9; m++)
					{
						filtered += ((double)filterval_temp[m] * box[m]);
					}
					*(m_pDibBits + i * m_nWidth + j) = filtered / 9;
				}
				else if (type == 2) //中值
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
void CDib::colorcircle()//彩色图像没有调色盘
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
				RGBTRIPLE *pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);  //强制转换
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
	memset(m_pDibBits, 0, m_nHeight*m_nWidthBytes);//异常关键
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

	}//inital 里是边界图像
	memcpy(m_pDibBits, buffer->m_pDibBits, m_nHeight*m_nWidthBytes);
	for (int i = 0; i < m_nHeight ; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			//RGBTRIPLE *tempRGB = (RGBTRIPLE *)&inital[i *m_nWidthBytes + j * 3];//这一步非常精髓
			RGBTRIPLE *tempRGB = (RGBTRIPLE *)(inital + i * m_nWidthBytes + j * 3);//和注释掉的行是等价的
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
	unsigned char*	lpSrc;							// 指向源图像的指针
	int y;										// 循环控制变量
	int x;										// 循环控制变量
	double dTmpOne;								//存放临时变量
	double dTmpTwo;								//存放临时变量
	int nTransWidth;								// 傅立叶变换的宽度（2的整数次幂）
	int nTransHeight;								// 傅立叶变换的高度（2的整数次幂）
	double unchValue;								// 存贮图像各像素灰度的临时变量
	complex<double> * pCTData;						// 指向时域数据的指针
	complex<double> * pCFData;						// 指向频域数据的指针
	// 计算进行傅立叶变换的点数－横向	（2的整数次幂）
	dTmpOne = log(1.0*m_nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// 计算进行傅立叶变换的点数－纵向 （2的整数次幂）
	dTmpOne = log(1.0* m_nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	double dReal;									// 傅立叶变换的实部
	double dImag;									// 傅立叶变换的虚部

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	// 图像数据的宽和高不一定是2的整数次幂，所以pCTData有一部分数据需要补0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y*nTransWidth + x] = complex<double>(0, 0);		// 补零
		}
	}
	//把图像数据传给pCTData
	for (y = 0; y < m_nHeight; y++)
	{
		for (x = 0; x < m_nWidth; x++)
		{
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)m_pDibBits + m_nWidth * (m_nHeight - 1 - y) + x;
			unchValue = (*lpSrc)*pow(-1.0, x + y);
			pCTData[y*nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData,m_nWidth, m_nHeight, pCFData);				// 傅立叶正变换


	for (y = 0; y < m_nHeight; y++)								// 反变换的数据传给lpDIBBits
	{
		for (x = 0; x < m_nWidth; x++)
		{
			//需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
			dReal = pCFData[y*nTransWidth + x].real();		// 实部
			dImag = pCFData[y*nTransWidth + x].imag();		// 虚部
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
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)m_pDibBits + m_nWidth * (m_nHeight - 1 - y) + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete pCTData;										// 释放内存
	delete pCFData;										// 释放内存
	pCTData = NULL;
	pCFData = NULL;
	// 返回结果
}

BOOL CDib::FFT(unsigned char* pDIBBits, long nWidth, long nHeight)
{ 
	unsigned char*	lpSrc;							// 指向源图像的指针
	int y ;										// 循环控制变量
	int x ;										// 循环控制变量
	double dTmpOne ;								//存放临时变量
	double dTmpTwo ;								//存放临时变量
	int nTransWidth ;								// 傅立叶变换的宽度（2的整数次幂）
	int nTransHeight;								// 傅立叶变换的高度（2的整数次幂）
	double unchValue;								// 存贮图像各像素灰度的临时变量
	complex<double> * pCTData ;						// 指向时域数据的指针
	complex<double> * pCFData ;						// 指向频域数据的指针
	// 计算进行傅立叶变换的点数－横向	（2的整数次幂）
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)	;
	dTmpTwo = pow(2,dTmpTwo);
	nTransWidth = (int) dTmpTwo;	
	// 计算进行傅立叶变换的点数－纵向 （2的整数次幂）
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)	;
	dTmpTwo = pow(2,dTmpTwo);
	nTransHeight = (int) dTmpTwo;
	double dReal;									// 傅立叶变换的实部
	double dImag;									// 傅立叶变换的虚部
	
	pCTData=new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	pCFData=new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	// 图像数据的宽和高不一定是2的整数次幂，所以pCTData有一部分数据需要补0
	for(y=0; y<nTransHeight; y++)
	{
		for(x=0; x<nTransWidth; x++)
		{
			pCTData[y*nTransWidth + x]=complex<double>(0,0);		// 补零
		}
	}
	//把图像数据传给pCTData
	for(y=0; y<nHeight; y++)
	{
		for(x=0; x<nWidth; x++)
		{
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc)*pow(-1.0,x+y);
			pCTData[y*nTransWidth + x]=complex<double>(unchValue,0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData) ;				// 傅立叶正变换

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	//
	//
	//
	//
	///////////////////////////////////////////////////////////////////////////

	IFFT_2D(pCFData, pCTData,nHeight, nWidth); 				// 傅立叶反变换

	for(y=0; y<nHeight; y++)								// 反变换的数据传给lpDIBBits
	{
		for(x=0; x<nWidth; x++)
		{
			//需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
			dReal = pCTData[y*nTransWidth + x].real() ;		// 实部
			dImag = pCTData[y*nTransWidth + x].imag() ;		// 虚部
			unchValue = dReal*pow(-1.0,x+y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc =(BYTE)unchValue ;
		}
	}	
	
	delete pCTData;										// 释放内存
	delete pCFData;										// 释放内存
	pCTData = NULL;
	pCFData = NULL;	
	return (true);										//返回结果
}

/*************************************************************************
 *
 * \函数名称：
 *   FFT_1D()
 *
 * \输入参数:
 *   complex<double> * pCTData	- 指向时域数据的指针，输入的需要变换的数据
 *   complex<double> * pCFData	- 指向频域数据的指针，输出的经过变换的数据
 *   nLevel						－傅立叶变换蝶形算法的级数，2的幂数，
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   一维快速傅立叶变换。
 *
 *************************************************************************
 */
void CDib::FFT_1D(complex<double> * pCTData, complex<double> * pCFData, int nLevel)
{
		// 循环控制变量
	int		i;
	int     j;
	int     k;

	// 傅立叶变换点数
	int	nCount =0 ;

	// 计算傅立叶变换点数
	nCount =(int)pow(2.0,nLevel) ;
	
	// 某一级的长度
	int		nBtFlyLen;
	nBtFlyLen = 0 ;
	
	// 变换系数的角度 ＝2 * PI * i / nCount
	double	dAngle;
	
	complex<double> *pCW ;
	
	// 分配内存，存储傅立叶变化需要的系数表
	pCW  = new complex<double>[nCount / 2];

    // 计算傅立叶变换的系数
	for(i = 0; i < nCount / 2; i++)
	{
		dAngle = -2 * Pi * i / nCount;
		pCW[i] = complex<double> ( cos(dAngle), sin(dAngle) );
	}

	// 变换需要的工作空间
	complex<double> *pCWork1,*pCWork2; 
	
	// 分配工作空间
	pCWork1 = new complex<double>[nCount];

	pCWork2 = new complex<double>[nCount];

	
	// 临时变量
	complex<double> *pCTmp;
	
	// 初始化，写入数据
	memcpy(pCWork1, pCTData, sizeof(complex<double>) * nCount);

	// 临时变量
	int nInter; 
	nInter = 0;

	// 蝶形算法进行快速傅立叶变换
	for(k = 0; k < nLevel; k++)
	{
		for(j = 0; j < (int)pow(2.0,k); j++)
		{
			//计算长度
			nBtFlyLen = (int)pow( 2.0,(nLevel-k) );
			
			//倒序重排，加权计算
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

		// 交换 pCWork1和pCWork2的数据
		pCTmp   = pCWork1	;
		pCWork1 = pCWork2	;
		pCWork2 = pCTmp		;
	}
	
	// 重新排序
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
	
	// 释放内存空间
	delete pCW;
	delete pCWork1;
	delete pCWork2;
	pCW		=	NULL	;
	pCWork1 =	NULL	;
	pCWork2 =	NULL	;

}
/*************************************************************************
 *
 * \函数名称：
 *    IFFT_1D()
 *
 * \输入参数:
 *   complex<double> * pCTData	- 指向时域数据的指针，输入的需要反变换的数据
 *   complex<double> * pCFData	- 指向频域数据的指针，输出的经过反变换的数据
 *   nLevel						－傅立叶变换蝶形算法的级数，2的幂数，
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   一维快速傅立叶反变换。
 *
 ************************************************************************
 */
void CDib::IFFT_1D(complex<double> * pCFData, complex<double> * pCTData, int nLevel)
{
	
	// 循环控制变量
	int		i;

	// 傅立叶反变换点数
	int nCount;

	// 计算傅立叶变换点数
	nCount = (int)pow(2.0,nLevel) ;
	
	// 变换需要的工作空间
	complex<double> *pCWork;	
	
	// 分配工作空间
	pCWork = new complex<double>[nCount];
	
	// 将需要反变换的数据写入工作空间pCWork
	memcpy(pCWork, pCFData, sizeof(complex<double>) * nCount);
	
	// 为了利用傅立叶正变换,可以把傅立叶频域的数据取共轭
	// 然后直接利用正变换，输出结果就是傅立叶反变换结果的共轭
	for(i = 0; i < nCount; i++)
	{
		pCWork[i] = complex<double> (pCWork[i].real(), -pCWork[i].imag());
	}
	
	// 调用快速傅立叶变换实现反变换，结果存储在pCTData中
	FFT_1D(pCWork, pCTData, nLevel);
	
	// 求时域点的共轭，求得最终结果
	// 根据傅立叶变换原理，利用这样的方法求得的结果和实际的时域数据
	// 相差一个系数nCount
	for(i = 0; i < nCount; i++)
	{
		pCTData[i] = 
			complex<double> (pCTData[i].real() / nCount, -pCTData[i].imag() / nCount);
	}
	
	// 释放内存
	delete pCWork;
	pCWork = NULL;
}

/*************************************************************************
 *
 * \函数名称：
 *   FFT_2D()
 *
 * \输入参数:
 *   complex<double> * pCTData	- 图像数据
 *   int    nWidth				- 数据宽度
 *   int    nHeight				- 数据高度
 *   complex<double> * pCFData	- 傅立叶变换后的结果
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   二维傅立叶变换。
 *
 ************************************************************************
 */
void CDib::FFT_2D(complex<double> * pCTData, int nWidth, int nHeight, complex<double> * pCFData)
{
	
	// 循环控制变量
	int	x;
	int	y;
	
	// 临时变量
	double	dTmpOne;
	double  dTmpTwo;
	
	// 进行傅立叶变换的宽度和高度，（2的整数次幂）
	// 图像的宽度和高度不一定为2的整数次幂
	int		nTransWidth;
	int 	nTransHeight;

	// 计算进行傅立叶变换的宽度	（2的整数次幂）
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransWidth = (int) dTmpTwo	   ;
	
	// 计算进行傅立叶变换的高度 （2的整数次幂）
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransHeight = (int) dTmpTwo	   ;	
	
	// x，y（行列）方向上的迭代次数
	int		nXLev;
	int		nYLev;

	// 计算x，y（行列）方向上的迭代次数
	nXLev = (int) ( log(1.0*nTransWidth)/log(2.0) +  0.5 );
	nYLev = (int) ( log(1.0*nTransHeight)/log(2.0) + 0.5 );
	
	for(y = 0; y < nTransHeight; y++)
	{
		// x方向进行快速傅立叶变换
		FFT_1D(&pCTData[nTransWidth * y], &pCFData[nTransWidth * y], nXLev);
	}
	
	// pCFData中目前存储了pCTData经过行变换的结果
	// 为了直接利用FFT_1D，需要把pCFData的二维数据转置，再一次利用FFT_1D进行
	// 傅立叶行变换（实际上相当于对列进行傅立叶变换）
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTData[nTransHeight * x + y] = pCFData[nTransWidth * y + x];
		}
	}
	
	for(x = 0; x < nTransWidth; x++)
	{
		// 对x方向进行快速傅立叶变换，实际上相当于对原来的图象数据进行列方向的
		// 傅立叶变换
		FFT_1D(&pCTData[x * nTransHeight], &pCFData[x * nTransHeight], nYLev);
	}

	// pCFData中目前存储了pCTData经过二维傅立叶变换的结果，但是为了方便列方向
	// 的傅立叶变换，对其进行了转置，现在把结果转置回来
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
 * \函数名称：
 *   IFFT_2D()
 *
 * \输入参数:
 *   complex<double> * pCFData	- 频域数据
 *   complex<double> * pCTData	- 时域数据
 *   int    nWidth				- 图像数据宽度
 *   int    nHeight				- 图像数据高度
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   二维傅立叶反变换。
 *
 ************************************************************************
 */
void CDib::IFFT_2D(complex<double> * pCFData, complex<double> * pCTData, int nWidth, int nHeight)
{
	// 循环控制变量
	int	x;
	int	y;
	
	// 临时变量
	double	dTmpOne;
	double  dTmpTwo;
	
	// 进行傅立叶变换的宽度和高度，（2的整数次幂）
	// 图像的宽度和高度不一定为2的整数次幂
	int		nTransWidth;
	int 	nTransHeight;

	// 计算进行傅立叶变换的宽度	（2的整数次幂）
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransWidth = (int) dTmpTwo	   ;
	
	// 计算进行傅立叶变换的高度 （2的整数次幂）
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransHeight = (int) dTmpTwo	   ;
	
	// 分配工作需要的内存空间
	complex<double> *pCWork= new complex<double>[nTransWidth * nTransHeight];

	//临时变量
	complex<double> *pCTmp ;
	
	// 为了利用傅立叶正变换,可以把傅立叶频域的数据取共轭
	// 然后直接利用正变换，输出结果就是傅立叶反变换结果的共轭
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTmp = &pCFData[nTransWidth * y + x] ;
			pCWork[nTransWidth * y + x] = complex<double>( pCTmp->real() , -pCTmp->imag() );
		}
	}

	// 调用傅立叶正变换
	FFT_2D(pCWork, nWidth, nHeight, pCTData) ;
	
	// 求时域点的共轭，求得最终结果
	// 根据傅立叶变换原理，利用这样的方法求得的结果和实际的时域数据
	// 相差一个系数
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
	unsigned char*	lpSrc;							// 指向源图像的指针
	int y;										// 循环控制变量
	int x;										// 循环控制变量
	double dTmpOne;								//存放临时变量
	double dTmpTwo;								//存放临时变量
	int nTransWidth;								// 傅立叶变换的宽度（2的整数次幂）
	int nTransHeight;								// 傅立叶变换的高度（2的整数次幂）
	double unchValue;								// 存贮图像各像素灰度的临时变量
	complex<double> * pCTData;						// 指向时域数据的指针
	complex<double> * pCFData;						// 指向频域数据的指针
	// 计算进行傅立叶变换的点数－横向	（2的整数次幂）
	dTmpOne = log(1.0*nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// 计算进行傅立叶变换的点数－纵向 （2的整数次幂）
	dTmpOne = log(1.0*nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;

	double dReal;									// 傅立叶变换的实部
	double dImag;									// 傅立叶变换的虚部

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	// 图像数据的宽和高不一定是2的整数次幂，所以pCTData有一部分数据需要补0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y*nTransWidth + x] = complex<double>(0, 0);		// 补零
		}
	}
	//把图像数据传给pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)m_pDibBits + nWidth * y + x; // here uses m_pDibBits
			unchValue = (*lpSrc)*pow(-1.0, x + y);
			pCTData[y*nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData);				// 傅立叶正变换

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
				H[y*nTransWidth + x] = 1 / (1 + pow(sqrt(pow((y - nTransHeight / 2), 2) + pow((x - nTransWidth / 2), 2)) / D0, 10));  //buttorworth 采用书上的做法，先做一个低通
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
				pCFData[y*nTransWidth + x] *= weiner; // 实部乘实部，虚部乘虚部和直接乘的效果是一样的

			}
		}
	delete[]H;
    }


	IFFT_2D(pCFData, pCTData, nHeight, nWidth); 				// 傅立叶反变换

	for (y = 0; y < nHeight; y++)								// 反变换的数据传给lpDIBBits
	{
		for (x = 0; x < nWidth; x++)
		{
			//需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
			dReal = pCTData[y*nTransWidth + x].real();		// 实部
			dImag = pCTData[y*nTransWidth + x].imag();		// 虚部
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)m_pDibBits + nWidth * y + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete pCTData;										// 释放内存
	delete pCFData;										// 释放内存
	pCTData = NULL;
	pCFData = NULL;
	// 返回结果
}

void CDib::filter_freq_domain_padded(int D0, int type) //
// D0： cutoff frequency   type =1,2,3,4,5,6
//means ideal low pass ,gaussian low pass, butterworth low pass, and high pass 
{
	// in the FFT function provided by supplemantory , paras are
	// (unsigned char* pDIBBits, long nWidth, long nHeight) 
	// for convenience, use the same name in this filtering code
	long nWidth = m_nWidth*2;
	long nHeight = m_nHeight*2;
	unsigned char*	lpSrc;							// 指向源图像的指针
	int y;										// 循环控制变量
	int x;										// 循环控制变量
	double dTmpOne;								//存放临时变量
	double dTmpTwo;								//存放临时变量
	int nTransWidth;								// 傅立叶变换的宽度（2的整数次幂）
	int nTransHeight;								// 傅立叶变换的高度（2的整数次幂）
	double unchValue;								// 存贮图像各像素灰度的临时变量
	complex<double> * pCTData;						// 指向时域数据的指针
	complex<double> * pCFData;						// 指向频域数据的指针
	// 计算进行傅立叶变换的点数－横向	（2的整数次幂）
	dTmpOne = log(1.0*nWidth) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;
	// 计算进行傅立叶变换的点数－纵向 （2的整数次幂）
	dTmpOne = log(1.0*nHeight) / log(2.0);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;

	double dReal;									// 傅立叶变换的实部
	double dImag;									// 傅立叶变换的虚部

	pCTData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	pCFData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	// 图像数据的宽和高不一定是2的整数次幂，所以pCTData有一部分数据需要补0
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[y*nTransWidth + x] = complex<double>(0, 0);		// 补零
		}
	}
	//把图像数据传给pCTData
	for (y = 0; y < m_nHeight; y++)  // main difference between unpadded code
	{
		for (x = 0; x < m_nWidth; x++)
		{
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)m_pDibBits + m_nWidth * y + x; // here uses m_pDibBits
			
			unchValue = (*lpSrc)*pow(-1.0, x + y);
			pCTData[y*nTransWidth + x] = complex<double>(unchValue, 0);
		}
	}
	



	FFT_2D(pCTData, nWidth, nHeight, pCFData);				// 傅立叶正变换

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
	IFFT_2D(pCFData, pCTData, nHeight, nWidth); 				// 傅立叶反变换

	for (y = 0; y < m_nHeight; y++)								// 反变换的数据传给lpDIBBits
	{
		for (x = 0; x <m_nWidth; x++)
		{
			//需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
			dReal = pCTData[y*nTransWidth + x].real();		// 实部
			dImag = pCTData[y*nTransWidth + x].imag();		// 虚部
			unchValue = dReal * pow(-1.0, x + y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)m_pDibBits + m_nWidth * y + x;
			*lpSrc = (BYTE)unchValue;
		}
	}

	delete pCTData;										// 释放内存
	delete pCFData;										// 释放内存
	pCTData = NULL;
	pCFData = NULL;
	// 返回结果
}


//it is not safe to assume that pitch is simply the width multiplied by the number of bytes per pixel
//sometimes more cache allocated
//矩阵不需要转置也可以一列一列访问


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

		//加过滤波器后的数据放回buffer的原位置
		for (int lineY = 0; lineY < m_nHeight; lineY++)
		{
			filterdata[lineY*m_nWidth + lineX] = wTData[lineY].real();
		}
	}

	int m_nWidth = 256;
	int m_nHeight = 256; //phantom256 获得投影数据 重建图像的大小任意,可修改,最好为偶数，pitch和width相同

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
		for (int j = 0; j < m_nWidthBytes; j++)  //由于bpp = 8;j就是纵坐标   m_nWidthBytes = 368  m_nHeight = 367
		{
			for (int k = 0; k < num_angles; k++)
			{
				double t = (i - m_nHeight / 2)*cos(k*Pi/num_angles) + (j - m_nHeight / 2)*sin(k*Pi / num_angles)+num_t/2;
				if (t >= 0 && t < num_t)
				{
					int t1 = floor(t);
					int t2 = ceil(t);
					double ratio = (t - t1);
					recon[j*m_nWidthBytes + i] += (filterdata[k + t1 * num_angles]*ratio+ filterdata[k + t2 * num_angles]*(1-ratio));		//线性插值,此处有转置赋值			
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
	long pastWidth = GetWidth();//原图尺寸
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
	double pastx;    //双线性插值
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
	//同一类型的变量可以使用memcpy 
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
	//分块归一化 这里没做
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
//用消失矩为4的db小波，注意使用double 进行运算，开辟新工作区，因为char只能表示正数，用double得到的值进行归一化，再转到char里，非常关键
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
	
	//列卷积 首先将filterval的值赋给buffer
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
	BYTE *temp1 = new BYTE[m_nHeight*m_nWidth];//存储梯度值
	memset(temp1, 0, m_nHeight*m_nWidth);
	BYTE *temp2 = new BYTE[m_nHeight*m_nWidth];//存储梯度角
	memset(temp2, 0, m_nHeight*m_nWidth);
	memset(m_pDibBits, 0, m_nHeight*m_nWidth);//清空显示缓存
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
			xdir = ydir = xdira = ydira =angle= 0;//special case:xdira=ydira=0, angle 默认为0
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
			int gradient_temp[9]; //待比较梯度数据
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
	BYTE *edge = new BYTE[m_nHeight*m_nWidth];//存储edge pixel
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