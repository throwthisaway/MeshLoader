#pragma once
#include "compatibility.h"
#include <vector>
#undef max
#include <gsl.h>
#ifdef GLM
#include <glm\glm.hpp>
//using namespace glm;
#define VEC3 glm::vec3
#define VEC2 glm::vec2
#else
#include <DirectXMath.h>
using namespace DirectX;
#define VEC3 DirectX::XMFLOAT3
#define VEC2 DirectX::XMFLOAT2
#endif
#include "MeshSurface.h"
namespace MeshLoader {
	struct Polygon {
		unsigned long v[VERTICESPERPOLY];
	};
	struct PolyLine {
		unsigned long v1, v2;
	};
	struct Normal {
		VEC3 n[VERTICESPERPOLY];
	};

	struct DUV {
		unsigned long p, v;
		VEC2 uv;
	};

	//struct Layer {
	//	struct Pivot {
	//		float x, y, z;
	//	}pivot;
	//	struct SurfSection {
	//		size_t index, start, count;
	//	};
	//	std::vector<SurfSection> polySections, lineSections;
	//};

	class UVMaps {
		typedef struct st_SurfaceUVMap
		{
			long n;
			float * uv;
			st_SurfaceUVMap() : uv(nullptr), n(0) {}
			~st_SurfaceUVMap() { delete[] uv; }
		}SurfaceUVMap;
		void CreateSurfaceUVs(long n, SurfaceUVMap * uvmap, unsigned int  offset, unsigned int num, const std::vector<Polygon>& polygons);
	public:
		std::vector<std::vector<SurfaceUVMap>> uvmaps;

		size_t m_nUVMaps;
		_UVMap * m_UVMap;
		_DVMap * m_DVMap;
		void Setup(gsl::span<Layer2>, gsl::span<Surface>, const std::vector<Polygon>&);
		void CleanUp(void);
		UVMaps() :m_UVMap(nullptr), m_nUVMaps(0), m_DVMap(nullptr) {}
		~UVMaps() { CleanUp(); }
	};

	struct Mesh {
		std::vector<Polygon> polygons;
		std::vector<VEC3> normalsP;
		std::vector<VEC3> vertices;
		std::vector<Normal> normalsV;
		std::vector<PolyLine> lines;
		//std::vector<Layer> layers;
		gsl::span<Layer2, gsl::dynamic_range> layers;
		gsl::span<Surface, gsl::dynamic_range> surfaces;
		UVMaps uvs;

		void Setup();
	private:
		void CalcNormals();
		std::vector<std::vector<size_t>> VtoP() const;
	};
}