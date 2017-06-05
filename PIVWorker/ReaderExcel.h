#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"
#include "MyException.h"

struct Header 
{ 
	static const int size = 10;		// Количество заголовков
	vector <list<CString>> list;	// Список допустимых заголовков 
	int adress[size];				// Массив адресов для заголовков

	const int iRow = 0;			//	Индекс строки в массиве адресов
	const int iNumWord = 1;		//	Индекс слова в массиве адресов
	const int iName = 2;		//	Индекс наименования сигнала в массиве адресов
	const int iSignal = 3;		//	Индекс идентификатора сигнала в массиве адресов
	const int iDimension = 4;	//	Индекс размерности в массиве адресов
	const int iMin = 5;			//	Индекс мин.знач. в массиве адресов
	const int iMax = 6;			//	Индекс макс.знач. в массиве адресов
	const int iCSR = 7;			//	Индекс цср в массиве адресов
	const int iBits = 8;		//	Индекс битов в массиве адресов
	const int iComment = 9;		//	Индекс примечаний в массиве адресов
};

const int MAX_EMPTY_STRING = 5;	// Максимально допустимое число пустых строк подряд


// Класс для чтения протоколов из excel файлов
class CReaderExcel	{
public:
	CReaderExcel();		// Конструктор
	~CReaderExcel();	// Деструктор


	bookData getBook(CString pathToExcel);			// Чтение одной книги
	vector<bookData> getBooks(vector <CString>);	// Чтение книг

private:
	vector <CString> extension;	// Допустимые расширения файлов
	Header header;				// Информация о заголовках

	vector <sheetData> getSheets(CWorkExcel& work);		// Чтение листов
	list <signalData> getSignals(CWorkExcel& work);		// Чтение параметров на листе
	CString getCell(CWorkExcel& work, Cell cell, long cName = 1);	// Чтение ячейки

	int getNumPK(CWorkExcel& work);				// Поиск номера кадра (в противном случае будет равен -1)
	bool findHeader(CWorkExcel& work);			// Поиск индексов заголовков

	bool IsEmpty(CWorkExcel& work, long row);	// Проверка строки на пустоту
	bool IsRemark(CWorkExcel& work, long row);	// Проверка строки на наличие примечания
	bool checkExtension(CString path);			// Проверка расширений файлов
};

