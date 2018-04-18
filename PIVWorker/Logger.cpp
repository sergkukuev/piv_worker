#pragma once

#include "stdafx.h"
#include "Logger.h"
#include "Settings.h"
#include "MyException.h"

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
	path = stg.GetDefaultPath();
	path.Format(L"%s\\%s", path, lgFolder);
	WriteInFile(lgSlash);
	WriteInFile(stThread[start]);
}

// Деструктор
CLogger::~CLogger()	
{
	DeleteCriticalSection(&csFile);
	DeleteCriticalSection(&csStat);
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

// Запись в файл логов с изменением статуса DLL ("..." чтобы не отображалось в статус баре)
void CLogger::Write(const CString& msg, const bool& bErr)
{
	WriteInFile(msg);
	EnterCriticalSection(&csStat);
	bErr ? status = L"При выполнении операции произошла ошибка (см. лог-файлы)" : status = msg;
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
		message.Format(L"%02d:%02d:%02d:\t%s\r\n", st.wHour, st.wMinute, st.wSecond, message);

	CString logPath;
	if (!stgdll::CreateDir(path))
		AfxMessageBox(L"Не удалось создать директорию под лог-файлы", MB_ICONERROR);
	logPath.Format(L"%s\\%02d_%02d_%d.txt", path, st.wDay, st.wMonth, st.wYear);
	EnterCriticalSection(&csFile);
	CStdioFile file;
	BOOL bOper;
	PathFileExists(logPath) ? bOper = file.Open(logPath, CFile::modeNoTruncate | CFile::modeWrite | CFile::typeUnicode) :	// Файл существует, открываем его
		bOper = file.Open(logPath, CFile::modeCreate | CFile::modeWrite | CFile::typeUnicode);	// Файл не существует, создаем его

	if (bOper)
	{
		file.SeekToEnd();
		// Преобразование к utf-8
		CT2CA res(message, CP_UTF8);
		file.Write(res, (UINT)::strlen(res)); 
		//file.WriteString(message);
		file.Close();
	}
	else
		AfxMessageBox(L"Не удалось записать данные в лог-файл", MB_ICONERROR);	// Ошибка создания лог файла
	LeaveCriticalSection(&csFile);
}