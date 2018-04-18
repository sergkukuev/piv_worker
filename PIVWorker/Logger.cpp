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

// �����������
CLogger::CLogger()
{
	InitializeCriticalSection(&csFile);
	InitializeCriticalSection(&csStat);
	stgdll::CSettings& stg = stgdll::CSettings::Instance();	// ��������� DLL
	path = stg.GetDefaultPath();
	path.Format(L"%s\\%s", path, lgFolder);
	WriteInFile(lgSlash);
	WriteInFile(stThread[start]);
}

// ����������
CLogger::~CLogger()	
{
	DeleteCriticalSection(&csFile);
	DeleteCriticalSection(&csStat);
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

// ������ � ���� ����� � ���������� ������� DLL ("..." ����� �� ������������ � ������ ����)
void CLogger::Write(const CString& msg, const bool& bErr)
{
	WriteInFile(msg);
	EnterCriticalSection(&csStat);
	bErr ? status = L"��� ���������� �������� ��������� ������ (��. ���-�����)" : status = msg;
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
		message.Format(L"%02d:%02d:%02d:\t%s\r\n", st.wHour, st.wMinute, st.wSecond, message);

	CString logPath;
	if (!stgdll::CreateDir(path))
		AfxMessageBox(L"�� ������� ������� ���������� ��� ���-�����", MB_ICONERROR);
	logPath.Format(L"%s\\%02d_%02d_%d.txt", path, st.wDay, st.wMonth, st.wYear);
	EnterCriticalSection(&csFile);
	CStdioFile file;
	BOOL bOper;
	PathFileExists(logPath) ? bOper = file.Open(logPath, CFile::modeNoTruncate | CFile::modeWrite | CFile::typeUnicode) :	// ���� ����������, ��������� ���
		bOper = file.Open(logPath, CFile::modeCreate | CFile::modeWrite | CFile::typeUnicode);	// ���� �� ����������, ������� ���

	if (bOper)
	{
		file.SeekToEnd();
		// �������������� � utf-8
		CT2CA res(message, CP_UTF8);
		file.Write(res, (UINT)::strlen(res)); 
		//file.WriteString(message);
		file.Close();
	}
	else
		AfxMessageBox(L"�� ������� �������� ������ � ���-����", MB_ICONERROR);	// ������ �������� ��� �����
	LeaveCriticalSection(&csFile);
}