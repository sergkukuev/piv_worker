#pragma once

#include <cstring>
#include <fstream>

#include "DllDeclare.h"

#define LOG_THREAD_BUSY L"Поток занят, подождите окончания процесса"
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

		// Перегрузка операторов
		CLogger& operator<< (const CString& msg);	// Запись конечной операции
		CLogger& operator>> (const CString& msg);	// Записть промежуточной операции

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

		CLogger(CLogger const&) = delete;	// Удаляем конструктор копирования
		CLogger& operator= (CLogger const&) = delete;	// И присваивания тоже

		void Write(const CString& message, const bool& error, const bool& iter);	// flag (true - запись ошибки, false - запись операций DLL)
	};
};