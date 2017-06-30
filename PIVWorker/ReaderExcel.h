#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"
#include "MyException.h"

#include <cstdlib>
#include <cerrno>

#define SIGN_FIELD L"Зн-4"

// Класс для чтения протоколов из excel файлов
class CReaderExcel	{
public:
	CReaderExcel();		// Конструктор
	~CReaderExcel();	// Деструктор

	bookData getBook(const CString& pathToExcel);	// Чтение одной книги
private:
	vector <CString> extension;	// Допустимые расширения файлов
	Header header;				// Информация о заголовках

	void getSheets(vector <sheetData>& book, CWorkExcel& work);			// Чтение листов из книги
	void getSignals(vector <signalData>& signals, CWorkExcel& work);	// Чтение параметров на листе

	intData getNumWord(const CString& field);			// Получить номера слов из строки в числа
	intData getBits(const CString& bits);				// Получить используемые разряды
	vector <int> stepGetBits(const CString& bits, bool& flag);			// Доп функция для разрядов
	void getMinMaxCsr(signalData& signal, CWorkExcel& work, const long& row);				// Получить значения мин, макс и цср
	CString getComment(CWorkExcel& work, const long& row, const int& size, bool& flag);		// Чтение примечания

	double getDouble(const CString& field, bool& flag);	// Получение значения double (если есть)
	int getInt(const CString& field, bool& flag);		// Получение значения int (если есть)


	bool isEmpty(CWorkExcel& work, const long& row);	// Проверка строки на пустоту
	bool isRemark(CWorkExcel& work, const long& row);	// Проверка строки на наличие примечания
	bool checkExtension(const CString& path);			// Проверка расширений файлов
};

