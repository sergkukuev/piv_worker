#pragma once

#include "StructPIV.h"
#include "Settings.h"
#include "Logger.h"
#include "MyException.h"

#include <regex>
#include <set>

#define MAX_BITS 32

namespace testdll
{
	// Повторения номеров слов(адресов) и битов
	typedef struct
	{
		int adr;	// Номер слова (адрес)
		vector <signalData*> signals;	// Указатели на сигналы с данным адресом
		vector <bool> bits;	// 0...MAX_BITS - присутствие битов
	} repiter;

	// Некорректные регулярные выражения
	typedef struct
	{
		string reg;		// Шаблон выражения
		CString desc;	// Замечание (дескриптор)
	} regular;

	// База регулярных выражений
	typedef struct
	{
		string correct;
		vector <regular> incorrect;
	} regBase;

	// Исключения
	typedef struct
	{
		set<CString> mkio;
		set<CString> arinc;
	} exceptTitle;
}

class PIV_DECLARE CTest 
{
public:
	CTest();	// Конструктор
	~CTest();	// Деструктор

	// Запуск проверки на ошибки
	errorData Start(bookData& book);
	list <errorData> Start(list <bookData>& books);

private:
	stgdll::CSettings& settings = stgdll::CSettings::Instance();	// Указатель на настройки
	logdll::CLogger& logger = logdll::CLogger::Instance();			// Логирование
	bookData* book = nullptr;			// Указатель на текущую книгу
	sheetData* sheet = nullptr;			// Указатель на текущий лист
	testdll::exceptTitle exception;		// Множество исключений (задается в конструкторе)

	vector <testdll::repiter> repit;	// Сетка перекрытия битов (для одного листа)
	vector <testdll::regBase> base;	// База регулярных выражений
	const enum index {numword, title, value, bits, /*adress, */size};	// Индексы параметров в базе регулярных выражений (value = min, max, csr в одном флаконе)

	void Initialize();	// Инициализация
	void GetErrors(errorSheet&);	// Проверка листа на синтаксические и семантические ошибки
	void WriteInfoToLog();	// Запись в лог-файл информации о проверке протокола
	void WriteBookStats(const errorData&);	// Запись статистики для системы логирования
	bool WriteError(errorSignal& signal, CString msg, const int& index);	// Запись ошибки 
	
	// Syntax
	void SyntaxChecker(errorSignal& signal, const int& index);	// Проверка всех параметров сигнала на синтаксические ошибки
	bool TemplateTest(const CString& field, const int& check, const int& index, errorSignal& signal); // Проверка шаблоном
	bool SimpleTest(errorSignal& signal);		// Простая проверка флагов всех числовых параметров

	// Simantic
	void SimanticCheker(errorSignal& signal, const int& index, vector <testdll::repiter>& repit);	// Проверка всех параметров сигнала на семантические ошибки
	bool NpTest(errorSheet&);	// Проверка номера набора параметров / индикационного кадра
	bool ValueTest(errorSignal& signal);	// Проверка всех числовых параметров
	bool TitleRepitTest(errorSignal& signal, const int& index);	// Поиск повторений идентификатора на листе
	bool PartTest(errorSignal& signal);	// Проверка двойного слова (КПРНО35)
	bool BitsTest(errorSignal& signal);	// Проверка используемых разрядов
	bool CheckReplace(CString title, const vector <signalData*> signals);	// Проверка слов, которые выборочно кладутся в одно слово 
	vector<int> CrossBits(vector <int>& bits, const vector <int>& numWord, const bool& sign);		// Проверка перекрытия битов

	// Repiter
	void InitRepiter();		// Инициализация репитера для проверки перекрытия битов
	void ClearRepiter();	// Очистка репитера
	void AddRepiter(const int& numWord, const int& index);	// Добавление нового номера слова (адреса) в сетку
	int GetIndexRepiter(const int& numWord);	// Получить индекс сетки битов по номеру слова (в случае неудачи возвр. индекс, иначе -1)
	
	// Warning
	void WarningChecker(errorSignal& signal, const int& index);	// Проверка на незначительные ошибки (замечания)
	void FindRepiteTitleInBook(errorSignal& signal, const int& index);	// Поиск повторений идентификатора в книге
};