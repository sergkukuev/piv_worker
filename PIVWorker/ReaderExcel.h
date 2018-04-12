#pragma once

#include <cstdlib>
#include <cerrno>

#include "StructPIV.h"
#include "Settings.h"
#include "Logger.h"
#include "WorkExcel.h"
#include "MyException.h"

#define PK_FIELD L'№'	// Символ номера подкадра
#define ARINC L"РТМ"	// Линия передачи arinc
#define MKIO L"МКИО"	// Линия передачи 

namespace readdll
{
	// Информация о повторения в ARINC протоколах
	struct arincData
	{
		CString symbol;		// Символ замены в идентификаторе
		int current = 0;	// Текущее повторение
		int amount = 0;		// Всего повторений
		int startRow;		// Начало блока повторения
		bool flag = true;	// Флаг отсутствия повторения
	};
}

// Класс для чтения протоколов из Excel
class PIV_DECLARE CReaderExcel
{
public:
	CReaderExcel();		// Конструктор
	~CReaderExcel();	// Деструктор

	bookData GetBook(const CString& pathToExcel);	// Чтение протокола
private:
	stgdll::CSettings& settings = stgdll::CSettings::Instance();	// Настройки
	logdll::CLogger& logger = logdll::CLogger::Instance();			// Логирование
	vector <CString> extension;	// Допустимые расширения файлов
	CWorkExcel work;			// Работа пространство excel файла
	exceldll::Header header;				// Информация о заголовках

	// Константные значения, требуемые для чтения ПИВ
	const CString line = L"Линия";	// Обозначение линии передачи данных
	const CString arincRemark = L"*Примечание:";	// Обозначение примечания в ARINC
	const vector<CString> remark = { L"Примечания:", L"Примечание:" };	// Примечания в конце таблицы
	const vector <CString> npId = { L"NP" /* все проекты, кроме МФПИ35 */, L"IK_MFPI" /* МФПИ35 */ };	// Идентификаторы, в которых зашиты номера наборов в ПИВ
	const vector <CString> puiId = { L"PagePUI" };			// Идентификаторы, в которых зашиты номера страниц ПУИ (для КАИ 35)
	const vector<CString> sign = { L"Зн-", L"зн.", L"Зн.", L"зн-" };	// Наличие отрицательного символа

	// Основные методы
	void Initialize();	// Инициализация
	void GetSheets(vector <sheetData>& sheets);	// Чтение таблиц протоколов (листов)
	void GetSignals(vector <signalData>& signals, const bool& bArinc);	// Чтение параметров на листе

	// DoubleWord (Обработка двойных слов)
	void ConcatDW(vector <signalData>& signals);	// Объединение двойных слов (установка указателя)
	void findDW(vector<signalData>& signals, size_t start, CString old, vector <CString> revert);	// Поиск второй части двойного слова

	// Arinc (Работа с параметрами при линии передачи arinc)
	void ArincChecker(readdll::arincData& arinc, long& row);	//  Поиск повторяющихся блоков
	void GetArinc(readdll::arincData& arinc, const long& row, CString field);	// Чтение циклов повторений в ARINC протоколе (порядковый номер в кадре)
	intData GetAdress(CString field, int current);	// Чтение адреса
	int GetSubIndex(CString& numeric);				// Получение подстрочного индекса адреса и удаление его из строки
	vector <int> StepAdress(CString adress, bool& flag);	// Парсинг поля адреса и преобразование его значений в int

	// SheetInfo (Обработка доп параметров текущего листа)
	int FindSignalById(const vector<signalData>& signals, const CString& id);	// Поиск сигнала по идентификатору
	int GetNp(const vector<signalData>& signals);		// Значение номера набора параметров
	int GetPuiPage(const vector<signalData>& signals);	// Значение страницы ПУИ
	int ParseValueById(const vector<signalData>&, const vector<CString>& id);	// Выделение значения из комментария по идентификатору (для ПУИ и НП)
	int GetPk();	// Значение номера подкадра

	// ReadParameters (Чтение параметров)
	intData GetNumWord(CString field);	// Номера слова
	void GetMinMaxCsr(signalData& signal, const long& row);		// Мин, макс и цср
	intData GetBits(CString field, const int& size);			// Используемых разрядов
	vector <int> StepBits(CString bits, bool& flag);			// Парсинг поля разрядов и преобразование в int
	CString GetComment(long row, const int& size, bool& flag);	// Примечания
	
	// Converters (Конвертирование строк в другие форматы)
	int GetInt(CString field, bool& flag);		// Конвертер int значения
	int GetInt(string value, bool& flag);		// Перегрузка для string
	double GetDouble(CString field, bool& flag);	// Конвертер double значения

	// Checkers (Различные проверки)
	bool IsTitle(const long& row);	// Проверка строки на заголовок
	bool IsEmpty(const long& row);	// Проверка строки на пустоту
	bool IsRemark(const long& row);	// Проверка строки на примечание
	bool IsArinc();					// Проверка линии передачи
	bool ExtensionChecker(const CString& path); // Проверка расширения файла
};