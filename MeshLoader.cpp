#include "pch.h"
#include "MeshLoader.h"
#include <streambuf>
#include <istream>

using namespace MeshLoader;
namespace {
	struct Chunk {
		uint32_t tag, size, count;
	};

	inline void LoadPolygons(uint8_t* ptr, size_t count, Mesh& mesh) {
		assert((size_t)ptr % sizeof(size_t) == 0);
		mesh.polygons = gsl::make_span(reinterpret_cast<Polygon*>(ptr), count);
	}
	inline void LoadLines(uint8_t* ptr, size_t count, Mesh& mesh) {
		assert((size_t)ptr % sizeof(size_t) == 0);
		mesh.lines = gsl::make_span(reinterpret_cast<PolyLine*>(ptr), count);
	}
	inline void LoadVertices(uint8_t* ptr, size_t count, Mesh& mesh) {
		assert((size_t)ptr % sizeof(size_t) == 0);
		mesh.vertices = gsl::make_span(reinterpret_cast<vec3_t*>(ptr), count);
	}
	inline void LoadSurfaces(uint8_t* ptr, size_t size, size_t elements, Mesh& mesh) {
		assert((size_t)ptr % sizeof(size_t) == 0);
		mesh.surfaces = gsl::make_span(reinterpret_cast<Surface*>(ptr), elements);
		for (size_t i = 0; i < elements; ++i)
			mesh.surfaces[i].Relocate(size, mesh.surfaces.data());
	}
	inline void LoadUVs(uint8_t* ptr, size_t size, size_t elements, Mesh& mesh) {
		assert((size_t)ptr % sizeof(size_t) == 0);
		mesh.uvs.count = elements;
		mesh.uvs.uv = reinterpret_cast<_UVMap*>(ptr);
		for (size_t i = 0; i < elements; ++i)
			mesh.uvs.uv[i].Relocate(size, mesh.uvs.uv);
	}
	inline void LoadDUVs(uint8_t* ptr, size_t size, size_t elements, Mesh& mesh) {
		assert((size_t)ptr % sizeof(size_t) == 0);
		mesh.uvs.dv = reinterpret_cast<_DVMap*>(ptr);
		for (size_t i = 0; i < elements; ++i)
			mesh.uvs.dv[i].Relocate(size, mesh.uvs.dv);
	}
	inline void LoadLayers(uint8_t* ptr, size_t size, size_t elements, Mesh& mesh) {
		assert((size_t)ptr % sizeof(size_t) == 0);
		mesh.layers = gsl::make_span(reinterpret_cast<Layer*>(ptr), elements);
		for (size_t i = 0; i < elements; ++i)
			mesh.layers[i].Relocate(size, mesh.layers.data());
	}
}
void LoadMesh(uint8_t* data, size_t len, Mesh& mesh) {
	uint8_t* ptr = data;
	while (1) {
		Chunk* chunk = reinterpret_cast<Chunk*>(ptr);
		ptr += sizeof(Chunk);
		if ((ptrdiff_t)(ptr - data) >= len) break;
		if (chunk->tag == Tag(POLS))
			LoadPolygons(ptr, chunk->count, mesh);
		else if (chunk->tag == Tag(VERT))
			LoadVertices(ptr, chunk->count, mesh);
		else if (chunk->tag == Tag(SURF))
			LoadSurfaces(ptr, chunk->size, chunk->count, mesh);
		else if (chunk->tag == Tag(VMP2))
			LoadUVs(ptr, chunk->size, chunk->count, mesh);
		else if (chunk->tag == Tag(DVMP))
			LoadDUVs(ptr, chunk->size, chunk->count, mesh);
		else if (chunk->tag == Tag(LINE))
			LoadLines(ptr, chunk->count, mesh);
		else if (chunk->tag == Tag(LAYR))
			LoadLayers(ptr, chunk->size, chunk->count, mesh);
		ptr += chunk->size;
	}
	mesh.Setup();
}
