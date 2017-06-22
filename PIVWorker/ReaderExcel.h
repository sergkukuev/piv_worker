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

	void getSheets(vector <sheetData>& book, CWorkExcel& work);			// Чтение листов из книги
	void getSignals(vector <signalData>& signals, CWorkExcel& work);	// Чтение параметров на листе

	vector <int> getNumWord(CWorkExcel& work, Cell cell, int& step);	// Получить номера слов
	void translateNumWord(CString field, vector <int>& numWord);		// Перевод номеров слов из строки в числа
	vector <CString> getTitle(CWorkExcel& work, Cell cell, long& step, int& size);
	CString getComment(CWorkExcel& work, Cell cell, int size);


	bool isEmpty(CWorkExcel& work, long row);	// Проверка строки на пустоту
	bool isRemark(CWorkExcel& work, long row);	// Проверка строки на наличие примечания
	bool checkExtension(CString path);			// Проверка расширений файлов
};

