#pragma once
#include "Img.h"
namespace Img {
	enum class TgaDecodeResult { Ok, UnsupportedPixelFormat, UnsupportedDataType, InsufficientSize };
	TgaDecodeResult DecodeTGA(uint8_t* p, size_t length, ImgData& image);
}
