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

// �����������
CLogger::CLogger()
{
	InitializeCriticalSection(&csFile);
	InitializeCriticalSection(&csStat);
	stgdll::CSettings& stg = stgdll::CSettings::Instance();	// ��������� DLL
	this->path = stg.GetRefPath();	// �������� ��������� ����
	WriteInFile(lgSlash);
	WriteInFile(stDLL[start]);
}

// ����������
CLogger::~CLogger()	
{
	WriteInFile(stDLL[end]);
	DeleteCriticalSection(&csFile);
	DeleteCriticalSection(&csStat);
	this->path = nullptr;
}

// ��������� ������� DLL
CString CLogger::GetStatus() { return status; }

// ��������� ����� ���������� ������� �����������, ������������ DLL
bool CLogger::IsRead() 
{
	bool temp = state;
	EnterCriticalSection(&csStat);
	state = true;
	LeaveCriticalSection(&csStat);
	return temp; 
}

// ������ ������� ��������
void CLogger::Write(const CString& message)
{
	Write(message, false);
}

// ������ ������
void CLogger::WriteError(const CString& message)
{
	Write(message, true);
}

// ������ � ��� ���� ��� ��������� �������
void CLogger::WriteInfo(const CString& message)
{
	WriteInFile(message);
}

// ������ � ��� ���� ��� ��������� �������
CLogger& CLogger::operator>>(const CString& message)
{
	WriteInFile(message);
	return *this;
}

// ������ � ���� ����� � ���������� ������� DLL
void CLogger::Write(const CString& msg, const bool& bErr)
{
	WriteInFile(msg);
	EnterCriticalSection(&csStat);
	bErr ? status = L"��� �������� ��������� ������ (��������� � log.txt)" : status = msg;
	if (msg.Find(L"...") == -1)
		state = false;
	LeaveCriticalSection(&csStat);
}

// ������ � ���� ����� ��� ��������� ������� DLL
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