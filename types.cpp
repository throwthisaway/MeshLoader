#include "pch_.h"
#include "types.h"
#include <cmath>
#include <stdlib.h>

namespace {

	//// TODO:: get rid of this shit...
	//int cmpUVVert(const void * p1, const void * p2)	{
	//	// TODO:: point index must be smaller than 2^31 !!!
	//	return (int)((int64_t)p1 - ((UV*)p2)->point);
	//}


#ifdef GLM
	const float smooth_limit = glm::radians(89.5f);
	void SumNormals(std::vector<vec3_t>& v_to_n, size_t v, const vec3_t* const np) {
		if (std::isnan(v_to_n[v].x))
			v_to_n[v] = *np;
		else if (std::acos(glm::dot(glm::normalize(v_to_n[v]), *np)) <= smooth_limit)
			v_to_n[v] += *np;
	}
#else
	const float smooth_limit = XMConvertToRadians(89.5f);
	void SumNormals(std::vector<vec3_t>& v_to_n, size_t v, const vec3_t* const np) {
		auto vnp = XMLoadFloat3(np);
		if (std::isnan(v_to_n[v].x))
			v_to_n[v] = *np;
		else {
			auto vn = XMLoadFloat3(&v_to_n[v]);
			float dot;
			XMStoreFloat(&dot, XMVector3Dot(XMVector4Normalize(vn), vnp));
			if (std::acos(dot) <= smooth_limit)
				XMStoreFloat3(&v_to_n[v], vn + vnp);
		}
	}
#endif
}
namespace MeshLoader {
	void Layer::Relocate(size_t size, void* p) {
		RELOCATEPTRARRAY(size, Sections::Section, p, poly.sections, poly.count);
		RELOCATEPTRARRAY(size, Sections::Section, p, line.sections, line.count);
	}

	void Mesh::Setup() {
		CalcNormals();
		uvs.Setup(layers, surfaces, polygons);
	}
	void Mesh::CalcNormals() {
		normalsP.reserve(polygons.size());
		for (const auto& p : polygons) {
#ifdef GLM
			normalsP.push_back(glm::cross(vertices[p.v[0]] - vertices[p.v[2]], vertices[p.v[2]] - vertices[p.v[1]]));
#else
			auto v1 = XMLoadFloat3(&vertices[p.v[0]]),
				v2 = XMLoadFloat3(&vertices[p.v[1]]),
				v3 = XMLoadFloat3(&vertices[p.v[2]]);
			XMFLOAT3 n;
			XMStoreFloat3(&n, XMVector3Normalize(XMVector3Cross(XMVectorSubtract(v1, v2), XMVectorSubtract(v3, v2))));
			normalsP.push_back(n);
#endif
		}
		// per-surface, per-vertex normals
		normalsV.resize(polygons.size());
		for (const auto& l : layers) {
			for (unsigned int section = 0; section < l.poly.count; ++section) {
				const size_t end = (size_t)l.poly.sections[section].offset + l.poly.sections[section].count;
				std::vector<vec3_t> v_to_n(vertices.size(), vec3_t{ NAN, NAN, NAN });
				// sum the poly normals sharing the same surface
				for (size_t j = l.poly.sections[section].offset; j < end; ++j) {
					const auto* np = &normalsP[j];
					SumNormals(v_to_n, polygons[j].v[0], np);
					SumNormals(v_to_n, polygons[j].v[1], np);
					SumNormals(v_to_n, polygons[j].v[2], np);
				}
				for (size_t j = l.poly.sections[section].offset; j < end; ++j) {
#ifdef GLM
					normalsV[j].n[0] = glm::normalize(v_to_n[polygons[j].v[0]]);
					normalsV[j].n[1] = glm::normalize(v_to_n[polygons[j].v[1]]);
					normalsV[j].n[2] = glm::normalize(v_to_n[polygons[j].v[2]]);
#else
					XMStoreFloat3(&normalsV[j].n[0], XMVector3Normalize(XMLoadFloat3(&v_to_n[polygons[j].v[0]])));
					XMStoreFloat3(&normalsV[j].n[1], XMVector3Normalize(XMLoadFloat3(&v_to_n[polygons[j].v[1]])));
					XMStoreFloat3(&normalsV[j].n[2], XMVector3Normalize(XMLoadFloat3(&v_to_n[polygons[j].v[2]])));
#endif
				}
			}

		}
	}

	std::vector<std::vector<size_t>> Mesh::VtoP() const {
		std::vector<std::vector<size_t>> res(vertices.size());
		for (size_t i = 0; i < (size_t)polygons.size(); ++i) {
			const auto& p = polygons[i];
			res[p.v[0]].push_back(i);
			res[p.v[1]].push_back(i);
			res[p.v[2]].push_back(i);
		}
		return res;
	}

	void UVMaps::Setup(gsl::span<const Layer>& layers, gsl::span<const Surface>& surfaces, const gsl::span<const Polygon, gsl::dynamic_range>& polygons) {
		if (!count) return;
		uvmaps.resize(surfaces.size());
		for (auto& uvmap : uvmaps) {
			uvmap.resize(count);
		}
		for (const Layer& l : layers) {
			for (size_t k = 0; k < l.poly.count; ++k) {
				auto& surface_section = l.poly.sections[k];
				for (unsigned int j = 0; j < NUM_MAP_TYPE; j++) {
					SurfLayer * pLayer = surfaces[surface_section.index].surface_infos[j].layers;
					while (pLayer && (pLayer->uvmap != count)) {
						CreateSurfaceUVs((long)pLayer->uvmap, &uvmaps[surface_section.index][pLayer->uvmap], surface_section.offset, surface_section.count, polygons);
						pLayer = pLayer->next;
					}
				}
			}
		}
	}

	void UVMaps::CreateSurfaceUVs(index_t n, SurfaceUVMap * uvmap, index_t offset, index_t count, const gsl::span<const Polygon, gsl::dynamic_range>& polygons) {
		if (uvmap->uv) return;
		assert(n < count);
		size_t size;
		uvmap->uv = new float[size = (UVCOORDS * VERTICESPERPOLY * count)];
		// TODO:: owns?? better yet gsl::view
		gsl::span<UV, gsl::dynamic_range> uvMap = gsl::make_span(uv[n].uv, (size_t)uv[n].count);
		// UVs
		for (size_t i = offset, j = 0; i < offset + count; ++i) {
			for (size_t k = 0; k < VERTICESPERPOLY; ++k) {

				auto it = std::lower_bound(std::begin(uvMap), std::end(uvMap), polygons[i].v[k], [](const auto& p1, const auto& p2) { return p1.point == p2; });
				if (it != std::end(uvMap))
				{
					uvmap->uv[j++] = it->u;
					uvmap->uv[j++] = it->v;
				}
				else
				{
					// Do nothing...
				}
			}
		}
		// Discontinous UVs
		for (size_t l = 0; l < dv[n].count; ++l) {
			DV *pDV = &dv[n].dv[l];
			if ((pDV->poly >= offset) && (pDV->poly < offset + count)) {
				float * uv = &uvmap->uv[(pDV->poly - offset) * UVCOORDS * VERTICESPERPOLY];
				uv += pDV->point*UVCOORDS;
				*uv = pDV->u; uv++;
				*uv = pDV->v;
			}
		}
	}
}
