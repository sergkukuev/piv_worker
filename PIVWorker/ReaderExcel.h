#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"

#include <regex>

const int INDEX_NUM = 1;
const int INDEX_NAME = 2;
const int INDEX_SIGNAL = 3;
const int INDEX_DIMENSION = 4;
const int INDEX_MIN = 5;
const int INDEX_MAX = 6;
const int INDEX_CSR = 7;
const int INDEX_BITS = 8;
const int INDEX_COMMENTS = 9;

// Класс для чтения протоколов из excel файлов
class CReaderExcel	{
public:
	CReaderExcel();									// Конструктор
	CReaderExcel(vector <CString> pathToExcel);	// Конструктор в параметром
	~CReaderExcel();								// Деструктор


	bool getBook(CString pathToExcel, bookData& book);				// Чтение одной книги
	bool getBooks(vector <bookData>& books);	// Чтение книг
	
	int getSize();		// Получить количество книг

private:
	vector <list<CString>> HeaderTable;	// Набор заголовков
	indexHeader iHeader;				// Индексы расположения заголовков
	bool bHeader;						// Найдены ли заголовки
	CWorkExcel work;					// Рабочая область

	vector <sheetData> getSheets();		// Чтение листов
	list <signalData> getSignals();		// Чтение параметров на листе

	int getNumPK();								// Поиск номера кадра (в противном случае будет равен -1)
	bool findHeader();							// Поиск индексов заголовков
	void setHeader(int index, adrCell cell);	// Установка заголовка

	string convertString(CString cStr);			// Функция преобразования CString в string
};

