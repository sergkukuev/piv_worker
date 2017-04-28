#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"

class CReaderExcel	{
public:
	CReaderExcel();									// Конструктор
	CReaderExcel(vector <CString>& pathToExcel);	// Конструктор в параметром
	~CReaderExcel();								// Деструктор

	vector <bookData> getBooks();	// Чтение книг
	int getSize();					// Получить количество книг

private:
	vector <CString> path;				// Расположение книг
	vector <list<CString>> HeaderTable;	// Набор заголовков
	indexHeader iHeader;				// Индексы расположения заголовков
	bool bHeader;						// Найдены ли заголовки
	CWorkExcel work;					// Рабочая область
};

