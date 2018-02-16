#pragma once

#include <cstring>
#include <fstream>

#include "DllDeclare.h"

#define LOG_THREAD_BUSY L"����� �����, ��������� ��������� ��������"
#define LOG_SLASH	L"================================================================\n"

namespace logdll
{
	class PIV_DECLARE CLogger
	{
	public:
		static CLogger& Instance()
		{
			static CLogger lg;
			return lg;
		}

		// ���������� ����������
		CLogger& operator<< (const CString& msg);	// ������ �������� ��������
		CLogger& operator>> (const CString& msg);	// ������� ������������� ��������

		void Write(const CString& messge, const bool& iter = false);
		void WriteError(const CString& message);

		bool IsRead();
		void SetPath(CString* path);
		CString GetStatus();
	private:
		CLogger();
		~CLogger();

		CRITICAL_SECTION cs;
		CString* path;

		CString status;
		bool state = false;

		CLogger(CLogger const&) = delete;	// ������� ����������� �����������
		CLogger& operator= (CLogger const&) = delete;	// � ������������ ����

		void Write(const CString& message, const bool& error, const bool& iter);	// flag (true - ������ ������, false - ������ �������� DLL)
	};
};