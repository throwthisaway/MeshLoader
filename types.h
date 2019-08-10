#pragma once
#include "compatibility.h"
#include <vector>
#include <gsl/gsl>
#include "SIMDTypeAliases.h"

#include "MeshSurface.h"
namespace MeshLoader {
	struct Polygon {
		union {
			poly_t v[VERTICESPERPOLY];
			struct {
				poly_t v1, v2, v3;
			};
		};
	};
	struct PolyLine {
		union {
			poly_t v[2];
			struct {
				poly_t v1, v2;
			};
		};
	};
	struct Normal {
		Normal() {}
		Normal(float val) {
			for (auto& v : n) {
				v = float3(val);
			}
		}
		//Normal(const float3& n1, const float3& n2, const float3& n3) : n1(n1), n2(n2), n3(n3) {}
		//union {
			float3 n[VERTICESPERPOLY];
			/*struct {
				float3 n1, n2, n3;
			};
		};*/
	};

	struct DUV {
		float2 uv;
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
		void CreateSurfaceUVs(index_t n, SurfaceUVMap * uvmap, index_t offset, index_t count, const gsl::span<const Polygon, gsl::dynamic_range>& polygons);
		const _UVMap * uv = nullptr;
		const _DVMap * dv = nullptr;
		index_t count = 0u;
		std::vector<std::vector<SurfaceUVMap>> uvmaps;
		void Setup(gsl::span<const Layer>&, gsl::span<const Surface>&, const gsl::span<const Polygon, gsl::dynamic_range>&);
	};

	struct Mesh {
		void Setup();

		gsl::span<const Polygon, gsl::dynamic_range> polygons;
		gsl::span<const PolyLine, gsl::dynamic_range> lines;
		gsl::span<const float3, gsl::dynamic_range> vertices;
		std::vector<float3> normalsP;
		std::vector<Normal> normalsPV; //normal by polygon index for every of it's vertex
		gsl::span<const Layer, gsl::dynamic_range> layers;
		gsl::span<const Surface, gsl::dynamic_range> surfaces;
		UVMaps uvs;

		std::vector<uint8_t> data;

	private:
		void CalcNormals();
		std::vector<std::vector<size_t>> VtoP() const;
	};
}
