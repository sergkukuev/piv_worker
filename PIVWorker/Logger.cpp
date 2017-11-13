#include "stdafx.h"
#include "Logger.h"

CLogger::CLogger()
{
	InitializeCriticalSection(&cs);
}

CLogger::~CLogger()	
{
	DeleteCriticalSection(&cs);
}

void CLogger::SetPath(CString* path) 
{ 
	EnterCriticalSection(&cs);
	this->path = path; 
	LeaveCriticalSection(&cs);
}

CString CLogger::GetStatus()
{
	EnterCriticalSection(&cs);
	CString result = status;
	LeaveCriticalSection(&cs);
	return result;
}

void CLogger::Write(const CString& message)
{
	Write(message, false);
}

void CLogger::WriteError(const CString& message)
{
	Write(message, true);
}

void CLogger::Write(const CString& message, const bool& flag)
{
	EnterCriticalSection(&cs);
	flag ? status = L"При операции произошла ошибка (подробное описание см. в логах)" : status = message;

	SYSTEMTIME st;
	CString str;
	GetLocalTime(&st);
	message.CompareNoCase(LOG_SLASH) != 0 ?
		str.Format(L"%02d:%02d:%02d %02d/%02d/%d:\t%s\n", st.wHour, st.wMinute, st.wSecond, st.wDay, st.wMonth, st.wYear, message) :
		str = message;
	LeaveCriticalSection(&cs);

	CString logPath;
	logPath.Format(L"%s\\%s", path, LOG_FILE_NAME);
	std::ofstream logStream(logPath, std::ios::out | std::ios::app);
	logStream << CT2A(str);
	logStream.close();
}