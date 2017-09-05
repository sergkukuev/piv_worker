#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>
#include <set>

#define MAX_BITS 32

// Структура для проверки повторений номеров слов(адресов) и битов
struct repiter 
{
	int adr;	// Номер слова (адрес)
	bool* bits;	// 0 - присутствие номера слова(адреса), 1...MAX_BITS - присутствие битов
};

// Структура для базы ошибок
struct regexBase 
{
	string correct;		// Корректная регулярка
	vector <regular> incorrect;	// Некорректные регулярки
};

// Структура некорректных регулярок с дескриптором
struct regular
{
	string reg;		// Регулярка
	CString desc;	// Дескриптор
};

class PIV_DECLARE CTest 
{
public:
	CTest();	// Конструктор
	~CTest();	// Деструктор

	errorSet Start(bookData& book);					// Проверка на ошибки одного протокола
	list <errorSet> Start(list <bookData>& books);	// Проверка на все ошибки

private:
	bookData* book = nullptr;	// Указатель на текущую книгу
	sheetData* sheet = nullptr;	// Указательна текущий лист

	vector <regexBase> base;	// База ошибок параметров
	set<CString> exception;	// Множество исключений задается в конструкторе

	void getErrors(vector <errorSignal>& syntax, vector <errorSignal>& simantic);	// Проверка на синтаксические и семантические ошибки
	void getWarnings(vector <errorSignal>& warning);	// Проверка на незначительные ошибки (замечания) 

	void initRepiter(vector<repiter>& repit);	// Инициализация репитера для проверки перекрытия
	int isContain(const vector<repiter>& repit, const int& numeric);	// Имеется ли уже такой номер слова (адрес) (в случае неудачи возвр. индекс, иначе -1)

	// Синтаксический анализ
	bool checkNP(vector <errorSignal>& error);	// Проверка номера набора параметров
	bool syntaxValue(errorSignal& set);	// Проверка всех числовых параметров
	bool syntaxTitle(errorSignal& set);	// Проверка синтаксиса идентификатора
	bool syntaxBits(errorSignal& set);	// Проверка используемых разрядов
	bool syntaxTemplate(const CString& field, const int& check, const regexBase& test, errorSignal& set); // Синтаксическая проверка шаблонным методом\

	// Семантический анализ
	bool simanticValue(errorSignal& set, vector <repiter>& repit);		// Проверка всех числовых параметров
	bool findRepiteTitleInSheet(errorSignal& set, const int& index);	// Поиск повторений идентификатора на листе
	void findRepiteTitleInBook(errorSignal& set, const int& index);		// Поиск повторений идентификатора в книге
	bool simanticBits(errorSignal& set, const CString& prevTitle, vector<repiter>& repit);	// Проверка используемых разрядов
	vector<int> checkCrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit); // Проверка перекрытия битов
	bool checkSameTitle(const CString& next, const CString& prev);		// Проверка двух соседних наименований на совпадение

	bool writeError(errorSignal& result, const CString& msg, const int& index);	// Запись ошибки 
};