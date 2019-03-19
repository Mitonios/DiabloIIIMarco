#pragma once

#include <windows.h>
#ifdef _DEBUG
bool _trace(TCHAR *format, ...);
#define TRACE _trace
#else
#define TRACE false && _trace
#endif

class Trace
{
public:
	Trace();
	~Trace();
};

