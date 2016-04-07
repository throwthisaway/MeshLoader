#pragma once
#include "File.h"
namespace IO
{

class CFileReader : public CFile
{
protected:
	CFileReader(FILE * f, _LPCTSTR fname) : CFile(f, fname) {};
public:
	static bool Exist(_LPCTSTR fname);
	long Read(void * data, long count);	
	bool GetLine(_LPTSTR data, long count);
	static CFileReader * Open(_LPCTSTR fname, long mode = FILE_READ | FILE_BIN);
	bool Eof(void);
};
}

