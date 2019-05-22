#include "Img.h"
#include "Log.h"
#include <assert.h>

namespace Img {
	uint8_t BytesPerPixel(PixelFormat pf) {
		switch (pf) {
			case PixelFormat::BGR8:
			case PixelFormat::RGB8:
				return 3;
			case PixelFormat::BGRA8:
			case PixelFormat::RGBA8:
				return 4;
			case PixelFormat::Greyscale8:
				return 1;
			case PixelFormat::RGBAF32:
				return 16;
			case PixelFormat::RGBAF16:
				return 8;
			case PixelFormat::Unknown:
			default:
				assert(false);

		}
		return 0xff;
	}

	ComponentType GetComponentType(PixelFormat pf) {
		switch (pf) {
			case PixelFormat::BGR8:
			case PixelFormat::RGB8:
			case PixelFormat::BGRA8:
			case PixelFormat::RGBA8:
			case PixelFormat::Greyscale8:
				return ComponentType::Byte;
			case PixelFormat::RGBAF32:
				return ComponentType::Float;
			case PixelFormat::RGBAF16:
				return ComponentType::HalfFloat;
			case PixelFormat::Unknown:
			default:
				assert(false);

		}
		return ComponentType::Unknown;
	}
	void ImgData::ChangeComponentOrder() {
		if (bytesPerPixel < 3) return;
		// BGR<->RGB, BGRA<->RGBA
		for (size_t i = 0; i < CalcSize(); i += bytesPerPixel)
			std::swap(data.get()[i], data.get()[i + 2]);
		if (bytesPerPixel == 3) pf = (pf == PixelFormat::BGR8) ? PixelFormat::RGB8 : PixelFormat::BGR8;
		if (bytesPerPixel == 4) pf = (pf == PixelFormat::BGRA8) ? PixelFormat::RGBA8 : PixelFormat::BGRA8;
	}
}
