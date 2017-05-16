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

	errorRegular errNumWord;		// Набор регулярок для номера слова
	errorRegular errTitleParam;		// Набор регулярок для наименований сигнала
	errorRegular errMinMaxCSR;	// Набор регулярок для минимального, максимального и цср
	errorRegular errBits;		// Набор регулярок для используемых разрядов
	errorRegular errComment;		// Набор регулярок для коментариев

	void testAll(errorBookData& errBook, bookData book);	// Проверка на все ошибки
	list <CString> testField(CString field, errorRegular errStruct);	// Проверка поля на ошибки
	errorSignalData CTest::getErrSignal(list<signalData>::iterator it, list <CString> error);	// Создание записи ошибки сигнала
	
	void testNumWord(errorSheetData& sheet, list<signalData>::iterator it);		// Проверка номера слова
	void testTitleParam(errorSheetData& sheet, list<signalData>::iterator it);	// Проверка наименований сигнала
	void testMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator it);	// Проверка минимального, максимального и цср
	void testBits(errorSheetData& sheet, list<signalData>::iterator it);		// Проверка используемых разрядов
	void testComment(errorSheetData& sheet, list<signalData>::iterator it);		// Проверка коментариев

	string convertString(CString cStr);	// Конвертер CString в string 
};

