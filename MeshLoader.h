#pragma once
#include "types.h"
namespace MeshLoader {
	void LoadChunk(index_t tag, index_t size, index_t count, uint8_t* data, Mesh& mesh);
	void LoadMesh(const uint8_t* data, size_t len, Mesh& mesh);
}
