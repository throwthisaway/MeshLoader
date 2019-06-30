#include "pch_.h"
#include "types.h"
#include <cmath>
#include <stdlib.h>

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
		// vertex back references to polygons
		std::vector<std::vector<index_t>> vtop;
		vtop.resize(vertices.size());
		
		for (index_t i = 0; i < polygons.size();++i) {
			const auto& p = polygons[i];
			vtop[p.v[0]].push_back(i);
			vtop[p.v[1]].push_back(i);
			vtop[p.v[2]].push_back(i);
//#ifdef GLM
			normalsP.push_back(glm::normalize(glm::cross(vertices[p.v[0]] - vertices[p.v[1]], vertices[p.v[2]] - vertices[p.v[1]])));
//#else
//			auto v1 = XMLoadFloat3(&vertices[p.v[0]]),
//				v2 = XMLoadFloat3(&vertices[p.v[1]]),
//				v3 = XMLoadFloat3(&vertices[p.v[2]]);
//			XMFLOAT3 n;
//			XMStoreFloat3(&n, XMVector3Normalize(XMVector3Cross(XMVectorSubtract(v1, v2), XMVectorSubtract(v3, v2))));
//			normalsP.push_back(n);
//#endif
		}
		normalsPV.resize(polygons.size(), {0.f});
		for (index_t i = 0; i < polygons.size(); ++i) {
			const auto& p = polygons[i];
//#ifdef GLM
			const float smooth_limit = glm::radians(89.5f);
			const auto& np = normalsP[i];
//#else
//			const float smooth_limit = XMConvertToRadians(89.5f);
//			auto np = XMLoadFloat3(&normalsP[i]);
//#endif
			// for flat shading
//			normalsPV[i].n[0] = np;
//			normalsPV[i].n[1] = np;
//			normalsPV[i].n[2] = np;
			for (size_t k = 0; k < VERTICESPERPOLY; ++k) {
				// get polygons sharing the same vertex
				for (auto polyIndex : vtop[p.v[k]]) {
//#ifdef GLM
					if (polyIndex == i || std::acos(glm::dot(normalsP[polyIndex], np)) <= smooth_limit)
						normalsPV[i].n[k] += normalsP[polyIndex];
//#else
//					float dot;
//					XMStoreFloat(&dot, XMVector3Dot(XMLoadFloat3(&normalsP[polyIndex]), np));
//					if (std::acos(dot) <= smooth_limit) {
//						normalsPV[i].n[k].x += normalsP[polyIndex].x;
//						normalsPV[i].n[k].y += normalsP[polyIndex].y;
//						normalsPV[i].n[k].z += normalsP[polyIndex].z;
//					}
//#endif
				}
			}
		}

		for (auto& n : normalsPV) {
			for (size_t k = 0; k < VERTICESPERPOLY; ++k) {
//#ifdef GLM
				n.n[k] = glm::normalize(n.n[k]);
//#else
//				XMStoreFloat3(&n.n[k], XMVector3Normalize(XMLoadFloat3(&n.n[k])));
//#endif
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
						CreateSurfaceUVs((index_t)pLayer->uvmap, &uvmaps[surface_section.index][pLayer->uvmap], surface_section.offset, surface_section.count, polygons);
						pLayer = pLayer->next;
					}
				}
			}
		}
	}

	void UVMaps::CreateSurfaceUVs(index_t n, SurfaceUVMap * srfUVOut, index_t offset, index_t count, const gsl::span<const Polygon, gsl::dynamic_range>& polygons) {
		if (srfUVOut->uv) return;
		size_t size;
		srfUVOut->uv = new float[size = (UVCOORDS * VERTICESPERPOLY * count)];
		srfUVOut->n = count;
		gsl::span<UV, gsl::dynamic_range> vertexUVs = gsl::make_span(uv[n].uv, (size_t)uv[n].count);
		// UVs
		for (size_t i = offset, j = 0; i < offset + count; ++i) {
			for (size_t k = 0; k < VERTICESPERPOLY; ++k) {
				auto it = std::lower_bound(std::begin(vertexUVs), std::end(vertexUVs), polygons[i].v[k], [](const auto& p1, const auto& p2) { 
					return p1.point < p2; });
				if (it != std::end(vertexUVs)) {
					assert(it->point == polygons[i].v[k]);
					srfUVOut->uv[j++] = it->u;
					srfUVOut->uv[j++] = it->v;
				}
				else
				{
					// Do nothing...
					assert(false);
				}
			}
		}
		// Discontinous UVs
		for (size_t l = 0; l < dv[n].count; ++l) {
			DV *pDV = &dv[n].dv[l];
			if ((pDV->poly >= offset) && (pDV->poly < offset + count)) {
				float * uv = &srfUVOut->uv[(pDV->poly - offset) * UVCOORDS * VERTICESPERPOLY];
				uv += pDV->point*UVCOORDS;
				*uv = pDV->u; uv++;
				*uv = pDV->v;
			}
		}
	}
}
