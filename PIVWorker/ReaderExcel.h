#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"
#include "MyException.h"

#include <cstdlib>
#include <cerrno>

#define SIGN_FIELD L"Зн-4"		// Обозначение отрицательного символа в МКИО

// Информация о повторения в arinc протоколах
struct arincData {
	CString symbol;		// Символ замены в идентификаторе
	int current = 0;	// Текущее повторение
	int amount = 0;		// Всего повторений
	int startRow;		// Начало блока повторения
	bool flag = true;	// Флаг отсутствия повторения
};

// Класс для чтения протоколов из excel файлов
class PIV_DECLARE CReaderExcel	{
public:
	CReaderExcel();		// Конструктор
	~CReaderExcel();	// Деструктор

	bookData getBook(const CString& pathToExcel);	// Чтение одной книги
private:
	vector <CString> extension;	// Допустимые расширения файлов
	Header header;				// Информация о заголовках

	void getSheets(vector <sheetData>& book, CWorkExcel& work);								// Чтение листов из книги
	void getSignals(vector <signalData>& signals, CWorkExcel& work, const bool& isArinc);	// Чтение параметров на листе
	void getArinc(const CString& field, const long& row, arincData& arinc);		// Чтение циклов повторений в arinc протоколе(порядковый номер в кадре)

	intData getNumWord(const CString& field);					// Получить номера слов из строки в числа
	intData getBits(const CString& bits);						// Получить используемые разряды
	intData getAdress(const CString& field, const int& num);	// Получить адрес из строки в число
	void getMinMaxCsr(signalData& signal, CWorkExcel& work, const long& row);				// Получить значения мин, макс и цср
	CString getComment(CWorkExcel& work, const long& row, const int& size, bool& flag);		// Чтение примечания
	
	vector <int> stepGetAdress(const CString& adress, bool& flag);	// Доп функция для адреса
	vector <int> stepGetBits(const CString& bits, bool& flag);		// Доп функция для разрядов
	
	double getDouble(const CString& field, bool& flag);	// Получение значения double (если есть)
	int getInt(const CString& field, bool& flag);		// Получение значения int (если есть)

	int getSubIndex(CString& numeric);					// Получение подстрочного индекса и удаление его из строки
	bool isTitle(CWorkExcel& work, const long& row);	// Проверка строки на заголовок
	bool isEmpty(CWorkExcel& work, const long& row);	// Проверка строки на пустоту
	bool isRemark(CWorkExcel& work, const long& row);	// Проверка строки на наличие примечания
	bool checkExtension(const CString& path);			// Проверка расширений файлов
};

