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
#include "resource.h"		// основные символы


// CPIVWorkerApp
// Про реализацию данного класса см. PIVWorker.cpp
//

class PIV_WORKER_API CPIVWorker	{
public:
	CPIVWorker();	// Конструктор
	~CPIVWorker();	// Деструктор

	void ReadExcel(CString pathToExcel);			// Получение пути ПИВ для чтения	
	void ReadExcel(vector <CString> pathToExcel);	// Получение путей ПИВ для чтения

	void Test(); // Проверка всех книг

	void CloseExcel(CString pathToExcel);		// Закрытие книги ПИВ в памяти
	void CloseExcel(vector <CString> pathToExcel);	// Закрытие книг ПИВ в памяти

protected:
	friend void ReadExcel(CPIVWorker& piv);	// Дружественная функция для запуска процесса чтения ПИВ
	friend void Test(CPIVWorker& piv);		// Дружественная функция для запуска процесса тестирования ПИВ

private:
	HANDLE primary;				// Основной поток
	HANDLE secondary;			// Дополнительный поток
	
	vector <bookData> books;	// Прочитанные ПИВ
	vector <CString> path;		// Пути проверенных ПИВ
	vector <CString> checkPath;	// Пути поданных для чтения ПИВ

	errorSet errorDB;	// База данных ошибок

	void StartRead();	// Начало чтения протоколов
	void ReadExcel();	// Чтение протоколов

	void TestAll();		// Проверка всех книг

	int findIndexBook(CString pathToExcel);	// Поиск индекса протокола в открытых ПИВ (в противном -1)
	int findReportBook(CString name);		// Поиск индекса протокола в базе ошибок (в противном -1)

	bool getStatusThread(HANDLE h);		// Проверка доступности потока
	void closeThread(HANDLE& h);		// Закрытие потока
};
