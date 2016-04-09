#pragma once

namespace Img
{
	#define ID_IMG_OK ID_OK
	#define ID_IMG_UBPP 0x3006
	typedef enum {PF_UNKNOWN, PF_GREYSCALE8, PF_RGB, PF_RGBA, PF_BGR, PF_BGRA}PIXELFORMAT;
	
	struct ImgData
	{
		unsigned char * data;
		unsigned int size;
		unsigned int width,height;
		unsigned char bpp;
		PIXELFORMAT pf;
		void Cleanup() { delete [] data; data = nullptr; };
		ImgData() : data(nullptr) {};
		~ImgData() { Cleanup(); };
	};

	class CImg
	{
	public:
		virtual ImgData& GetImage(void) = 0;
		virtual int Load(const char * fname) = 0;
		virtual void Cleanup(void) = 0;
		int PixelFormat(int bpp, PIXELFORMAT& pf);
		CImg(void);
		virtual ~CImg(void);
	};
}
