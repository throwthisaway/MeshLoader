#pragma once
#include "types.h"
namespace MeshLoader {
	struct Chunk {
		index_t tag, size, count;
	};
	void LoadChunk(index_t tag, index_t size, index_t count, uint8_t* data, Mesh& mesh);
	//void LoadMesh(uint8_t* data, size_t len, MeshLoader::Mesh& mesh);
}
