#pragma once
#include "Deserialize.h"
#include <functional>

typedef enum eIMAGE_TYPE{STILL_IMAGE,SEQUENCE_IMAGE}IMAGE_TYPE;
typedef struct st_Image
{
#ifdef _TAG
	long tag;
#endif
	union
	{
		_LPSTR path;
		int64_t pathp;
	};
	union
	{
		void * ref;	// Reference to scene related data
		int64_t refp;
	};
	IMAGE_TYPE type;

	float brightness;
	float contrast;
	float hue;
	float saturation;
	float gamma;
	bool invert;
	SERIALIZER
	bool operator== (struct st_Image& img1)
	{		
		if (img1.path && path && !_CSCMP(img1.path, path))
			return (img1.brightness == brightness && img1.contrast == contrast && img1.gamma == gamma && img1.hue == hue && img1.invert == invert && img1.saturation == saturation && img1.type == type);	
		return false;
	}
	bool operator< (const struct st_Image& img1)
	{
		return _CSCMP(img1.path, path) < 0;
	}
	struct st_Less : public std::less<const struct st_Image*>
	{
		bool operator() (const struct st_Image* img1, const struct st_Image* img2)
		{
			return _CSCMP(img1->path, img2->path) < 0;
		}
	};
}Image;
