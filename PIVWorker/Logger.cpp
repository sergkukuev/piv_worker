#pragma once

#include "stdafx.h"
#include "Logger.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace logdll;

// Конструктор
CLogger::CLogger()
{
	InitializeCriticalSection(&csFile);
	InitializeCriticalSection(&csStat);
	stgdll::CSettings& stg = stgdll::CSettings::Instance();	// Настройки DLL
	this->path = stg.GetRefPath();	// Привязка указателя пути
	WriteInFile(lgSlash);
	WriteInFile(stDLL[start]);
}

// Деструктор
CLogger::~CLogger()	
{
	WriteInFile(stDLL[end]);
	DeleteCriticalSection(&csFile);
	DeleteCriticalSection(&csStat);
	this->path = nullptr;
}

// Получение статуса DLL
CString CLogger::GetStatus() { return status; }

// Установка флага считывания статуса приложением, использующим DLL
bool CLogger::IsRead() 
{
	bool temp = state;
	EnterCriticalSection(&csStat);
	state = true;
	LeaveCriticalSection(&csStat);
	return temp; 
}

// Запись текущей операции
void CLogger::Write(const CString& message)
{
	Write(message, false);
}

// Запись ошибки
void CLogger::WriteError(const CString& message)
{
	Write(message, true);
}

// Запись в лог файл без изменения статуса
void CLogger::WriteInfo(const CString& message)
{
	WriteInFile(message);
}

// Запись в лог файл без изменения статуса
CLogger& CLogger::operator>>(const CString& message)
{
	WriteInFile(message);
	return *this;
}

// Запись в файл логов с изменением статуса DLL
void CLogger::Write(const CString& msg, const bool& bErr)
{
	WriteInFile(msg);
	EnterCriticalSection(&csStat);
	bErr ? status = L"При операции произошла ошибка (подробнее в log.txt)" : status = msg;
	if (msg.Find(L"...") == -1)
		state = false;
	LeaveCriticalSection(&csStat);
}

// Запись в файл логов без изменения статуса DLL
void CLogger::WriteInFile(CString message)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	if (message.CompareNoCase(lgSlash) != 0)
		message.Format(L"%02d:%02d:%02d %02d/%02d/%d:\t%s\n", st.wHour, st.wMinute, st.wSecond, st.wDay, st.wMonth, st.wYear, message);

	CString logPath;
	logPath.Format(L"%s\\log.txt", *path);

	EnterCriticalSection(&csFile);
	std::ofstream logStream(logPath, std::ios::out | std::ios::app);
	logStream << CT2A(message);
	logStream.close();
	LeaveCriticalSection(&csFile);
}