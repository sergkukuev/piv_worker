#pragma once

#include "StructPIV.h"
#include "MyException.h"

class CTest
{
public:
	CTest();	// Конструктор
	~CTest();	// Деструктор

	errorBookData Start(bookData book);	// Проверка на все ошибки

private:

	list <errorRegular> errNumWord;		// Набор регулярок для номера слова
	list <errorRegular> errTitle;		// Набор регулярок для наименований сигнала
	list <errorRegular> errMinMaxCSR;	// Набор регулярок для минимального, максимального и цср
	list <errorRegular> errBits;		// Набор регулярок для используемых разрядов
	list <errorRegular> errComent;		// Набор регулярок для коментариев

	void Syntax(errorBookData& errBook, bookData book);			// Проверка на синтаксические ошибки
	void Simantic(errorBookData& errBook, bookData book);		// Проверка на семантические ошибки
	errorRegular getRegular(string regular, CString msg, bool bError = true);	// Установка регулярного выражения

	// Синтаксический анализ
	void SyntaxNumWord(errorBookData& book, size_t cSheet, list<signalData>::iterator it);		// Проверка номера слова
	void SyntaxTitleParam(errorBookData& book, size_t cSheet, list<signalData>::iterator it);	// Проверка наименований сигнала
	void SyntaxMinMaxCSR(errorBookData& book, size_t cSheet, list<signalData>::iterator it);	// Проверка минимального, максимального и цср
	void SyntaxBits(errorBookData& book, size_t cSheet, list<signalData>::iterator it);			// Проверка используемых разрядов
	void SyntaxComment(errorBookData& book, size_t cSheet, list<signalData>::iterator it);		// Проверка коментариев

	// Семантический анализ
	void SimanticNumWord(errorBookData& book, size_t cSheet, list<signalData>::iterator it);	// Проверка номера слова
	void SimanticTitleParam(errorBookData& book, size_t cSheet, list<signalData>::iterator it);	// Проверка наименований сигнала
	void SimanticMinMaxCSR(errorBookData& book, size_t cSheet, list<signalData>::iterator it);	// Проверка минимального, максимального и цср
	void SimanticBits(errorBookData& book, size_t cSheet, list<signalData>::iterator it);		// Проверка используемых разрядов
	void SimanticComment(errorBookData& book, size_t cSheet, list<signalData>::iterator it);	// Проверка коментариев
};

