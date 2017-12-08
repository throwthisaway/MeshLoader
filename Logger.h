#pragma once
#include "compatibility.h"
namespace Log {

class CLogger {
public:
	CLogger() {};
	virtual void Write(_LPCTSTR) = 0;
	virtual void Close() = 0;
	virtual void Flush() const = 0;
	virtual ~CLogger() = default;
};

}
