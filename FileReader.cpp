#include "FileReader.h"
#include <sys/stat.h>
namespace IO
{
	
bool CFileReader::Exist(_LPCTSTR fname)
{
#ifdef WIN32
	struct _stat64i32 buffer;
	return !_stat(fname, &buffer);
#else
	struct stat buffer;
	return !stat(fname, &buffer);
#endif

}

CFileReader * CFileReader::Open(_LPCTSTR fname, long mode)
{
	FILE * f;	
	unsigned long sm;	
	if (mode & FILE_READ)
		sm = 'r';	
	else
		return NULL;
	if (mode & FILE_BIN)
		sm |= 'b'<<8;	
	else if (mode & FILE_TEXT)
		sm |= 't'<<8;	
	else 
		sm |= 'b'<<8;	 
#ifdef PLATFORM_WIN
	if (fopen_s(&f, fname, (_LPCTSTR)&sm))
#else
	if (!(f = fopen(fname, (_LPCTSTR)&sm)))
#endif
		throw CFileNotFoundException(fname);
		//return NULL;	
	return new CFileReader(f, fname);
}

size_t CFileReader::Read(void * data, long count)
{
	if (!IsOpened())
		throw new CFileNotOpenException(fname);
	return ::fread(data, count, 1, f);
}

bool CFileReader::GetLine(_LPTSTR data, long count)
{
	if (count <= 0)
		return false;
	if (!IsOpened())
		throw new CFileNotOpenException(fname);
	if (fgets(data, count, f))
		return true;
	return false;
}
bool CFileReader::Eof(void)
{
	return !!feof(f);
}

}
