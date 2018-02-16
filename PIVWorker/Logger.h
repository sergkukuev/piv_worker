#pragma once

#include <cstring>
#include <fstream>

#include "DllDeclare.h"

#define LOG_THREAD_BUSY L"����� �����, ��������� ��������� ��������"
#define LOG_SLASH	L"================================================================\n"
#define LOG_FILE_NAME L"log.txt"

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

	void Write(const CString& message, const bool& error, const bool& iter);	// flag (true - ������ ������, false - ������ �������� DLL)
};

static CLogger logger;

