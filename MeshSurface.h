#pragma once
#include "Image.h"

//#ifdef _VS6
//#define RELOCATEPTR(size, type, base, ptr) ptr = ((long)ptr != size)? new((type*)((long)base+(long)ptr)) type: NULL
//#define RELOCATEPTRARRAY(size, type, base, ptr, arraysize) ptr = ((long)ptr != size)? new((type*)((long)base+(long)ptr)) type[arraysize]: NULL
//#else
#define RELOCATEPTR(size, type, base, ptr) ptr = ((long long)ptr != size)? new((type*)((long long)base+(long long)ptr)) type: NULL
#define RELOCATEPTRARRAY(size, type, base, ptr, arraysize) ptr = ((long long)ptr != size)? new((type*)((long long)base+(long long)ptr)) type[arraysize]: NULL
//#endif

#define CURRENT_OBJECT 0
#define PNTS "PNTS"
#define POLS "POLS"
#define SURF "SURF"

#define SINF "SINF"
#define IMAG "IMAG"
#define RMAP "RMAP"
#define RRMA "RRMA"
#define TXTC "TXTC"
#define VMAP "VMAP"
#define VMP2 "VMP2"
#define DVMP "DVMP"
#define SLYR "SLYR"
#define SRFC "SRFC"
#define LINE "LINE"
#define LAYR "LAYR"
#define SECT "SECT"
#define TAG(t) ((t[3] << 24) | (t[2] << 16) | (t[1] << 8)  | t[0])

#define VERTICESPERPOLY 3
#define VERTICESPERLINE 2
#define MAP_TYPE_COUNT 10
typedef enum eMAP_TYPE{COLOR_MAP,LUMINOSITY_MAP,DIFFUSE_MAP,SPECULARITY_MAP,GLOSSINESS_MAP,REFLECTION_MAP,TRANSPARENCY_MAP,REFRACTION_INDEX_MAP,TRANSLUCENCY_MAP,BUMP_MAP,NUM_MAP_TYPE}MAP_TYPE;
typedef enum eREF_OPT{BACKDROP,RAY_TRACING_BACKDROP,SPHERICAL,RAY_TRACING_SPHERICAL}REF_OPT;
typedef enum eLAYER_TYPE{IMAGE_LAYER,PROCEDURAL_LAYER,GRADIENT_LAYER}LAYER_TYPE;
typedef enum eBLEND_MODE{NORMAL_BLEND,ADDITIVE_BLEND,SUBSTRACTIVE_BLEND,DIFFERENCE_BLEND,MULTIPLY_BLEND,DIVIDE_BLEND,ALPHA_BLEND,TEXTURE_DISPLACEMENT_BLEND}BLEND_MODE;
typedef enum ePROJECTION{PLANAR_PROJECTION,CYLINDERAL_PROJECTION,SPHERICAL_PROJECTION,CUBIC_PROJECTION,FRONT_PROJECTION,UV_PROJECTION}PROJECTION;
typedef enum eTILE_TYPE{RESET_TILE,REPEAT_TILE,MIRROR_TILE,EDGE_TILE}TILE_TYPE;
typedef enum eFALLOFF_TYPE{CUBIC_FALLOFF,SPERICHAL_FALLOFF,LINEARX_FALLOFF,LINEARY_FALLOFF,LINEARZ_FALLOFF}FALLOFF_TYPE;
typedef enum eAXIS{NO_AXIS,X_AXIS,Y_AXIS,Z_AXIS}AXIS;
typedef enum eSIDEDNESS{SINGLE_SIDED,DOUBLE_SIDED}SIDEDNESS;

typedef struct st_RefMap
{
#ifdef _TAG
	long tag;
#endif
	REF_OPT opt;
	union
	{
		Image * map;
		int64_t mapp;					// 64bit pointer workaround
	};
	float seam_angle,blurring;
	SERIALIZER
}RefMap;

typedef struct st_TextureCoords
{
#ifdef _TAG
	long tag;
#endif
	AXIS axis;
	float	scale[3]; // TODO: struct{X,Y,Z}
	float pos[3]; // TODO: struct{X,Y,Z}
	float rot[3]; // TODO: struct{H,P,B}
	float falloff[3]; // TODO: struct{X,Y,Z}
	FALLOFF_TYPE falloff_type;
	bool world_coords;
}TextureCoords;

#define UVCOORDS 2

typedef struct st_UVMap
{
#ifdef _TAG
	long tag;
#endif
	long ID, count, dim;				// LWID during export, per-polygon UV coords. otherwise., count of UV pairs
	union
	{
		float * uv;					// perpoint_count * float UV pairs
		int64_t uvp;					// 64bit pointer workaround
	};
	SERIALIZER
#ifdef __cplusplus
	bool operator== (struct st_UVMap& map1);
#endif
}UVMap;

typedef struct st_SurfLayer
{
#ifdef _TAG
	long tag;
#endif
	LAYER_TYPE type;
	BLEND_MODE blend_mode;
	PROJECTION projection;
	unsigned int uvmap;
	union
	{
		Image * image;
		int64_t imgp;					// 64bit pointer workaround
	};
	float opacity;
	bool invert;
	TILE_TYPE width_tile, height_tile;
	bool pixel_blending;
	union
	{
		struct st_SurfLayer * next;
		int64_t nextp;					// 64bit pointer workaround
	};
	//TextureCoords *texture_coords; obsolote...
	SERIALIZER
}SurfLayer;

typedef struct st_SurfInfo
{
#ifdef _TAG
	long tag;
#endif
	MAP_TYPE type;
	float val; //0..100%, 0.0..
	union
	{
		SurfLayer * layers;
		int64_t lyrsp;					// 64bit pointer workaround
	};
	//struct st_SurfInfo *next;
	SERIALIZER

}SurfInfo;

typedef struct st_Surface
{
#ifdef _TAG
	long tag;
#endif
	union
	{
		float color[3]; // TODO: struct{R,G,B}
		struct /*st_Color*/
		{
			float r, g, b;
		}Color;
	};
	float smoothing;
	union
	{
		SurfInfo * surface_infos;
		int64_t sinfp;					// 64bit pointer workaround
	};
	union
	{
		RefMap * reflection_map;
		int64_t reflmp;					// 64bit pointer workaround
	};
	union
	{
		RefMap * refraction_map;
		int64_t refrmp;					// 64bit pointer workaround
	};
	union
	{
		const char * name;
		int64_t namep;					// 64bit pointer workaround
	};
	SIDEDNESS sidedness;

	//unsigned int poly_offset, poly_count;
	//	line_offset, line_count;
	// GL specific things...
	PTR(const char *, program_name)
	SERIALIZER
	// TODO: render outline, etc.
	/*char *comment;*/
}Surface;

// TODO:: get rid of this
typedef struct st_Surfaces
{	
	long m_nSurfaces;
	Surface * m_Surface;
	
	SERIALIZER
#ifdef __cplusplus
	void CleanUp(void)
	{
	// Remember: this address is not pointing to an array of surfaces, it points to a "neutral" char array
	//DEL(m_Surface);
	if (m_Surface) { delete m_Surface; m_Surface = NULL; };
	//TODO:
	//// Call dtors:
	//for (unsigned int i=0;i<m_nSurfaces;i++)
	//	delete m_Surfaces->m_Surface[i];
	//delete [] m_Surfaces;	// Remember: this address is not pointing to an array of surfaces, it Vertices to a "neutral" char array
	//TODO:
	}
public:
	inline long GetCount(void) { return m_nSurfaces; };
	inline Surface * GetSurfaces() { return m_Surface; };
	inline Surface * GetSurface(long i) { return &m_Surface[i]; };
	st_Surfaces() { m_Surface = NULL; };
	~st_Surfaces() { CleanUp(); };

#endif
}Surfaces;
#define UVCOORDS 2

typedef struct st_UV
{
		unsigned int point; // point index
		float/*float*/ u,v;
}UV;

typedef struct st_DV
{
	unsigned int poly;	// actual polygon index
	unsigned int point; // point index
	float/*float*/ u,v;
}DV;	
typedef struct _st_DVMap
{
#ifdef _TAG
	long tag;
#endif
	unsigned int nV;
	union
	{
		DV *dv;
		int64_t dvp;					// 64bit pointer workaround
	};
	/*void CleanUp(void) { nV = 0; DEL_ARRAY(dv); };
	_st_DVMap(): nV(0), dv(NULL) {};
	~_st_DVMap() { CleanUp(); }*/
	SERIALIZER
}_DVMap;
typedef struct _st_UVMap
{
#ifdef _TAG
	long tag;
#endif
	unsigned int nV;
	union
	{
		UV *uv;
		int64_t uvp;					// 64bit pointer workaround
	};
	/*void CleanUp(void) { nV = 0; DEL_ARRAY(uv); };
	_st_UVMap(): nV(0), uv(NULL) {};
	~_st_UVMap() { CleanUp(); }*/
	SERIALIZER
}_UVMap;

struct Layer2 {
#ifdef _TAG
	long tag;
#endif
	struct {
		float x, y, z;
	}pivot;
	struct Sections {
		unsigned int n;
		struct Section {
#ifdef _TAG
			long tag;
#endif
			unsigned int index, start, count;
		};
		PTR(Section*, sections);
	}poly, line;
	SERIALIZER
};
