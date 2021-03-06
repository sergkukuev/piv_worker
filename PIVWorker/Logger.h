#pragma once

#include <cstring>
#include <vector>

#include "DllDeclare.h"

namespace logdll
{
	const CString lgSlash = L"-----------------------------------------------------------------------------------------------------------------------------------\r\n\r\n";
	const CString lgFolder = L"log";
	
	const enum stIndex {start, end};	// ������� ��� �������� ��������� ���������� � dll
	const std::vector <CString> stThread = { L"���������� ��������", L"�������� ������ � ������� �������� DLL" };

	// ����� �����������
	// ����� ������ � ��� ���� ����� �������� ������ ������ � ���������� ���� � ���������
	class PIV_DECLARE CLogger
	{
	public:
		static CLogger& Instance()
		{
			static CLogger lg;
			return lg;
		}

		// ������ ��� ��������� ��������� DLL
		void WriteInfo(const CString& msg);			// ������ � ��� ��� ��������� ������� DLL (�������)
		CLogger& operator>> (const CString& msg);	// ������ � ��� ��� ��������� ������� DLL (��������)

		// ������ � ���������� ��������� DLL
		void Write(const CString& msg);				// ������ ������� �������� ("..." ����� �� ������������ � ������ ����)
		void WriteError(const CString& msg);		// ������ ������

		bool IsRead();			// ��������� ����� ���������� ������� �����������, ������������ DLL
		CString GetStatus();	// ��������� ������� DLL
	private:
		CLogger();	// �����������
		~CLogger();	// ����������

		CRITICAL_SECTION csStat, csFile;	// ����������� ������ ��� ������ ������� � ������ � ����
		CString path;	// ���� � ��� ������

		CString status;		// ������
		bool state = false;	// ������ ����������

		CLogger(CLogger const&) = delete;	// ������� ����������� �����������
		CLogger& operator= (CLogger const&) = delete;	// � ������������ ����

		// ����������� � ���������� ������� � ��� ���������
		void Write(const CString& msg, const bool& bErr);	// bErr (true - ������ ������, false - ������ �������� DLL)
		void WriteInFile(CString message);
	};
};