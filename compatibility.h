//#define WIDE_CHAR
#ifdef WIDE_CHAR
#define _LPSTR wchar*
#define _LPCSTR const wchar*
#define _CSCMP __Tscscmp
#define _CHAR wchar
#define __T(x) Lx
#define _fgetts fgetws
#define _tstat _wstat
#else
#define _LPSTR char* 
#define _LPCSTR const char*
#define _CSCMP strcmp 
#define _CHAR char 
#define __T(x) x
#ifndef WIN32
#define _fgetts fgets
#define _tstat stat
#endif
#endif
#define _LPCTSTR _LPCSTR
#define _LPTSTR _LPSTR
#ifdef __EMSCRIPTEN__
#define _tfopen fopen
#endif
#define ID_OK 0


#ifdef _WIN32
	#define PLATFORM_WIN
	#undef max
	#ifdef _WIN64
		#define PLATFORM_WIN64
	#else
		#define PLATFORM_WIN32
	#endif
#elif __APPLE__
	#include "TargetConditionals.h"
	#if TARGET_IPHONE_SIMULATOR
		#define PLATFORM_IOS_SIM
	#elif TARGET_OS_IPHONE
		#define PLATFORM_IOS
	#elif TARGET_OS_MAC
		#define PLATFORM_MAC_OS
	#else
	#   error "Unknown Apple platform"
	#endif
#elif __linux__
// linux
#elif __unix__ // all unices not caught above
// Unix
#elif defined(_POSIX_VERSION)
// POSIX
#elif __EMSCRIPTEN__
	#define PLATFORM_EMSCRIPTEN
#else
#   error "Unknown compiler"
#endif

#ifndef PLATFORM_WIN
	#define GLM
#endif
