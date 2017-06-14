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
#define VERT "VERT"
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

#define VERTICESPERPOLY 3u
#define VERTICESPERLINE 2u
#define UVCOORDS 2u
#define MAP_TYPE_COUNT 10u
namespace MeshLoader {
	typedef enum eMAP_TYPE { COLOR_MAP, LUMINOSITY_MAP, DIFFUSE_MAP, SPECULARITY_MAP, GLOSSINESS_MAP, REFLECTION_MAP, TRANSPARENCY_MAP, REFRACTION_INDEX_MAP, TRANSLUCENCY_MAP, BUMP_MAP, NUM_MAP_TYPE }MAP_TYPE;
	typedef enum eREF_OPT { BACKDROP, RAY_TRACING_BACKDROP, SPHERICAL, RAY_TRACING_SPHERICAL }REF_OPT;
	typedef enum eLAYER_TYPE { IMAGE_LAYER, PROCEDURAL_LAYER, GRADIENT_LAYER }LAYER_TYPE;
	typedef enum eBLEND_MODE { NORMAL_BLEND, ADDITIVE_BLEND, SUBSTRACTIVE_BLEND, DIFFERENCE_BLEND, MULTIPLY_BLEND, DIVIDE_BLEND, ALPHA_BLEND, TEXTURE_DISPLACEMENT_BLEND }BLEND_MODE;
	typedef enum ePROJECTION { PLANAR_PROJECTION, CYLINDERAL_PROJECTION, SPHERICAL_PROJECTION, CUBIC_PROJECTION, FRONT_PROJECTION, UV_PROJECTION }PROJECTION;
	typedef enum eTILE_TYPE { RESET_TILE, REPEAT_TILE, MIRROR_TILE, EDGE_TILE }TILE_TYPE;
	typedef enum eFALLOFF_TYPE { CUBIC_FALLOFF, SPERICHAL_FALLOFF, LINEARX_FALLOFF, LINEARY_FALLOFF, LINEARZ_FALLOFF }FALLOFF_TYPE;
	typedef enum eAXIS { NO_AXIS, X_AXIS, Y_AXIS, Z_AXIS }AXIS;
	typedef enum eSIDEDNESS { SINGLE_SIDED, DOUBLE_SIDED }SIDEDNESS;

	struct RefMap {
		_TAG
		REF_OPT opt;
		Image* map;
		float seam_angle, blurring;
		SERIALIZER
	};
	struct TextureCoords {
		_TAG
		float scale[3]; // TODO: struct{X,Y,Z}
		float pos[3]; // TODO: struct{X,Y,Z}
		float rot[3]; // TODO: struct{H,P,B}
		float falloff[3]; // TODO: struct{X,Y,Z}
		bool world_coords;
		FALLOFF_TYPE falloff_type;
		AXIS axis;
	};
	struct UVMap {
		_TAG
		index_t ID, count, dim;		// LWID during export, per-polygon UV coords. otherwise., count of UV pairs
		float* uv;					// perpoint_count * float UV pairs
		SERIALIZER
	};
	struct SurfLayer {
		_TAG
		LAYER_TYPE type;
		Image* image;
		SurfLayer* next;
		BLEND_MODE blend_mode;
		PROJECTION projection;
		index_t uvmap;
		float opacity;
		bool invert;
		TILE_TYPE width_tile, height_tile;
		bool pixel_blending;
		SERIALIZER
	};
	struct SurfInfo	{
		_TAG
		MAP_TYPE type;
		SurfLayer* layers;
		float val; //0..100%, 0.0..
		SERIALIZER
	};
	struct Surface {
		_TAG
		float smoothing;
		SurfInfo* surface_infos;
		RefMap* reflection_map;
		RefMap* refraction_map;
		const char* name, *program_name;
		union
		{
			float color[3];
			struct Color {
				float r, g, b;
			};
		};
		SIDEDNESS sidedness;
		SERIALIZER
	};

	// TODO:: get rid of this
	struct Surfaces	{
		Surface * surface = nullptr;
		index_t count;
		SERIALIZER
#ifdef __cplusplus
	public:
		void CleanUp() {
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
		~Surfaces() { CleanUp(); };
#endif
	};
	struct UV {
		index_t point;
		float u, v;
	};
	struct DV {
		index_t poly, point;
		float u, v;
	};
	struct _DVMap {
		_TAG
		index_t count;
		DV* dv;
		SERIALIZER
	};
	struct _UVMap {
		_TAG
		index_t count;
		UV* uv;
		SERIALIZER
	};
	struct Layer {
		_TAG
		struct {
			float x, y, z;
		}pivot;
		struct Sections {
			struct Section {
				_TAG
				index_t index, offset, count;
			}*sections;
			index_t count;
		}poly, line;
		SERIALIZER
	};
}
