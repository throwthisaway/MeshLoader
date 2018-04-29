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
	size_t Read(void * data, long count);	
	bool GetLine(_LPTSTR data, int count);
	static CFileReader * Open(_LPCTSTR fname, long mode = FILE_READ | FILE_BIN);
	bool Eof(void);
};
}

