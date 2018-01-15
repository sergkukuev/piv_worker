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

	bookData GetBook(const CString& pathToExcel, bool bProject);	// Чтение протокола
private:
	vector <CString> extension;	// Допустимые расширения файлов
	bool bProj = false;
	Header header;				// Информация о заголовках

	void GetSheets(vector <sheetData>& book, CWorkExcel& work);								// Чтение таблиц протокола
	void GetSignals(vector <signalData>& signals, CWorkExcel& work, const bool& isArinc);	// Чтение параметров на листе
	void GetArinc(const CString& field, const long& row, arincData& arinc);					// Чтение циклов повторений в ARINC протоколе (порядковый номер в кадре)

	// Чтение параметров:
	intData GetNumWord(const CString& field);					// Номер слова
	intData GetBits(const CString& bits, const int& size);		// Используемые разряды
	intData GetAdress(const CString& field, const int& num);	// Адрес
	void GetMinMaxCsr(signalData& signal, CWorkExcel& work, const long& row);				// Мин, макс и цср
	CString GetComment(CWorkExcel& work, const long& row, const int& size, bool& flag);		// Примечание
	
	int GetSubIndex(CString& numeric);							// Получение подстрочного индекса адреса и удаление его из строки
	vector <int> StepAdress(const CString& adress, bool& flag);	// Доп функция для адреса
	vector <int> StepBits(const CString& bits, bool& flag);		// Доп функция для используемых разрядов
	
	void ConcatDW(vector <signalData>& signals);	// Объединение двойных слов
	int ParsePart(const CString& part);					// Парсинг строки с расположением слова
	void findAndReplace(vector <signalData>& signals, size_t index, CString old, vector <CString> revert);
	int findSignalByNum(vector <signalData>& signals, size_t start, vector <CString> part/* int num, CString& partNum, CString& partComment*/);		// Поиск сигнала по номеру и примечанию
	double GetDouble(const CString& field, bool& flag);	// Получение значения double (иначе = 0)
	int GetInt(const CString& field, bool& flag);		// Получение значения int (иначе = 0)

	bool IsTitle(CWorkExcel& work, const long& row);	// Проверка строки на заголовок
	bool IsEmpty(CWorkExcel& work, const long& row);	// Проверка строки на пустоту
	bool IsRemark(CWorkExcel& work, const long& row);	// Проверка строки на наличие примечания
	bool CheckExtension(const CString& path);			// Проверка расширений файлов
};

