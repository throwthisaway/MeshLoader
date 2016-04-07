#include "Img.h"
#include "Log.h"
namespace Img
{
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
				Log::CLog::Write(_T("CImg: Unhandled bit depth...\r\n"));
				return ID_IMG_UBPP;
			}
			return ID_IMG_OK;
	}
}