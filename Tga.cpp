#include <assert.h>
#include "Tga.h"
#include "Log.h"

namespace Img {
	// http://www.paulbourke.net/dataformats/tga/

	namespace {
		struct Header {
			uint8_t  idlength;
			uint8_t  colourmaptype;
			uint8_t  datatypecode;
			int16_t colourmaporigin;
			int16_t colourmaplength;
			uint8_t  colourmapdepth;
			int16_t x_origin;
			int16_t y_origin;
			uint16_t width;
			uint16_t height;
			uint8_t  bitsperpixel;
			uint8_t  imagedescriptor;
		};

		Img::TgaDecodeResult ParseHeader(const Header& header, ImgData& image) {
			image.width = header.width; image.height = header.height;
			image.bytesPerPixel = header.bitsperpixel >> 3;
			switch (image.bytesPerPixel) {
			case 3:
				image.pf = PixelFormat::RGB8;
				break;
			case 4:
				image.pf = PixelFormat::RGBA8;
				break;
			case 1:
				image.pf = PixelFormat::Greyscale8;
				break;
			default:
				image.pf = PixelFormat::Unknown;
				return Img::TgaDecodeResult::UnsupportedPixelFormat;
			}
			return TgaDecodeResult::Ok;
		}

		Img::TgaDecodeResult LoaduTGA(uint8_t* p, size_t length, ImgData& image, bool expand24To32) {
			const auto size = image.CalcSize();
			if (length - sizeof(Header) < size) {
				Log::CLog::Write("LoaduTGA: Insufficient size...\r\n");
				return Img::TgaDecodeResult::InsufficientSize;
			}
			if (expand24To32 && image.bytesPerPixel == 3) {
				image.bytesPerPixel = 4;
				image.pf = (image.pf == PixelFormat::RGB8) ? PixelFormat::RGBA8 : PixelFormat::BGRA8;
				const auto size = image.CalcSize();
				image.data = std::unique_ptr<uint8_t>(new uint8_t[size]);
				for (size_t i = 0; i < length;) {
					*(image.data.get() + i++) = *(p++);
					*(image.data.get() + i++) = *(p++);
					*(image.data.get() + i++) = *(p++);
					*(image.data.get() + i++) = 255;
				}
			} else {
				image.data = std::unique_ptr<uint8_t>(new uint8_t[size]);
				memcpy(image.data.get(), p + sizeof(Header), size);
			}
			return Img::TgaDecodeResult::Ok;
		}

		Img::TgaDecodeResult LoadcTGA(uint8_t* p, ImgData& image, bool expand24To32) {
			assert(image.bytesPerPixel == 3 || image.bytesPerPixel == 4 || image.bytesPerPixel == 1);
			p += sizeof(Header);
			const auto origBytesPerPixel = image.bytesPerPixel;
			if (expand24To32 && image.bytesPerPixel == 3) {
				image.bytesPerPixel = 4;
				image.pf = (image.pf == PixelFormat::RGB8) ? PixelFormat::RGBA8 : PixelFormat::BGRA8;
			}
			const auto size = image.CalcSize();
			image.data = std::unique_ptr<uint8_t>(new uint8_t[size]);
			auto dst = image.data.get();
			do {
				auto chunkheader = *(p++);
				auto col = dst;
				if (chunkheader & 0x80) {
					switch (origBytesPerPixel) {
					case 4:
						*(dst++) = *(p++);
					case 3:
						*(dst++) = *(p++);
						*(dst++) = *(p++);
					case 1:
						*(dst++) = *(p++);
					}
					if (origBytesPerPixel != image.bytesPerPixel) *(dst++) = 255;
					const size_t n = chunkheader & 0x7f;
					for (size_t i = 1; i < n; ++i) {
						switch (origBytesPerPixel)
						{
						case 4:
							*(dst++) = *(col + 3);
						case 3:
							*(dst++) = *(col + 2);
							*(dst++) = *(col + 1);
						case 1:
							*(dst++) = *col;
						}
						if (origBytesPerPixel != image.bytesPerPixel) *(dst++) = 255;
					}
				}
				else {
					if (origBytesPerPixel != image.bytesPerPixel) {
						for (size_t i = 0; i < chunkheader + 1; ++i) {
							*(dst++) = *(p++);
							*(dst++) = *(p++);
							*(dst++) = *(p++);
							*(dst++) = 255;
						}
					} else {
						auto n = (chunkheader + 1) * origBytesPerPixel;
						memcpy(dst, p, n);
						p += n;
						dst += n;
					}
				}
			} while (image.data.get() + size < dst);
			return Img::TgaDecodeResult::Ok;
		}
	}

	TgaDecodeResult DecodeTGA(uint8_t* p, size_t length, ImgData& image, bool expand24To32) {
		Header *header = (Header*)p;
		TgaDecodeResult result = ParseHeader(*header, image);
		if (result != TgaDecodeResult::Ok) {
			Log::CLog::Write("DecodeTga: Unhandled file format or header error...\r\n");
			return result;
		}
		if (header->datatypecode == 10 /*RLE RGB*/ || header->datatypecode == 11 /*RLE BW*/)
			return LoadcTGA(p, image, expand24To32);
		if (header->datatypecode == 2/*Uncompressed RGB*/ || header->datatypecode == 3/*Uncompressed BW*/)
			return LoaduTGA(p, length, image, expand24To32);
		Log::CLog::Write("DecodeTga: Unsupported file format...\r\n");
		return TgaDecodeResult::UnsupportedDataType;
	}
}
