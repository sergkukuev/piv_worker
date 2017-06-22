// PIVWorker.h: главный файл заголовка для DLL PIVWorker
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#ifdef PIV_WORKER_EXPORTS
#define PIV_WORKER_API __declspec(dllexport)
#else
#define PIV_WORKER_API __declspec(dllimport)
#endif

#include "StructPIV.h"		// структуры протоколов
#include "ReaderExcel.h"	// чтение протоколов
#include "Test.h"			// проверка на ошибки протоколов
#include "Report.h"			// создание отчетов об ошибках
#include "resource.h"		// основные символы


// CPIVWorkerApp
// Про реализацию данного класса см. PIVWorker.cpp
//

// команды
struct cmd_set {
	const int open = 0;		// открыть 
	const int test = 1;		// анализировать
	const int report = 2;	// отчет
	const int txt = 3;		// txt
	const int close = 4;	// закрыть
};

class PIV_WORKER_API CPIVWorker	{
public:
	HANDLE primary;				// Основной поток
	HANDLE secondary;			// Дополнительный поток

	CPIVWorker();	// Конструктор
	~CPIVWorker();	// Деструктор

	void ReadExcel(CString pathToExcel);			// Получение пути ПИВ для чтения	
	void ReadExcel(vector <CString> pathToExcel);	// (перегрузка)
	void CloseExcel(CString pathToExcel);			// Закрытие книги ПИВ в памяти
	void CloseExcel(vector <CString> pathToExcel);	// (перегрузка)

	void TestExcel();				// Проверка всех книг
	void Report(CString path);		// Создание отчета об ошибках
	void Report();					// (перегрузка)
	void CreateTxt(CString path);	// Создание txt файлов
	void CreateTxt();				// (перегрузка)

	void setStatusNumPK(bool status);	// Установка флага bNumPK

protected:
	friend void Thread(CPIVWorker& piv);	// Дружественная функция для запуска определенной операции

private:
	cmd_set command;		// Набор команд
	int hCmd;				// Команда для потока (0 - открыть ПИВ, 1 - анализировать, 2 - отчет, 3 - txt, 4 - закрыть)

	vector <bookData> books;	// Прочитанные ПИВ
	vector <CString> path;		// Пути проверенных ПИВ
	vector <CString> buffer;	// Вектор обмена данными
	CString pathReport;			// Путь отчета
	errorSet errorDB;			// База данных ошибок
	bool bNumPK = false;		// Нужно ли устанавливать в txt подкадры

	void StartRead();	// Начало чтения протоколов
	void StartClose();	// Начало закрытия протоколов
	void StartReport();	// Начало создания отчета об ошибках
	void StartTxt();	// Начало создания txt файлов
	
	void ReadExcel();	// Чтение протоколов
	void Test();		// Проверка всех книг
	void MakeReport();	// Создание отчета
	void GenerateTxt();	// Генерировать в txt файл
	void CloseExcel();	// Закрыть протоколы

	int findIndexBook(CString pathToExcel);	// Поиск индекса протокола в открытых ПИВ (в противном -1)
	int findReportBook(CString name);		// Поиск индекса протокола в базе ошибок (в противном -1)

	bool getStatusThread(HANDLE h);		// Проверка доступности потока
	void closeThread(HANDLE& h);		// Закрытие потока
};
