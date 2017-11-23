#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>
#include <set>

#define MAX_BITS 32

// Повторения номеров слов(адресов) и битов
typedef struct
{
	int adr;	// Номер слова (адрес)
	bool* bits;	// 0 - присутствие номера слова(адреса), 1...MAX_BITS - присутствие битов
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

class PIV_DECLARE CTest 
{
public:
	CTest();	// Конструктор
	~CTest();	// Деструктор

	// Запуск проверки на ошибки
	errorSet Start(bookData& book, const bool& fast);
	list <errorSet> Start(list <bookData>& books, const bool& fast);	

private:
	bookData* book = nullptr;	// Указатель на текущую книгу
	sheetData* sheet = nullptr;	// Указатель на текущий лист
	set<CString> exception;	// Множество исключений задается в конструкторе

	vector <regBase> base;	// База регулярных выражений
	const enum index {numword, title, value, bits, /*adress, */size};	// Индексы параметров в базе регулярных выражений (value = min, max, csr в одном флаконе)

	void GetErrors(vector <errorSignal>& syntax, vector <errorSignal>& simantic, const bool& fast);	// Проверка листа на синтаксические и семантические ошибки
	void GetWarnings(vector <errorSignal>& warning);	// Проверка листа на незначительные ошибки (замечания) 

	void InitRepiter(vector<repiter>& repit);	// Инициализация репитера для проверки перекрытия
	int IsRepitContain(const vector<repiter>& repit, const int& numeric);	// Имеется ли уже такой номер слова (адрес) (в случае неудачи возвр. индекс, иначе -1)

	// Синтаксический анализ
	bool NpTest(vector <errorSignal>& error);	// Проверка номера набора параметров
	bool SimpleTest(errorSignal& set);			// Простая проверка флагов всех числовых параметров
	bool TemplateTest(const CString& field, const int& check, const regBase& test, errorSignal& set); // Проверка шаблоном

	// Семантический анализ
	bool ValueTest(errorSignal& set, vector <repiter>& repit);	// Проверка всех числовых параметров
	bool RepiterTest(errorSignal& set, const int& index);		// Поиск повторений идентификатора на листе
	bool BitsTest(errorSignal& set, const CString& prevTitle, vector<repiter>& repit);	// Проверка используемых разрядов
	vector<int> CrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit); // Проверка перекрытия битов
	bool CheckSameTitle(const CString& next, const CString& prev);		// Проверка двух соседних наименований на совпадение
	
	// Замечания
	void FindRepiteTitleInBook(errorSignal& set, const int& index);		// Поиск повторений идентификатора в книге

	bool WriteError(errorSignal& result, const CString& msg, const int& index);	// Запись ошибки 
};