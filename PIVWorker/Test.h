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

class PIV_DECLARE CTest 
{
public:
	CTest();	// Конструктор
	~CTest();	// Деструктор

	// Запуск проверки на ошибки
	errorData Start(bookData& book, const int& iMethod);
	list <errorData> Start(list <bookData>& books, const int& iMethod);	

private:
	bookData* book = nullptr;	// Указатель на текущую книгу
	sheetData* sheet = nullptr;	// Указатель на текущий лист
	set<CString> exception;	// Множество исключений задается в конструкторе
	int iMethod;			// Метод проверки

	vector <regBase> base;	// База регулярных выражений
	const enum index {numword, title, value, bits, /*adress, */size};	// Индексы параметров в базе регулярных выражений (value = min, max, csr в одном флаконе)

	void GetErrors(vector <errorSignal>& syntax, vector <errorSignal>& simantic);	// Проверка листа на синтаксические и семантические ошибки
	void GetWarnings(vector <errorSignal>& warning);	// Проверка листа на незначительные ошибки (замечания) 
	bool WriteError(errorSignal& signal, CString msg, const int& index);	// Запись ошибки 
	
	void InitRepiter(vector<repiter>& repit);		// Инициализация репитера для проверки перекрытия
	int IsRepitContain(const vector<repiter>& repit, const int& numeric);	// Имеется ли уже такой номер слова (адрес) (в случае неудачи возвр. индекс, иначе -1)

	// Syntax
	void SyntaxChecker(errorSignal& signal, const int& index);	// Проверка всех параметров сигнала на синтаксические ошибки
	bool TemplateTest(const CString& field, const int& check, const int& index, errorSignal& signal); // Проверка шаблоном
	bool NpTest(vector <errorSignal>& signals);	// Проверка номера набора параметров
	bool SimpleTest(errorSignal& set);			// Простая проверка флагов всех числовых параметров

	// Semantic
	void SemanticCheker(errorSignal& signal, const int& index, vector <repiter>& repit);	// Проверка всех параметров сигнала на семантические ошибки
	bool ValueTest(errorSignal& set);	// Проверка всех числовых параметров
	bool RepiterTest(errorSignal& set, const int& index);		// Поиск повторений идентификатора на листе
	bool BitsTest(errorSignal& set, vector<repiter>& repit);	// Проверка используемых разрядов
	vector<int> CrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit); // Проверка перекрытия битов
	bool IsReplaceable(const CString& title, const vector <signalData*> signals);		// Проверка на заменяемость
	
	// Замечания
	void FindRepiteTitleInBook(errorSignal& set, const int& index);		// Поиск повторений идентификатора в книге
};