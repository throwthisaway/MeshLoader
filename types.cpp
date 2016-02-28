#include "pch.h"
#include "types.h"
#include <cmath>
#include <stdlib.h>

namespace {

	// TODO:: get rid of this shit...
	int cmpUVVert(const void * p1, const void * p2)	{
		// TODO:: point index must be smaller than 2^31 !!!
		return (int)((int64_t)p1 - ((UV*)p2)->point);
	}


#ifdef GLM
	const float smooth_limit = glm::radians(89.5f);
	void SumNormals(std::vector<VEC3>& v_to_n, size_t v, const VEC3* const np) {
		if (std::isnan(v_to_n[v].x))
			v_to_n[v] = *np;
		else if (std::acos(glm::dot(glm::normalize(v_to_n[v]), *np)) <= smooth_limit)
			v_to_n[v] += *np;
	}
#else
	const float smooth_limit = XMConvertToRadians(89.5f);
	void SumNormals(std::vector<VEC3>& v_to_n, size_t v, const VEC3* const np) {
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

void Mesh::Setup() {
	CalcNormals();
	uvs.Setup(surfaces, nSurfaces, polygons);
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
	for (size_t i = 0; i < nSurfaces; ++i) {
		const Surface& srf = surfaces[i];
		const size_t end = (size_t)srf.poly_offset + srf.poly_num;
		std::vector<VEC3> v_to_n(vertices.size());
		std::fill(v_to_n.begin(), v_to_n.end(), VEC3{NAN, NAN, NAN });
		// sum the poly normals sharing the same surface
		for (size_t j = srf.poly_offset; j < end; ++j) {
			const auto* np = &normalsP[j];
			SumNormals(v_to_n, polygons[j].v[0], np);
			SumNormals(v_to_n, polygons[j].v[1], np);
			SumNormals(v_to_n, polygons[j].v[2], np);
		}
		for (size_t j = srf.poly_offset; j < end; ++j) {
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

std::vector<std::vector<size_t>> Mesh::VtoP() const {
	std::vector<std::vector<size_t>> res(vertices.size());
	for (size_t i = 0; i < polygons.size(); ++i) {
		const auto& p = polygons[i];
		res[p.v[0]].push_back(i);
		res[p.v[1]].push_back(i);
		res[p.v[2]].push_back(i);
	}
	return res;
}

Mesh::Mesh() : surfaces(nullptr) {}
Mesh::~Mesh() {
	delete[] surfaces;
}

void UVMaps::CleanUp(void)
{
	delete[] m_UVMap;
	delete[] m_DVMap;
	for (size_t i = 0; i<nUV; i++)
	{
		delete[] uvmaps[i];
	}
	delete[] uvmaps;
}

void UVMaps::Setup(Surface* surfaces, size_t nSurfaces, const std::vector<Polygon>& polygons)
{
	if (!m_nUVMaps)
		return;
	uvmaps = new SurfaceUVMap*[nUV = nSurfaces];
	for (size_t i = 0; i<nSurfaces; i++)
	{
		uvmaps[i] = new SurfaceUVMap[m_nUVMaps];
	}
	for (size_t i = 0; i<nSurfaces; i++)
	{
		for (unsigned int j = 0; j<NUM_MAP_TYPE; j++)
		{
			SurfLayer * pLayer = surfaces[i].surface_infos[j].layers;
			while (pLayer && (pLayer->uvmap != m_nUVMaps))
			{
				CreateSurfaceUVs((long)pLayer->uvmap, &uvmaps[i][(long)pLayer->uvmap], surfaces[i].poly_offset, surfaces[i].poly_num, polygons);
				/// pLayer->uvmap = uvmaps[i][(long)pLayer->uvmap].VBO;
				pLayer = pLayer->next;
			}
		}
	}
}

void UVMaps::CreateSurfaceUVs(long n, SurfaceUVMap * uvmap, unsigned int offset, unsigned int num, const std::vector<Polygon>& polygons)
{
	if (uvmap->uv)
		return;
	assert((size_t)n < m_nUVMaps);
	long size;
	uvmap->uv = new float[size = (UVCOORDS * VERTICESPERPOLY * num)];
	// UVs
	for (unsigned int i = offset, j = 0; i<offset + num; i++)
	{
		for (unsigned int k = 0; k<VERTICESPERPOLY; k++)
		{
			UV *pUV = (UV*)bsearch((const void*)polygons[i].v[k], m_UVMap[n].uv, m_UVMap[n].nV, sizeof(UV), cmpUVVert);
			if (pUV)
			{
				uvmap->uv[j] = pUV->u; j++;
				uvmap->uv[j] = pUV->v; j++;
			}
			else
			{
				// Do nothing...
			}
		}
	}
	// Discontinous UVs
	for (unsigned long l = 0; l<m_DVMap[n].nV; l++)
	{
		DV *pDV = &m_DVMap[n].dv[l];
		if ((pDV->poly >= offset) && (pDV->poly<offset + num))
		{
			float * uv = &uvmap->uv[(pDV->poly - offset) * UVCOORDS * VERTICESPERPOLY];
			uv += pDV->point*UVCOORDS;
			*uv = pDV->u; uv++;
			*uv = pDV->v;
		}
	}
}

