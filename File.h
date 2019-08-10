#pragma once
#include <stdio.h>
#include "Platform.h"
#include "compatibility.h"
#ifdef PLATFORM_WIN
#include <direct.h>
#elif defined(PLATFORM_MAC_OS)
#include <sys/stat.h>
#include <unistd.h>
#else
#include <unistd.h>
#endif
#include <string>

namespace IO {
#define FILE_READ 0x1L
#define FILE_REWRITE 0x2L
#define FILE_APPEND 0x4L
#define FILE_BIN 0x8L
#define FILE_TEXT 0x10L


typedef enum eSEEK_ORIGIN{SEEK_BEGIN, SEEK_BOTTOM, SEEK_POS}SEEK_ORIGIN;
class CFileExceptionBase {
public:
	std::string msg;
};
class CFileNotOpenException : public CFileExceptionBase {
public:
	CFileNotOpenException(_LPCTSTR fname) { this->msg = __T("File not open: "); this->msg += fname; };
};
class CFileNotFoundException : public CFileExceptionBase
{
public:
	CFileNotFoundException(_LPCTSTR fname) { this->msg = __T("File not found: "); this->msg += fname; };
};

class CFileSystem {
public:
	static void GetCWD(_LPTSTR buffer, int size) {
		#ifdef PLATFORM_WIN
			_getcwd(buffer, size);
		#else
			getcwd(buffer, size);
		#endif
	};
	static void ChDir(_LPCTSTR buffer) {
	#ifdef PLATFORM_WIN
		_chdir(buffer);
	#else
		chdir(buffer); 
	#endif
	};
	static void MkDir(_LPCTSTR  buffer) {
	#ifdef PLATFORM_WIN
		_mkdir(buffer);
	#elif defined(PLATFORM_EMSCRIPTEN)
		// ...
	#elif defined(PLATFORM_MAC_OS)
		mkdir(buffer, 0644);
	#else
		mkdir(buffer);
	#endif
	};
};

class CFile {
	CFile(void);
protected:
	FILE * f = nullptr;
	_LPCTSTR fname;
	CFile(FILE * f, _LPCTSTR fname);
	virtual ~CFile();
public:	
	void Close(void);
	bool IsOpened(void);
	void Seek(SEEK_ORIGIN origin, long offset);
	long Pos(void);
	long Size(void);
	static bool Exists(const char * fname);

};

}
