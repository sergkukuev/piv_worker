#pragma once

#include <cstring>
#include <vector>
#include <fstream>

#include "DllDeclare.h"

namespace logdll
{
	const CString lgBr = L"\n\t\t\t\t\t\t";	// ������� ������ � ��� �����
	const CString lgSlash = L"================================================================\n";
	const CString lgName = L"log.txt";	// ��� ����� ��� �����
	
	const enum stIndex {start, end};	// ������� ��� �������� ��������� ���������� � dll
	const std::vector <CString> stApp = {L"���������� ��������", L"���������� ���������"}; // �������� ������� ����������
	const std::vector <CString> stDLL = { L"������ ������ DLL", L"����� ������ DLL" };		// �������� ������� DLL

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
		void Write(const CString& msg);				// ������ ������� ��������
		void WriteError(const CString& msg);		// ������ ������

		bool IsRead();			// ��������� ����� ���������� ������� �����������, ������������ DLL
		CString GetStatus();	// ��������� ������� DLL
	private:
		CLogger();	// �����������
		~CLogger();	// ����������

		CRITICAL_SECTION csStat, csFile;	// ����������� ������ ��� ������ ������� � ������ � ����
		CString* path;	// ��������� �� ���� ����� � ������

		CString status;		// ������
		bool state = false;	// ������ ����������

		CLogger(CLogger const&) = delete;	// ������� ����������� �����������
		CLogger& operator= (CLogger const&) = delete;	// � ������������ ����

		// ����������� � ���������� ������� � ��� ���������
		void Write(const CString& msg, const bool& bErr);	// bErr (true - ������ ������, false - ������ �������� DLL)
		void WriteInFile(CString message);
	};
};