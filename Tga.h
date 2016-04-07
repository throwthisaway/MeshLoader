#pragma once
#include <stdio.h>
#include "FileReader.h"
#include "Img.h"

namespace Img
{
	//////////////////////////////////////////////////////////////////////
	// TGA Loader
	//////////////////////////////////////////////////////////////////////
	#define ID_TGA_OK ID_IMG_OK
	#define ID_TGA_FNAM 0x3001
	#define ID_TGA_FLEN 0x3002
	#define ID_TGA_FFMT 0x3003
	#define ID_TGA_AMEM 0x3004
	#define ID_TGA_UDAT 0x3005

	const unsigned char compressedTGA[12]={0,0,10,0,0,0,0,0,0,0,0,0};
	const unsigned char uncompressedTGA[12]={0,0,2,0,0,0,0,0,0,0,0,0};

	static int error=0;
	//stTga * LoadTGA(const char * fname);

	//stTga * LoadcTGA(FILE * f);
	//stTga * LoaduTGA(FILE * f);
	class CTga : CImg 
	{
		IO::CFileReader * _fr;
		ImgData image;
		int LoaduTGA(void);
		int LoadcTGA(void);
		int ParseHeader(void);

	public:
		virtual ImgData& GetImage(void);
		virtual int Load(const char * fname);
		virtual void Cleanup(void);
		CTga(void);
		virtual ~CTga(void);

	};
}

