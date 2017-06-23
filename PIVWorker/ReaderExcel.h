#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"
#include "MyException.h"

#define RESERVE_SIGNAL _T("Резерв")

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

	void getNumWord(CString field, vector <int>& result);		// Получить номера слов из строки в числа
	void getDouble(CString field, double& result);				// Получение значения double (если есть)
	void getBits(CString bits, vector <int>& result);			// Получить используемые разряды
	vector <int> CReaderExcel::stepGetBits(CString bits);		// Доп функция для разрядов
	CString getComment(CWorkExcel& work, long row, int size);	// Чтение примечания


	bool isEmpty(CWorkExcel& work, long row);	// Проверка строки на пустоту
	bool isRemark(CWorkExcel& work, long row);	// Проверка строки на наличие примечания
	bool checkExtension(CString path);			// Проверка расширений файлов
};

