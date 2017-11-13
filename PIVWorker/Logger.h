#pragma once

#include <cstring>
#include <fstream>

#include "Defines.h"

class PIV_DECLARE CLogger
{
public:
	CLogger();
	~CLogger();

	void Write(const CString& messge);
	void WriteError(const CString& message);

	void SetPath(CString* path);
	CString GetStatus();
private:
	CRITICAL_SECTION cs;
	CString* path;
	CString status;

	void Write(const CString& message, const bool& flag);	// flag (true - запись ошибки, false - запись операций DLL)
};

