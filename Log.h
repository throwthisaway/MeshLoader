#pragma once
#include <vector>
#include "Logger.h"
using namespace std;

namespace Log
{
	
typedef enum eLOG_MODE{LOG_FILE = 0x1, LOG_CONSOLE = 0x2, LOG_APPEND/*For file log*/ = 0x4, LOG_TIME = 0x8, LOG_DATE = 0x16}LOG_MODE; 
class CLog
{
	static vector<CLogger*> loggers;
public:
	static void SetMode(LOG_MODE mode);
	/*static void SetProperty(const char * id, const char *value);*/
	static size_t Add(CLogger * logger);	
	static void Write(_LPCTSTR fmt, ...);
	static void Cleanup(void);
	static void Flush(void);
	CLog();
	~CLog();
};

}
