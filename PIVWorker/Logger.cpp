#include "stdafx.h"
#include "Logger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

CString CLogger::GetStatus() { return status; }

bool CLogger::IsRead() 
{
	bool temp = state;
	EnterCriticalSection(&cs);
	state = true;
	LeaveCriticalSection(&cs);
	return temp; 
}

void CLogger::Write(const CString& message, const bool& iter)
{
	Write(message, false, iter);
}

void CLogger::WriteError(const CString& message)
{
	Write(message, true, false);
}

void CLogger::Write(const CString& message, const bool& error, const bool& iter)
{
	error ? status = L"Что-то пошло не так (см. в log.txt)" : status = message;

	SYSTEMTIME st;
	CString str;
	GetLocalTime(&st);
	message.CompareNoCase(LOG_SLASH) != 0 ?
		str.Format(L"%02d:%02d:%02d %02d/%02d/%d:\t%s\n", st.wHour, st.wMinute, st.wSecond, st.wDay, st.wMonth, st.wYear, message) :
		str = message;

	CString logPath;
	logPath.Format(L"%s\\%s", *path, LOG_FILE_NAME);
	std::ofstream logStream(logPath, std::ios::out | std::ios::app);
	logStream << CT2A(str);
	logStream.close();

	EnterCriticalSection(&cs);
	if (!iter)
		state = false;
	LeaveCriticalSection(&cs);
}