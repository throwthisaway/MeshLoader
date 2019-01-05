#pragma once
#include <memory>
namespace Img {
	#define ID_IMG_OK ID_OK
	enum class PixelFormat : uint8_t {Unknown, Greyscale8, RGB8, RGBA8, BGR8, BGRA8, RGBAF16, RGBAF32};
	uint8_t BytesPerPixel(PixelFormat pf);

	struct ImgData {
		std::unique_ptr<uint8_t> data;
		uint16_t width, height;
		uint8_t bytesPerPixel;
		PixelFormat pf;
		inline size_t CalcSize() const { return width * height * bytesPerPixel; }
		void ChangeComponentOrder();
	};

	class CImg {
	public:
		virtual ImgData& GetImage(void) = 0;
		virtual int Load(const char * fname) = 0;
		virtual void Cleanup(void) = 0;
	};
}
