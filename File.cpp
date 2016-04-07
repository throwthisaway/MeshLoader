#include "File.h"
namespace IO
{

CFile::CFile(void) : f(NULL), fname(NULL)
{
}

CFile::CFile(FILE * f, const char * fname)
{
	this->f = f;
	this->fname = fname;
}

CFile::~CFile(void)
{
	Close();
}

bool CFile::IsOpened(void)
{
	if (!f)
		return false;
	return true;
}

void CFile::Close(void)
{
	if (!IsOpened())
		return;
	fclose(f);
}

void CFile::Seek(SEEK_ORIGIN origin, long offset)
{
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
long CFile::Pos(void)
{
	if (!IsOpened())
		throw new CFileNotOpenException(fname);
	return ftell(f);
}

long CFile::Size(void)
{
	if (!IsOpened())
		throw new CFileNotOpenException(fname);
	// Hack:
	long ret, pos = Pos();
	Seek(SEEK_BOTTOM, 0);
	ret = Pos();
	Seek(SEEK_BEGIN, 0);
	return ret;
}
bool CFile::Exists(const char * fname)
{
    FILE * f = NULL;
    f = fopen(fname, "rb");
    if (f)
        fclose(f);
    return f != NULL;
}

}