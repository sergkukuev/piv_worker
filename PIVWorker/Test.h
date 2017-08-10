#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>
#include <set>

#define MAX_BITS 32

// Структура для проверки повторений номеров слов(адресов) и битов
struct repiter {
	int adr;	// Номер слова (адрес)
	bool* bits;	// 0 - присутствие номера слова(адреса), 1...MAX_BITS - присутствие битов
};

class PIV_DECLARE CTest {
public:
	CTest();	// Конструктор
	~CTest();	// Деструктор

	errorSet Start(bookData& book);					// Проверка на ошибки одного протокола
	list <errorSet> Start(list <bookData>& books);	// Проверка на все ошибки

private:
	bookData* book = nullptr;	// Указатель на текущую книгу
	sheetData* sheet = nullptr;	// Указательна текущий лист

	set<CString> exception;	// Множество исключений задается в конструкторе

	void getErrors(vector <errorSignal>& syntax, vector <errorSignal>& simantic);	// Проверка на синтаксические и семантические ошибки
	void getWarnings(vector <errorSignal>& warning);	// Проверка на незначительные ошибки (замечания) 

	void initRepiter(vector<repiter>& repit);	// Инициализация репитера для проверки перекрытия
	int isContain(const vector<repiter>& repit, const int& numeric);	// Имеется ли уже такой номер слова (адрес) (в случае неудачи возвр. индекс, иначе -1)

	// Синтаксический анализ
	bool checkNP(vector <errorSignal>& error);	// Проверка номера набора параметров
	bool syntaxValue(const signalData& signal, vector <CString>& error);	// Проверка всех числовых параметров
	void stepSyntaxValue(const bool& flag, const int& indx, vector <CString>& error);	// Доп функция для проверки числовых параметров
	bool syntaxTitle(const vector <CString>& title, vector <CString>& error);	// Проверка синтаксиса идентификатора
	bool syntaxBits(const intData& bits, vector <CString>& error);				// Проверка используемых разрядов

	// Семантический анализ
	bool simanticValue(const signalData& signal, vector <CString>& error);	// Проверка всех числовых параметров
	bool findRepiteTitleInSheet(const int& index, vector <CString>& error);	// Поиск повторений идентификатора на листе
	void findRepiteTitleInBook(const int& index, vector <CString>& error);	// Поиск повторений идентификатора в книге
	bool simanticBits(const signalData& signal, const CString& prevTitle, vector<repiter>& repit, vector <CString>& error);	// Проверка используемых разрядов
	bool checkCrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit); // Проверка перекрытия битов
	bool checkSameTitle(const CString& next, const CString& prev);		// Проверка двух соседних наименований на совпадение
};