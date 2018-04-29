#include "File.h"
namespace IO
{

CFile::CFile(FILE * f, _LPCTSTR fname) : f(f), fname(fname) {}

CFile::~CFile(void) {
	Close();
}

bool CFile::IsOpened(void) {
	if (!f)
		return false;
	return true;
}

void CFile::Close(void) {
	if (IsOpened()) fclose(f);
}

void CFile::Seek(SEEK_ORIGIN origin, long offset) {
	if (!IsOpened())
		throw new CFileNotOpenException(fname);
	int _origin;
	switch(origin)
	{
	case SEEK_BEGIN:
		_origin = SEEK_SET;
		break;
	case SEEK_BOTTOM:
		_origin = SEEK_END;
		break;
	case SEEK_POS:
		_origin = SEEK_CUR;
		break;
	}
	fseek(f, offset, _origin);
}
long CFile::Pos(void) {
	if (!IsOpened())
		throw new CFileNotOpenException(fname);
	return ftell(f);
}

long CFile::Size(void) {
	if (!IsOpened())
		throw new CFileNotOpenException(fname);
	// Hack:
	long ret;
	Seek(SEEK_BOTTOM, 0);
	ret = Pos();
	Seek(SEEK_BEGIN, 0);
	return ret;
}
bool CFile::Exists(const char * fname) {
    FILE * f = NULL;
#ifdef PLATFORM_WIN
	fopen_s(&f, fname, "rb");
#else
    f = fopen(fname, "rb");
#endif
    if (f)
        fclose(f);
    return f != NULL;
}

}
