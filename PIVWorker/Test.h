#pragma once

#include "StructPIV.h"
#include "MyException.h"
#include "ErrBase.h"

class CTest
{
public:
	CTest();	// Конструктор
	~CTest();	// Деструктор

	errorOneSet Start(bookData& book);	// Проверка на все ошибки

private:

	CErrBase ErrorBase;	// База ошибок
	errorOneSet error;

	void Syntax(errorBookData& errBook, bookData& book);	// Проверка на синтаксические ошибки
	void Simantic(errorBookData& errBook, bookData& book);	// Проверка на семантические ошибки
	list <CString> testField(CString field, errorData errStruct);	// Проверка поля на ошибки
	errorSignalData getErrSignal(list<signalData>::iterator it, list <CString> error);	// Создание записи ошибки сигнала
	
	// Синтаксические
	bool syntaxNumWord(errorSheetData& sheet, list<signalData>::iterator& it);		// Проверка номера слова
	bool syntaxTitleParam(errorSheetData& sheet, list<signalData>::iterator& it);	// Проверка наименований сигнала
	bool syntaxMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it);	// Проверка минимального, максимального и цср
	bool syntaxBits(errorSheetData& sheet, list<signalData>::iterator& it);			// Проверка используемых разрядов
	bool syntaxComment(errorSheetData& sheet, list<signalData>::iterator& it, bool begin);		// Проверка коментариев

	// Семантические
	bool simanticNumWord(errorSheetData& sheet, list<signalData>::iterator& it, bool wRep[]);		// Проверка номера слова
	bool simanticTitleParam(errorSheetData& sheet, list<signalData>::iterator& it);		// Проверка наименований сигнала
	bool simanticMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it);		// Проверка минимального, максимального и цср
	bool simanticBits(errorSheetData& sheet, list<signalData>::iterator& it);			// Проверка используемых разрядов

	bool findRepiteInBook();
	bool findRepiteInSheet();
	void translateNumWord(list<signalData>::iterator& it);	// Перевод № слова из строки в числа
	void translateBits(list<signalData>::iterator& it);		// Перевод используемых разрядов из строки в числа
	vector <int> stepTranslateBits(CString bits);			// Дополнительная функция для перевода разрядов
	void translateBitSign(list<signalData>::iterator& it);	// Перевод значения знака из примечания

	string convertString(CString cStr);	// Конвертер CString в string 
};

