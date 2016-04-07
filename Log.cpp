#include "Log.h"
#include <stdarg.h>
#include <stdio.h>

namespace Log
{
vector<CLogger*> CLog::loggers;

CLog::CLog()
{

}

CLog::~CLog()
{

}
#define BUF_SIZE 32768+1
static _CHAR buf[BUF_SIZE];
void CLog::Cleanup(void)
{
	while (loggers.size()>0)
	{
		delete loggers.back();//loggers[0];
		loggers.pop_back();
	}
}
void CLog::Write(_LPCTSTR fmt, ...)
{	
	va_list args;
	va_start(args, fmt);	 
	for (int i=0;i<loggers.size();i++)
	{
		// TODO: time & date
		#ifdef WIN32		
		_vsntprintf(buf, sizeof(buf), fmt, args);
		#else
		vsnprintf(buf, sizeof(buf), fmt, args);
		#endif
		loggers[i]->Write(buf);	
	}	
    va_end(args);
}

size_t CLog::Add(CLogger * logger)
{
	loggers.push_back(logger);
	return loggers.size() - 1;
}
void CLog::Flush(void)
{
	std::vector<Log::CLogger*>::iterator it = loggers.begin();
	while (it != loggers.end())
	{
		(*it)->Flush();	
		it++;
	}
}
}
