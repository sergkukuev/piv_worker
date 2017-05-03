#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"
#include "MyException.h"

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
	CReaderExcel();		// Конструктор
	~CReaderExcel();	// Деструктор


	bookData getBook(CString pathToExcel);			// Чтение одной книги
	vector<bookData> getBooks(vector <CString>);	// Чтение книг

private:
	vector <CString> extension;
	vector <list<CString>> HeaderTable;	// Набор заголовков
	indexHeader iHeader;				// Индексы расположения заголовков

	vector <sheetData> getSheets(CWorkExcel& work);		// Чтение листов
	list <signalData> getSignals(CWorkExcel& work);		// Чтение параметров на листе

	int getNumPK(CWorkExcel& work);				// Поиск номера кадра (в противном случае будет равен -1)
	bool findHeader(CWorkExcel& work);			// Поиск индексов заголовков
	void setHeader(int index, adrCell cell);	// Установка заголовка

	string convertString(CString cStr);			// Функция преобразования CString в string
	bool checkPath(CString path);				// Проверка расширений файлов
};

