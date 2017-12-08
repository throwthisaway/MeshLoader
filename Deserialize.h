#pragma once
#include <stdint.h>
#define SERIALIZER public: void Relocate(size_t, void *);
#define PTR(X, x)	union { X x; int64_t _x; };

namespace MeshLoader {
	template<typename T> constexpr T AlignTo(T val, uint8_t alignment) { return (val + alignment - 1) & ~(alignment - 1); }
	using tag_t = uint32_t;
	using index_t = uint32_t;
	using poly_t = uint16_t;
	constexpr uint8_t alignment = sizeof(size_t);
	struct Header {
		uint32_t magicNumber, version;
		uint8_t alignment, tagSize, indexSize/*size for indexing into arrays*/, polySize /*polygon indices*/;
	};
	struct Chunk {
		tag_t tag;
		index_t size, count;
	};
}
#define _TAG MeshLoader::tag_t tag;
