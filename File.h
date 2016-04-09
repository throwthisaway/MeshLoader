#pragma once
#include <stdio.h>
#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <string>
#include "compatibility.h"

namespace IO
{
#define FILE_READ 0x1L
#define FILE_REWRITE 0x2L
#define FILE_APPEND 0x4L
#define FILE_BIN 0x8L
#define FILE_TEXT 0x10L


typedef enum eSEEK_ORIGIN{SEEK_BEGIN, SEEK_BOTTOM, SEEK_POS}SEEK_ORIGIN;
class CFileExceptionBase
{
public:
	std::string msg;
};
class CFileNotOpenException : public CFileExceptionBase
{
public:
	CFileNotOpenException(_LPCTSTR fname) { this->msg = __T("File not open: "); this->msg += fname; };
};
class CFileNotFoundException : public CFileExceptionBase
{
public:
	CFileNotFoundException(_LPCTSTR fname) { this->msg = __T("File not found: "); this->msg += fname; };
};

class CFileSystem
{
public:
	static void GetCWD(_LPTSTR buffer, int size)
	{
		#ifdef WIN32
			_getcwd(buffer, size); 
		#else
			getcwd(buffer, size);
		#endif
	};
	static void ChDir(_LPCTSTR buffer)
	{
	#ifdef WIN32  
		_chdir(buffer);
	#else
		chdir(buffer); 
	#endif
	};
	static void MkDir(_LPCTSTR  buffer)
    {
	#ifdef WIN32  
		_mkdir(buffer);
	#elif __EMSCRIPTEN__

	#else
		mkdir(buffer); 
	#endif
	};
};

class CFile
{
	CFile(void);
protected:
	FILE * f;
	_LPCTSTR fname;
	CFile(FILE * f, _LPCTSTR fname);
    ~CFile();
public:	
	void Close(void);
	bool IsOpened(void);
	void Seek(SEEK_ORIGIN origin, long offset);
	long Pos(void);
	long Size(void);
	static bool Exists(_LPCTSTR fname);

};

}
