#include "Img.h"
#include "Log.h"
namespace Img {
	void ImgData::ChangeComponentOrder() {
		if (bytesPerPixel < 3) return;
		// BGR<->RGB, BGRA<->RGBA
		for (size_t i = 0; i < CalcSize(); i += bytesPerPixel)
			std::swap(data.get()[i], data.get()[i + 2]);
		if (bytesPerPixel == 3) pf = (pf == PixelFormat::BGR8) ? PixelFormat::RGB8 : PixelFormat::BGR8;
		if (bytesPerPixel == 4) pf = (pf == PixelFormat::BGRA8) ? PixelFormat::RGBA8 : PixelFormat::BGRA8;
	}
}