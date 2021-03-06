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