#pragma once
#include "compatibility.h"
#include <vector>
#include <gsl/gsl>
#ifdef GLM
#include <glm/glm.hpp>
//using namespace glm;
using vec3_t = glm::vec3;
using vec2_t = glm::vec2;
#else
#include <DirectXMath.h>
using namespace DirectX;
using vec3_t = DirectX::XMFLOAT3;
using vec2_t = DirectX::XMFLOAT2;
#endif
#include "MeshSurface.h"
namespace MeshLoader {
	struct Polygon {
		union {
			index_t v[VERTICESPERPOLY];
			struct {
				uint32_t v1, v2, v3;
			};
		};
	};
	struct PolyLine {
		union {
			index_t v[2];
		};
		struct {
			index_t v1, v2;
		};
	};
	struct Normal {
		Normal() {}
		union {
			vec3_t n[VERTICESPERPOLY];
			struct {
				vec3_t n1, n2, n3;
			};
		};
	};

	struct DUV {
		vec2_t uv;
		index_t p;
		uint8_t v;
	};

	struct UVMaps {
		typedef struct st_SurfaceUVMap
		{
			float * uv = nullptr;
			index_t n = 0u;
			~st_SurfaceUVMap() { delete[] uv; }
		}SurfaceUVMap;
		void CreateSurfaceUVs(index_t n, SurfaceUVMap * uvmap, index_t offset, index_t count, const gsl::span<Polygon, gsl::dynamic_range>& polygons);
		_UVMap * uv = nullptr;
		_DVMap * dv = nullptr;
		index_t count = 0u;
		std::vector<std::vector<SurfaceUVMap>> uvmaps;
		void Setup(gsl::span<Layer>&, gsl::span<Surface>&, const gsl::span<Polygon, gsl::dynamic_range>&);
		void CleanUp();
		~UVMaps() { CleanUp(); }
	};

	struct Mesh {
		gsl::span<Polygon, gsl::dynamic_range> polygons;
		gsl::span<PolyLine, gsl::dynamic_range> lines;
		gsl::span<vec3_t, gsl::dynamic_range> vertices;
		std::vector<vec3_t> normalsP;
		std::vector<Normal> normalsV;
		gsl::span<Layer, gsl::dynamic_range> layers;
		gsl::span<Surface, gsl::dynamic_range> surfaces;
		UVMaps uvs;

		void Setup();
	private:
		void CalcNormals();
		std::vector<std::vector<size_t>> VtoP() const;
	};
}
