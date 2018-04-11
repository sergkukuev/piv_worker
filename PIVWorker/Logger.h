#pragma once

#include <cstring>
#include <vector>

#include "DllDeclare.h"

namespace logdll
{
	const CString lgSlash = L"-----------------------------------------------------------------------------------------------------------------------------------\r\n\r\n";
	const CString lgFolder = L"log";
	
	const enum stIndex {start, end};	// Индексы для статусов основного приложения и dll
	const std::vector <CString> stApp = {L"Диалоговое окно успешно запустилось", L"Диалоговое окно успешно закрылось"}; // Основные статусы приложения
	const std::vector <CString> stDLL = { L"Запуск DLL прошел успешно", L"Закрытие DLL прошло успешно\n" };		// Основные статусы DLL

	// Класс логирования
	// Кроме записи в лог файл класс содержит строку вывода в диалоговое окно о состоянии
	class PIV_DECLARE CLogger
	{
	public:
		static CLogger& Instance()
		{
			static CLogger lg;
			return lg;
		}

		// Запись без изменения состояния DLL
		void WriteInfo(const CString& msg);			// Запись в лог без изменения статуса DLL (функция)
		CLogger& operator>> (const CString& msg);	// Запись в лог без изменения статуса DLL (оператор)

		// Запись с изменением состояния DLL
		void Write(const CString& msg);				// Запись текущей операции ("..." чтобы не отображалось в статус баре)
		void WriteError(const CString& msg);		// Запись ошибки

		bool IsRead();			// Установка флага считывания статуса приложением, использующим DLL
		CString GetStatus();	// Получение статуса DLL
	private:
		CLogger();	// Конструктор
		~CLogger();	// Деструктор

		CRITICAL_SECTION csStat, csFile;	// Критические секции для чтения статуса и записи в файл
		CString path;	// Путь к лог файлам

		CString status;		// Статус
		bool state = false;	// Статус считывания

		CLogger(CLogger const&) = delete;	// Удаляем конструктор копирования
		CLogger& operator= (CLogger const&) = delete;	// И присваивания тоже

		// Логирование с изменением статуса и без изменения
		void Write(const CString& msg, const bool& bErr);	// bErr (true - запись ошибки, false - запись операций DLL)
		void WriteInFile(CString message);
	};
};