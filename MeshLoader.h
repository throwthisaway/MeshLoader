#pragma once
#include "types.h"

//void LoadPolygons(unsigned char * ptr, size_t elements, Mesh& mesh);
//void LoadVertices(unsigned char * ptr, size_t elements, Mesh& mesh);
//void LoadSurfaces(unsigned char * ptr, size_t size, size_t elements, Mesh& mesh);
//void LoadUVs(unsigned char * ptr, size_t size, size_t elements, Mesh& mesh);
//void LoadDUVs(unsigned char * ptr, size_t size, size_t elements, Mesh& mesh);
void LoadMesh(unsigned char* data, size_t len, Mesh& mesh);
