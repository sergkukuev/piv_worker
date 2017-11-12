#include "stdafx.h"
#include "Logger.h"

DWORD WINAPI LoggerThread(LPVOID lpParam)
{
	CoInitialize(NULL);
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	CLogger** object = static_cast<CLogger**>(lpParam);
	Thread(**object);
	CoUninitialize();
	return 0;
}

CLogger::CLogger()	
{
	//InitializeCriticalSection(&cs);
}

CLogger::~CLogger()	
{
	//DeleteCriticalSection(&cs);
}

void CLogger::SetPath(const CString& path) 
{ 
	WaitForSingleObject(hLog, INFINITE);
	this->path = path; 
}

CString CLogger::GetStatus()
{
	WaitForSingleObject(hLog, INFINITE);
	return status;
}

void CLogger::Write(const CString& message)
{
	WaitForSingleObject(hLog, INFINITE);
	params.message = message;
	params.error = false;
	hLog = CreateThread(NULL, 0, LoggerThread, this, 0, 0);
}

void CLogger::WriteError(const CString& message)
{
	WaitForSingleObject(hLog, INFINITE);
	params.message = message;
	params.error = true;
	hLog = CreateThread(NULL, 0, LoggerThread, this, 0, 0);
}

void Thread(CLogger& logger) { logger.Write(); }

void CLogger::Write()
{
	//EnterCriticalSection(&cs);
	params.error ? status = L"При операции произошла ошибка (подробное описание см. в логах)" : status = params.message;

	SYSTEMTIME st;
	CString str;
	GetLocalTime(&st);
	params.message.CompareNoCase(LOG_SLASH) != 0 ?
		str.Format(L"%02d:%02d:%02d %02d/%02d/%d:\t%s\n", st.wHour, st.wMinute, st.wSecond, st.wDay, st.wMonth, st.wYear, params.message) :
		str = params.message;
	//LeaveCriticalSection(&cs);

	CString logPath;
	logPath.Format(L"%s%s\\%s", path, BASE_FOLDER, LOG_FILE_NAME);
	ofstream logStream(logPath, ios::out | ios::app);
	logStream << CT2A(str);
	logStream.close();
	
	// Закрытие потока
	CloseHandle(hLog);
	hLog = NULL;
}