#pragma once

#include <cstring>
#include <fstream>

#include "Defines.h"

using namespace std;

typedef struct
{
	CString message;
	bool error;
} ThreadParams;

class PIV_DECLARE CLogger
{
public:
	CLogger();
	~CLogger();

	void Write(const CString& messge);
	void WriteError(const CString& message);

	void SetPath(const CString& path);
	CString GetStatus();
protected:
	friend void Thread(CLogger& logger);
private:
	HANDLE hLog;
	CRITICAL_SECTION cs;

	ThreadParams params;
	CString path;
	CString status;

	void Write();
};

