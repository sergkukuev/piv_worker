#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>
#include <set>

#define MAX_BITS 32
#define SIZE_BASE 4

// Повторения номеров слов(адресов) и битов
struct repiter 
{
	int adr;	// Номер слова (адрес)
	bool* bits;	// 0 - присутствие номера слова(адреса), 1...MAX_BITS - присутствие битов
};

// Некорректные регулярные выражения
struct regular
{
	string reg;		// Шаблон выражения
	CString desc;	// Замечание (дескриптор)
};

// База регулярных выражений
struct regBase
{
	string correct;
	vector <regular> incorrect;
};

class PIV_DECLARE CTest 
{
public:
	CTest();	// Конструктор
	~CTest();	// Деструктор

	// Начало проверки
	errorSet Start(bookData& book);
	list <errorSet> Start(list <bookData>& books);	

private:
	bookData* book = nullptr;	// Указатель на текущую книгу
	sheetData* sheet = nullptr;	// Указатель на текущий лист

	vector <regBase> base;	// База регулярных выражений
	set<CString> exception;	// Множество исключений задается в конструкторе

	const enum index {numword, title, value, bits, adress};	// Индексы параметров в базе регулярных выражений (min, max, csr в одном флаконе)

	void GetErrors(vector <errorSignal>& syntax, vector <errorSignal>& simantic);	// Проверка на синтаксические и семантические ошибки
	void GetWarnings(vector <errorSignal>& warning);	// Проверка на незначительные ошибки (замечания) 

	void InitRepiter(vector<repiter>& repit);	// Инициализация репитера для проверки перекрытия
	int IsContain(const vector<repiter>& repit, const int& numeric);	// Имеется ли уже такой номер слова (адрес) (в случае неудачи возвр. индекс, иначе -1)

	// Синтаксический анализ
	bool CheckNP(vector <errorSignal>& error);	// Проверка номера набора параметров
	bool SyntaxValue(errorSignal& set);	// Проверка всех числовых параметров
	bool SyntaxTemplate(const CString& field, const int& check, const regBase& test, errorSignal& set); // Синтаксическая проверка шаблонным методом\

	// Семантический анализ
	bool SimanticValue(errorSignal& set, vector <repiter>& repit);		// Проверка всех числовых параметров
	bool FindRepiteTitleInSheet(errorSignal& set, const int& index);	// Поиск повторений идентификатора на листе
	void FindRepiteTitleInBook(errorSignal& set, const int& index);		// Поиск повторений идентификатора в книге
	bool SimanticBits(errorSignal& set, const CString& prevTitle, vector<repiter>& repit);	// Проверка используемых разрядов
	vector<int> CheckCrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit); // Проверка перекрытия битов
	bool CheckSameTitle(const CString& next, const CString& prev);		// Проверка двух соседних наименований на совпадение

	bool WriteError(errorSignal& result, const CString& msg, const int& index);	// Запись ошибки 
};