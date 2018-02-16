#pragma once

#include <cstring>
#include <fstream>

#include "Defines.h"

#define LOG_THREAD_BUSY L"Поток занят, подождите окончания процесса"
#define LOG_SLASH	L"================================================================\n"

class PIV_DECLARE CLogger
{
public:
	CLogger();
	~CLogger();

	void Write(const CString& messge, const bool& iter = false);
	void WriteError(const CString& message);

	bool IsRead();
	void SetPath(CString* path);
	CString GetStatus();
private:
	CRITICAL_SECTION cs;
	CString* path;
	
	CString status;
	bool state = false;

	void Write(const CString& message, const bool& error, const bool& iter);	// flag (true - запись ошибки, false - запись операций DLL)
};

CLogger logger;

