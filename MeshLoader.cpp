#include "pch.h"
#include "MeshLoader.h"
#include <streambuf>
#include <istream>
using namespace MeshLoader;
namespace {
	struct Chunk {
		unsigned long tag, size, elements;
	};

	void LoadPolygons(char * ptr, size_t elements, Mesh& mesh) {
		Polygon* p = reinterpret_cast<Polygon*>(ptr);
		mesh.polygons.assign(p, p + elements);
	}
	void LoadLines(char * ptr, size_t elements, Mesh& mesh) {
		PolyLine* p = reinterpret_cast<PolyLine*>(ptr);
		mesh.lines.assign(p, p + elements);
	}
	void LoadVertices(char * ptr, size_t elements, Mesh& mesh) {
		VEC3* p = reinterpret_cast<VEC3*>(ptr);
		mesh.vertices.assign(p, p + elements);
	}
	void LoadSurfaces(char * ptr, size_t size, size_t elements, Mesh& mesh) {
		char * dest = new char[size];
		memcpy(dest, ptr, size);
		mesh.surfaces = gsl::as_span(new(dest)Surface[elements], elements);
		for (size_t i = 0; i < elements; ++i)
			mesh.surfaces[i].Relocate(size, mesh.surfaces.data());
	}
	void LoadUVs(char * ptr, size_t size, size_t elements, Mesh& mesh) {
		mesh.uvs.m_nUVMaps = elements;
		char * dest = new char[size];
		memcpy(dest, ptr, size);
		mesh.uvs.m_UVMap = new(dest)_UVMap[elements];
		for (size_t i = 0; i < elements; ++i)
			mesh.uvs.m_UVMap[i].Relocate(size, mesh.uvs.m_UVMap);
	}
	void LoadDUVs(char * ptr, size_t size, size_t elements, Mesh& mesh) {
		char * dest = new char[size];
		memcpy(dest, ptr, size);
		mesh.uvs.m_DVMap = new(dest)_DVMap[elements];
		for (size_t i = 0; i < elements; ++i)
			mesh.uvs.m_DVMap[i].Relocate(size, mesh.uvs.m_DVMap);
	}
	//void LoadLayers(char * ptr, size_t size, size_t elements, Mesh& mesh) {
	//	struct membuf : std::streambuf
	//	{
	//		membuf(char* begin, char* end) {
	//			setg(begin, begin, end);
	//		}
	//	};
	//	membuf sbuf(ptr, ptr + sizeof(size));
	//	std::istream in(&sbuf);
	//	mesh.layers.reserve(elements);
	//	for (size_t i = 0; i < elements; ++i) {
	//		mesh.layers.push_back({});
	//		auto& l = mesh.layers.back();
	//		in >> l.pivot.x; in >> l.pivot.y; in >> l.pivot.z;
	//		size_t size;
	//		in >> size;
	//		l.polySections.reserve(size);
	//		for (size_t j = 0; j < size; ++j) {
	//			// TODO::
	//		}
	//	}
	//}
	void LoadLayers2(char * ptr, size_t size, size_t elements, Mesh& mesh) {
		char * dest = new char[size];
		memcpy(dest, ptr, size);
		mesh.layers = gsl::as_span(new(dest)Layer2[elements], elements);
		for (size_t i = 0; i < elements; ++i)
			mesh.layers[i].Relocate(size, mesh.layers.data());
	}

}
void LoadMesh(char* data, size_t len, Mesh& mesh) {
	size_t pos = 0;
	char * ptr = data;
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
		else if (chunk.tag == TAG(LAYR))
			LoadLayers2(ptr, chunk.size, chunk.elements, mesh);
		ptr += chunk.size;
		chunk = reinterpret_cast<Chunk&>(*ptr);
		ptr += sizeof(chunk);
	}
	mesh.Setup();
}
