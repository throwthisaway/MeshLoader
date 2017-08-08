#include "pch.h"
#include "MeshSurface.h"
#include <new>
#include <stdlib.h>
namespace MeshLoader {
	void Surface::Relocate(size_t size, void * p)
	{
		if (!p)
			return;
		RELOCATEPTR(size, SurfInfo, p, surface_infos);
		//new(surface_infos) SurfInfo();	
		RELOCATEPTR(size, RefMap, p, reflection_map);
		RELOCATEPTR(size, RefMap, p, refraction_map);
		RELOCATEPTR(size, char, p, name);
		RELOCATEPTR(size, char, p, program_name);
		//RELOCATEPTR(size, unsigned int,p,poly_list);
		if (reflection_map)
			reflection_map->Relocate(size, p);
		if (refraction_map)
			refraction_map->Relocate(size, p);
		if (surface_infos)
		{
			for (unsigned int i = 0; i < NUM_MAP_TYPE; i++)
				surface_infos[i].Relocate(size, p);
		}
	}
	void Surfaces::CleanUp() {
		// Remember: this address is not pointing to an array of surfaces, it points to a "neutral" char array
		//DEL(m_Surface);
		free(surface); surface = nullptr;
		//TODO:
		//// Call dtors:
		//for (unsigned int i=0;i<m_nSurfaces;i++)
		//	delete m_Surfaces->m_Surface[i];
		//delete [] m_Surfaces;	// Remember: this address is not pointing to an array of surfaces, it Vertices to a "neutral" char array
		//TODO:
	}

	void RefMap::Relocate(size_t size, void * p)
	{
		if (!p)
			return;
		RELOCATEPTR(size, Image, p, map);
		if (map)
			map->Relocate(size, p);
	}

	void SurfInfo::Relocate(size_t size, void * p)
	{
		if (!p)
			return;
		RELOCATEPTR(size, SurfLayer, p, layers);
		//RELOCATEPTR(size, SurfInfo,p,next);
		if (layers)
			layers->Relocate(size, p);
		/*if (next)
			next->Relocate(size,p);*/
	}

	void SurfLayer::Relocate(size_t size, void * p)
	{
		if (!p)
			return;
		//	RELOCATEPTR(size, UVMap,p,uvmap);	
		RELOCATEPTR(size, Image, p, image);
		RELOCATEPTR(size, SurfLayer, p, next);
		//	if (uvmap)
		//		uvmap->Relocate(size,p);
		if (image)
			image->Relocate(size, p);
		if (next)
			next->Relocate(size, p);
	}

	void UVMap::Relocate(size_t size, void * p)
	{
		if (!p)
			return;
		RELOCATEPTR(size, float, p, uv);
		//((long long)ptr != size)? ptr = new((type*)((long long)base+(long long)ptr)) type: NULL	
	}

	void Image::Relocate(size_t size, void * p)
	{
		if (!p)
			return;
		RELOCATEPTR(size, _CHAR, p, path);
	}

	//void Surfaces::Relocate(long size, void * p)
	//{
	//	if (!p)
	//		return;
	//	RELOCATEPTR(size, Surface,p,m_Surface);	
	//	for (int i = 0;i<m_nSurfaces;i++)
	//	{
	//		m_Surface[i].Relocate(size,p);
	//	}
	//}

	void _UVMap::Relocate(size_t size, void * p)
	{
		if (!p)
			return;
		RELOCATEPTR(size, UV, p, uv);
		//((long long)ptr != size)? ptr = new((type*)((long long)base+(long long)ptr)) type: NULL
	}

	void _DVMap::Relocate(size_t size, void * p)
	{
		if (!p)
			return;
		RELOCATEPTR(size, DV, p, dv);
		//((long long)ptr != size)? ptr = new((type*)((long long)base+(long long)ptr)) type: NULL
	}
}
