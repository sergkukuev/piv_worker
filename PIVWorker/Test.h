#pragma once

#include "StructPIV.h"
#include "MyException.h"
#include "ErrBase.h"

class CTest
{
public:
	CTest();	// Конструктор
	~CTest();	// Деструктор

	errorBookData Start(bookData book);	// Проверка на все ошибки

private:

	CErrBase ErrorBase;	// База ошибок

	void testAll(errorBookData& errBook, bookData book);	// Проверка на все ошибки
	list <CString> testField(CString field, errorData errStruct);	// Проверка поля на ошибки
	errorSignalData CTest::getErrSignal(list<signalData>::iterator it, list <CString> error);	// Создание записи ошибки сигнала
	
	void testNumWord(errorSheetData& sheet, list<signalData>::iterator it);		// Проверка номера слова
	void testTitleParam(errorSheetData& sheet, list<signalData>::iterator it);	// Проверка наименований сигнала
	void testMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator it);	// Проверка минимального, максимального и цср
	void testBits(errorSheetData& sheet, list<signalData>::iterator it);		// Проверка используемых разрядов
	void testComment(errorSheetData& sheet, list<signalData>::iterator it);		// Проверка коментариев

	string convertString(CString cStr);	// Конвертер CString в string 
};

