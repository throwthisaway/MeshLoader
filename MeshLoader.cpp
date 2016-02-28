#include "pch.h"
#include "MeshLoader.h"
namespace {
	struct Chunk {
		unsigned long tag, size, elements;
	};

	void LoadPolygons(unsigned char * ptr, size_t elements, Mesh& mesh) {
		Polygon* p = reinterpret_cast<Polygon*>(ptr);
		mesh.polygons.assign(p, p + elements);
	}
	void LoadLines(unsigned char * ptr, size_t elements, Mesh& mesh) {
		PolyLine* p = reinterpret_cast<PolyLine*>(ptr);
		mesh.lines.assign(p, p + elements);
	}
	void LoadVertices(unsigned char * ptr, size_t elements, Mesh& mesh) {
		VEC3* p = reinterpret_cast<VEC3*>(ptr);
		mesh.vertices.assign(p, p + elements);
	}
	void LoadSurfaces(unsigned char * ptr, size_t size, size_t elements, Mesh& mesh) {
		mesh.nSurfaces = elements;
		char * dest = new char[size];
		memcpy(dest, ptr, size);
		mesh.surfaces = new(dest)Surface[elements];
		for (size_t i = 0; i < elements; ++i)
			mesh.surfaces[i].Relocate(size, mesh.surfaces);
	}
	void LoadUVs(unsigned char * ptr, size_t size, size_t elements, Mesh& mesh) {
		mesh.uvs.m_nUVMaps = elements;
		char * dest = new char[size];
		memcpy(dest, ptr, size);
		mesh.uvs.m_UVMap = new(dest)_UVMap[elements];
		for (size_t i = 0; i < elements; ++i)
			mesh.uvs.m_UVMap[i].Relocate(size, mesh.uvs.m_UVMap);
	}
	void LoadDUVs(unsigned char * ptr, size_t size, size_t elements, Mesh& mesh) {
		char * dest = new char[size];
		memcpy(dest, ptr, size);
		mesh.uvs.m_DVMap = new(dest)_DVMap[elements];
		for (size_t i = 0; i < elements; ++i)
			mesh.uvs.m_DVMap[i].Relocate(size, mesh.uvs.m_DVMap);
	}
}
void LoadMesh(unsigned char* data, size_t len, Mesh& mesh) {
	size_t pos = 0;
	unsigned char * ptr = data;
	Chunk& chunk = reinterpret_cast<Chunk&>(*ptr);
	ptr += sizeof(chunk);
	while ((unsigned long)(ptr - data) < len) {
		if (chunk.tag == TAG(POLS))
			LoadPolygons(ptr, chunk.elements, mesh);
		else if (chunk.tag == TAG(PNTS))
			LoadVertices(ptr, chunk.elements, mesh);
		else if (chunk.tag == TAG(SURF))
			LoadSurfaces(ptr, chunk.size, chunk.elements, mesh);
		else if (chunk.tag == TAG(VMP2))
			LoadUVs(ptr, chunk.size, chunk.elements, mesh);
		else if (chunk.tag == TAG(DVMP))
			LoadDUVs(ptr, chunk.size, chunk.elements, mesh);
		else if (chunk.tag == TAG(LINE))
			LoadLines(ptr, chunk.elements, mesh);
		ptr += chunk.size;
		chunk = reinterpret_cast<Chunk&>(*ptr);
		ptr += sizeof(chunk);
	}
	mesh.Setup();
}
