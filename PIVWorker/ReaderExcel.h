#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"

#include <regex>

// Класс для чтения протоколов из excel файлов
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

	vector <sheetData> getSheets();		// Чтение листов
	list <signalData> getSignals();		// Чтение параметров на листе

	int getNumPK();								// Поиск номера кадра (в противном случае будет равен -1)
	bool findHeader();							// Поиск индексов заголовков
	void setHeader(int index, adrCell& cell);	// Установка заголовка

	string convertString(CString cStr);			// Функция преобразования CString в string
};

