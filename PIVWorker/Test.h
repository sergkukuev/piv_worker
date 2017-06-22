#pragma once

#include "StructPIV.h"
#include "MyException.h"
#include "ErrBase.h"

class CTest {
public:
	CTest();	// Конструктор
	~CTest();	// Деструктор

	errorOneSet Start(bookData& book);	// Проверка на все ошибки

private:

	CErrBase ErrorBase;	// База ошибок
	errorOneSet error;
	int NP = 0;

	void Syntax(errorBookData& errBook, bookData& book);	// Проверка на синтаксические ошибки
	void Simantic(errorBookData& errBook, bookData& book);	// Проверка на семантические ошибки
	void Warning(errorBookData& errBook, bookData& book);	// Проверка на замечания
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
	bool simanticTitleParam(errorSheetData& sheet, list<signalData>::iterator& it, bookData book, int iSheet);	// Проверка наименований сигнала
	bool simanticMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it, int currNP, bool begin);	// Проверка минимального, максимального и цср
	bool simanticBits(errorSheetData& sheet, list<signalData>::iterator& it, bool tRep[][32]);			// Проверка используемых разрядов

	bool findRepiteInBook(CString field, bookData book);	// Поиск повторений в книге
	bool findRepiteInSheet(CString field, sheetData sheet);	// Поиск повторений в листе
	bool checkCrossBits(list<signalData>::iterator& it, bool repiter[][32]);	// Проверка на перекрытия битов
	void translateNumWord(list<signalData>::iterator& it);	// Перевод № слова из строки в числа
	void translateMMC(list<signalData>::iterator& it);		// Перевод мин, макс и цср
	double stepTranslateMMC(CString value);					// Дополнительная функция для перевода разрядов
	void translateBits(list<signalData>::iterator& it);		// Перевод используемых разрядов из строки в числа
	vector <int> stepTranslateBits(CString bits);			// Дополнительная функция для перевода разрядов
	void translateComment(list<signalData>::iterator& it);	// Перевод значения знака или NP набора из примечания
	int stepTranslateComment(CString field, int indx);		// Дополнительная функция для перевода знака или NP
};

