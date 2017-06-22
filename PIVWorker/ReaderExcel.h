#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"
#include "MyException.h"

// Класс для чтения протоколов из excel файлов
class CReaderExcel	{
public:
	CReaderExcel();		// Конструктор
	~CReaderExcel();	// Деструктор

	bookData getBook(CString pathToExcel);			// Чтение одной книги
private:
	vector <CString> extension;	// Допустимые расширения файлов
	Header header;				// Информация о заголовках

	vector <sheetData> getSheets(CWorkExcel& work);		// Чтение листов
	list <signalData> getSignals(CWorkExcel& work);		// Чтение параметров на листе
	CString getCell(CWorkExcel& work, Cell cell, long cName = 1);	// Чтение ячейки

	int getNumPK(CWorkExcel& work);				// Поиск номера кадра (в противном случае будет равен -1)

	bool isEmpty(CWorkExcel& work, long row);	// Проверка строки на пустоту
	bool isRemark(CWorkExcel& work, long row);	// Проверка строки на наличие примечания
	bool checkExtension(CString path);			// Проверка расширений файлов
};

