#pragma once
#include <stdint.h>
#define SERIALIZER public: void Relocate(size_t, void *);
#define PTR(X, x)	union { X x; int64_t _x; };

namespace MeshLoader {
	using tag_t = uint32_t;
	using index_t = uint32_t;
}
#define _TAG MeshLoader::tag_t tag;
