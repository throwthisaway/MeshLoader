#pragma once
#define SERIALIZER public: void Relocate(long, void *);
#define PTR(X, x)	union { X x; int64_t _x; };
//#define WIDE_CHAR
#ifdef WIDE_CHAR
#define _LPSTR wchar*
#define _LPCSTR const wchar*
#define _CSCMP _tscscmp
#define _CHAR wchar
#else
#define _LPSTR char* 
#define _LPCSTR const char*
#define _CSCMP strcmp 
#define _CHAR char 
#endif
#define _TAG
