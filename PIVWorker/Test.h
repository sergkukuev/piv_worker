#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>

#define ERROR_DATA _T("Не удалось прочитать значение %s. (Поле содержит недопустимые символы или лишние '.' и ',')")
#define RESERVE_SIGNAL _T("Резерв")
#define MAX_NUMWORD 32
#define MAX_BITS 32

class CTest {
public:
	CTest();	// Конструктор
	~CTest();	// Деструктор

	list <errorSet> Start(list <bookData>& books);	// Проверка на все ошибки

private:
	void getErrors(sheetData* sheet, vector <errorSignal>& syntax, vector <errorSignal>& simantic); // Проверка на ошибки
	void getWarnings(sheetData* sheet, vector <errorSignal>& warning);		// Проверка на замечания

	void checkNP(signalData& signal, const int& np, vector <errorSignal>& syntax);	// Проверка ошибок, связанных с номером набора
	void initRepiter(bool* num, bool** bits);	// Инициализация репитеров
	
	// Синтаксический анализ
	void syntaxValue(const convertError& flags, vector <CString>& error);	// Проверка числовых параметров
	bool syntaxTitle(const vector <CString>& title, vector <CString>& error);		// Проверка синтаксиса идентификатора

	void checkValueByFlag(const CString& field, const int& indx, const bool& flag, vector <CString>& error); // Проверка числовых параметров по набору флагов

	// Семантический анализ
	void simanticNumWord(const vector <int>& numeric, const bool& flag, bool* repiter, vector <CString>& error);		// Проверка номера слова
	void simanticTitle(sheetData* sheet, const CString& title, const bool& flag, vector <CString>& error);				// Проверка наименований сигнала
	void simanticValue(const signalData& signal, vector <CString>& error);						// Проверка минимального, максимального и цср
	void simanticBits(const signalData& signal, bool** repiter, vector <CString>& error);		// Проверка используемых разрядов
	
	bool checkCrossBits(const vector <int>& bits, const vector <int>& numWord, bool** repiter); // Проверка перекрытия битов
	bool findRepiteInSheet(const CString& field, sheetData* sheet);
};
