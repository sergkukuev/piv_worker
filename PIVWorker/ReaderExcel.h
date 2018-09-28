#pragma once
//#include "StructPIV.h"
#include "WorkExcel.h"

#include "Data.h"
#include "Settings.h"
#include "Logger.h"
#include "MyException.h"

// Класс чтения протоколов из excel файла
class PIV_DECLARE CReader : private CWorkExcel
{
protected:
	CReader();
	~CReader();
	// Чтение протокола
	bool ReadBook(const CString& path, CData& data);	
private:
	CSettings& stg = CSettings::Instance();
	CLogger& log = CLogger::Instance();

	// Индексы заголовков в массиве адресов
	const enum index {
		hRow = 0,	// Текущая строка 
		hNumWord,	// Номер слова
		hName,		// Наименование сигнала 
		hSignal,	// Идентификатор сигнала
		hDimension,	// Размерность
		hMin,		// Мин.знач.
		hMax,		// Макс.знач.
		hCSR,		// Цср 
		hBits,		// Используемые разряды
		hComment,	// Примечание
		hAdress,	// Адрес (для arinc)
		hSize		// Общее количество заголовков
	};
	// Заголовок таблиц протоколов
	struct Header
	{
		vector <vector<CString>> list;	// Список допустимых заголовков 
		long adress[hSize];				// Адреса заголовков
	};
	Header header;	// Заголовки таблиц
	vector <CString> extension = { L"xlsx", L"xlsb", L"xls" };	// Допустимые расширения файлов

	// Константные значения, требуемые для чтения ПИВ
	const vector<CString> remark1 = { L"Примечания:", L"Примечание:" };	// Примечания в конце таблиц
	const vector<CString> remark2 = { L"*Примечание:" };				// Примечания в ARINC таблицах
	const vector<CString> sign = { L"Зн-", L"зн.", L"Зн.", L"зн-" };	// Наличие отрицательного символа

	void GetSheets(vector <CData::Sheet>&);			// Чтение таблиц протоколов (листов)
	void GetSignals(vector <CData::Signal>&, vector<CData::DevRemark>&);	// Чтение параметров на листе
	bool FindHeader(const int& lineType);	// Поиск заголовков на листе

	// DoubleWord (Обработка двойных слов)
	void ConcatDW(vector <signalData>& signals);	// Объединение двойных слов (установка указателя)
	void findDW(vector<signalData>& signals, size_t start, CString old, vector <CString> revert);	// Поиск второй части двойного слова

	// Arinc (Работа с параметрами при линии передачи arinc)
	void ArincChecker(CReader::arincData& arinc, long& row);	//  Поиск повторяющихся блоков
	void GetArinc(CReader::arincData& arinc, const long& row, CString field);	// Чтение циклов повторений в ARINC протоколе (порядковый номер в кадре)
	intData GetAdress(CString field, int current);	// Чтение адреса
	int GetSubIndex(CString& numeric);				// Получение подстрочного индекса адреса и удаление его из строки
	vector <int> StepAdress(CString adress, bool& flag);	// Парсинг поля адреса и преобразование его значений в int

	// SheetInfo (Обработка доп параметров текущего листа)
	vector<int> ParseValueById(const vector<CData::Signal>&, const vector<CString>& id);	// Выделение значения из комментария по идентификатору (для ПУИ и НП)
	int FindSignalById(const vector<CData::Signal>&, const CString& id);	// Поиск сигнала по идентификатору
	void SetNp(CData::Sheet&);		// Значение номера набора параметров
	void SetPuiPage(CData::Sheet&);	// Значение страницы ПУИ
	int GetPk();	// Значение номера подкадра

	// ReadParameters (Чтение параметров)
	intData GetNumWord(CString field);	// Номера слова
	void GetMinMaxCsr(signalData& signal, const long& row);		// Мин, макс и цср
	intData GetBits(CString field, const int& size);			// Используемых разрядов
	vector <int> StepBits(CString bits, bool& flag);			// Парсинг поля разрядов и преобразование в int
	CString GetComment(long row, const int& size);				// Примечания

	// Converters (Конвертирование строк в другие форматы)
	int GetInt(CString field, bool& flag);		// Конвертер int значения
	int GetInt(string value, bool& flag);		// Перегрузка для string
	double GetDouble(CString field, bool& flag);	// Конвертер double значения

	// Checkers (Различные проверки)
	bool ExtensionChecker(const CString&); // Проверка расширения файла
	bool IsTitle(const long& row);	// Проверка строки на заголовок
	bool IsEmpty(const long& row);	// Проверка строки на пустоту
	bool IsRemark(const long& row);	// Проверка строки на примечание
	int SetLine();					// Установка линии передачи
};