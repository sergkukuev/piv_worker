#pragma once

#include <cstdlib>
#include <cerrno>

#include "StructPIV.h"
#include "WorkExcel.h"
#include "MyException.h"

// Информация о повторения в ARINC протоколах
struct arincData
{
	CString symbol;		// Символ замены в идентификаторе
	int current = 0;	// Текущее повторение
	int amount = 0;		// Всего повторений
	int startRow;		// Начало блока повторения
	bool flag = true;	// Флаг отсутствия повторения
};

// Класс для чтения протоколов из Excel
class PIV_DECLARE CReaderExcel
{
public:
	CReaderExcel();		// Конструктор
	~CReaderExcel();	// Деструктор

	bookData GetBook(const CString& pathToExcel, const int& iProject);	// Чтение протокола
private:
	vector <CString> extension;	// Допустимые расширения файлов
	CWorkExcel work;			// Работа пространство excel файла
	Header header;				// Информация о заголовках
	int iProject;				// Проект проверки (930М или Кпрно35)

	void GetSheets(vector <sheetData>& sheets);	// Чтение таблиц протоколов (листов)
	void GetSignals(vector <signalData>& signals, const bool& bArinc);	// Чтение параметров на листе

	// ARINC
	void ArincChecker(arincData& arinc, long& row);	//  Поиск повторяющихся блоков
	void GetArinc(arincData& arinc, const long& row, CString field);	// Чтение циклов повторений в ARINC протоколе (порядковый номер в кадре)

	// Read_params
	// Чтение параметров
	intData GetNumWord(CString field);	// Номера слова
	intData GetAdress(CString field, int current);	// Адреса
	int GetSubIndex(CString& numeric);				// Получение подстрочного индекса адреса и удаление его из строки
	vector <int> StepAdress(CString adress, bool& flag);		// Парсинг поля адреса и преобразование его значений в int
	void GetMinMaxCsr(signalData& signal, const long& row);		// Мин, макс и цср
	intData GetBits(CString field, const int& size);			// Используемых разрядов
	vector <int> StepBits(CString bits, bool& flag);			// Парсинг поля разрядов и преобразование в int
	CString GetComment(long row, const int& size, bool& flag);	// Примечания
	
	// Converters
	int GetInt(CString field, bool& flag);		// Конвертер int значения
	double GetDouble(CString field, bool& flag);	// Конвертер double значения

	// Checkers
	bool IsTitle(const long& row);	// Проверка строки на заголовок
	bool IsEmpty(const long& row);	// Проверка строки на пустоту
	bool IsRemark(const long& row);	// Проверка строки на примечание
	bool ExtensionChecker(const CString& path); // Проверка расширения файла
};