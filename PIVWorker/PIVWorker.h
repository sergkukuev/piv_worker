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
	HANDLE primary;	// Основной поток

	CPIVWorker();	// Конструктор
	~CPIVWorker();	// Деструктор

	void ReadExcel(const CString& pathToExcel);				// Получение пути ПИВ для чтения
	void ReadExcel(const vector <CString>& pathToExcel);	// (перегрузка)
	
	void CloseExcel();								// Закрытие всех книг
	void CloseExcel(const CString& pathToExcel);	// Закрытие одной книги
	void CloseExcel(const vector <CString>& pathToExcel);

	void TestExcel();				// Проверка всех книг
	//void TestExcel(const CString& pathToExcel);				// Проверка одной книги
	//void TestExcel(const vector <CString>& pathToExcel);	// Проверка выбранных книг

	void Report(const CString& path);		// Создание отчета об ошибках
	void Report();					// (перегрузка)
	
	void CreateTxt(CString path);	// Создание txt файлов
	void CreateTxt();				// (перегрузка)

	void setStatusNumPK(const bool& status);	// Установка флага bNumPK

protected:
	friend void Thread(CPIVWorker& piv);	// Дружественная функция для запуска определенной операции

private:
	const cmd_set command;	// Набор команд
	int hCmd;	// Команда для потока (0 - открыть ПИВ, 1 - анализировать, 2 - отчет, 3 - txt, 4 - закрыть)
	
	list <bookData> books;		// Данные о прочитанных ПИВ
	vector <CString> buffer;	// Вектор временного хранения путей файлов
	CString path;
	list <errorSet> Db;			// База ошибок

	bool bNumPK = false;		// Нужно ли устанавливать в txt подкадры

	void StartRead();	// Начало чтения протоколов
	void StartClose();	// Начало закрытия протоколов
	void StartReport();	// Начало создания отчета об ошибках
	void StartTxt();	// Начало создания txt файлов
	
	void Read();		// Чтение протоколов
	void Test();		// Проверка всех книг
	void MakeReport();	// Создание отчета
	void GenerateTxt();	// Генерировать в txt файл
	
	void Close();		// Закрыть книги

	CString nameFromPath(const CString& path);	// Выделение имени из файла
	bool findBook(const CString& pathToExcel);	// Поиск индекса протокола в открытых ПИВ (в противном -1)

	bool getStatusThread(const HANDLE& h);	// Проверка доступности потока
	void closeThread(HANDLE& h);			// Закрытие потока
};
