#include "pch_.h"
#include "MeshLoader.h"
#include <streambuf>
#include <istream>


namespace {
	using namespace MeshLoader;
	inline void LoadPolygons(const uint8_t* ptr, index_t count, Mesh& mesh) {
		assert((size_t)ptr % sizeof(size_t) == 0);
		mesh.polygons = gsl::make_span(reinterpret_cast<const Polygon*>(ptr), count);
	}
	inline void LoadLines(const uint8_t* ptr, index_t count, Mesh& mesh) {
		assert((size_t)ptr % sizeof(size_t) == 0);
		mesh.lines = gsl::make_span(reinterpret_cast<const PolyLine*>(ptr), count);
	}
	inline void LoadVertices(const uint8_t* ptr, index_t count, Mesh& mesh) {
		assert((size_t)ptr % sizeof(size_t) == 0);
		mesh.vertices = gsl::make_span(reinterpret_cast<const float3*>(ptr), count);
	}
	inline void LoadSurfaces(const uint8_t* ptr, index_t size, index_t elements, Mesh& mesh) {
		assert((size_t)ptr % sizeof(size_t) == 0);
		mesh.surfaces = gsl::make_span(reinterpret_cast<const Surface*>(ptr), elements);
		for (size_t i = 0; i < elements; ++i)
			const_cast<Surface&>(mesh.surfaces[i]).Relocate(size, const_cast<Surface*>(mesh.surfaces.data()));
	}
	inline void LoadUVs(const uint8_t* ptr, index_t size, index_t elements, Mesh& mesh) {
		assert((size_t)ptr % sizeof(size_t) == 0);
		mesh.uvs.count = elements;
		mesh.uvs.uv = reinterpret_cast<const _UVMap*>(ptr);
		for (size_t i = 0; i < elements; ++i)
			const_cast<_UVMap&>(mesh.uvs.uv[i]).Relocate(size, const_cast<_UVMap*>(mesh.uvs.uv));
	}
	inline void LoadDUVs(const uint8_t* ptr, index_t size, index_t elements, Mesh& mesh) {
		assert((size_t)ptr % sizeof(size_t) == 0);
		mesh.uvs.dv = reinterpret_cast<const _DVMap*>(ptr);
		for (size_t i = 0; i < elements; ++i)
			const_cast<_DVMap&>(mesh.uvs.dv[i]).Relocate(size, const_cast<_DVMap*>(mesh.uvs.dv));
	}
	inline void LoadLayers(const uint8_t* ptr, index_t size, index_t elements, Mesh& mesh) {
		assert((size_t)ptr % sizeof(size_t) == 0);
		mesh.layers = gsl::make_span(reinterpret_cast<const Layer*>(ptr), elements);
		for (size_t i = 0; i < elements; ++i)
			const_cast<Layer&>(mesh.layers[i]).Relocate(size, const_cast<Layer*>(mesh.layers.data()));
	}
}
namespace MeshLoader {
	inline void LoadChunk(index_t tag, index_t size, index_t count, const uint8_t* data, Mesh& mesh) {
		if (tag == Tag(POLS))
			LoadPolygons(data, count, mesh);
		else if (tag == Tag(VERT))
			LoadVertices(data, count, mesh);
		else if (tag == Tag(SURF))
			LoadSurfaces(data, size, count, mesh);
		else if (tag == Tag(VMP2))
			LoadUVs(data, size, count, mesh);
		else if (tag == Tag(DVMP))
			LoadDUVs(data, size, count, mesh);
		else if (tag == Tag(LINE))
			LoadLines(data, count, mesh);
		else if (tag == Tag(LAYR))
			LoadLayers(data, size, count, mesh);
		else
			assert(false);
	}
	void LoadMesh(const uint8_t* data, size_t len, Mesh& mesh) {
		auto ptr = data;
		const Header* header = reinterpret_cast<const Header*>(ptr);
		ptr += AlignTo(sizeof(Header), header->alignment);
		while (1) {
			if ((ptrdiff_t)(ptr - data) >= len) break;
			const Chunk* chunk = reinterpret_cast<const Chunk*>(ptr);
			ptr += AlignTo(sizeof(Chunk), header->alignment);
			if ((ptrdiff_t)(ptr - data) >= len) break;
			LoadChunk(chunk->tag, chunk->size, chunk->count, ptr, mesh);
			ptr += AlignTo(chunk->size, header->alignment);
		}
		mesh.Setup();
	}
}
