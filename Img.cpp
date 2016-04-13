#include "Img.h"
#include "Log.h"
namespace Img
{
	void ImgData::ChangeComponentOrder() {
		if (bpp<24) return;
		auto inc = (bpp == 24) ? 3 : 4;
		// BGR<->RGB, BGRA<->RGBA
		for (size_t i = 0; i < data.size(); i += inc)
			std::swap(data[i], data[i + 2]);
		if (bpp == 24) pf = (pf == PF_BGR) ? PF_RGB : PF_BGR;
		if (bpp == 32) pf = (pf == PF_BGRA) ? PF_RGBA : PF_BGRA;
	}

	CImg::CImg(void)
	{
	}

	CImg::~CImg(void)
	{
	}
	int CImg::PixelFormat(int bpp, PIXELFORMAT& pf)
	{
			switch (bpp)
			{
			case 24:
				pf = PF_BGR;
				break;
			case 32:
				pf = PF_BGRA;
				break;
			case 8:
				pf = PF_GREYSCALE8;
				break;
			default:
				Log::CLog::Write(__T("CImg: Unhandled bit depth...\r\n"));
				return ID_IMG_UBPP;
			}
			return ID_IMG_OK;
	}
}