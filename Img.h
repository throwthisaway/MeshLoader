#pragma once
#include <memory>
namespace Img
{
	#define ID_IMG_OK ID_OK
	#define ID_IMG_UBPP 0x3006
	typedef enum {PF_UNKNOWN, PF_GREYSCALE8, PF_RGB, PF_RGBA, PF_BGR, PF_BGRA}PIXELFORMAT;
	
	struct ImgData {
		std::unique_ptr<uint8_t> data;
		unsigned int width, height;
		unsigned char bpp;
		size_t GetSize() const { return width * height * (bpp >> 3); }
		PIXELFORMAT pf;
		void ChangeComponentOrder();
	};

	class CImg
	{
	public:
		virtual ImgData& GetImage(void) = 0;
		virtual int Load(const char * fname) = 0;
		virtual void Cleanup(void) = 0;
		static int PixelFormat(int bpp, PIXELFORMAT& pf);
		CImg(void);
		virtual ~CImg(void);
	};
}
