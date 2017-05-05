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
#include "resource.h"		// основные символы


// CPIVWorkerApp
// Про реализацию данного класса см. PIVWorker.cpp
//

class PIV_WORKER_API CPIVWorker	{
public:
	CPIVWorker();	// Конструктор
	~CPIVWorker();	// Деструктор

	void ReadExcel(CString pathToExcel);			// Чтение ПИВ
	void ReadExcel(vector <CString> pathToExcel);	// Чтение ПИВ (перегрузка)

// Переопределение
private:
	vector <bookData> books;	// Прочитанные ПИВ
	vector <CString> path;		// Пути всех ПИВ

	bool isExist(CString pathToExcel);	// Имеется ли уже такая книга
};
