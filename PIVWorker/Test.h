#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>

#define ERROR_DATA L"Не удалось прочитать значение %s. (Поле пустое или содержит недопустимые символы)"
#define MAX_NUMWORD 32
#define MAX_BITS 32

struct bitRepiter {
	int adr;	// Номер слова или адрес
	bool* bits;	// массив битов для перекрытия
};

class PIV_DECLARE CTest {
public:
	CTest();	// Конструктор
	~CTest();	// Деструктор

	errorSet Start(bookData& book);					// Проверка на ошибки одного протокола
	list <errorSet> Start(list <bookData>& books);	// Проверка на все ошибки

private:
	void getErrors(sheetData* sheet, vector <errorSignal>& syntax, vector <errorSignal>& simantic); // Проверка на ошибки
	void getWarnings(sheetData* sheet, vector <errorSignal>& warning);								// Проверка на замечания

	void checkNP(signalData& signal, const int& np, vector <errorSignal>& syntax);	// Проверка ошибок, связанных с номером набора
	
	void initRepiter(vector <bitRepiter>& repit, sheetData* sheet);							// Инициализация репитера для проверки перекрытия
	bool isContain(const vector<bitRepiter>& repit, const int& numeric);	// Имеется ли уже такой номер слова

	// Синтаксический анализ
	bool syntaxValue(const signalData& signal, vector <CString>& error);			// Проверка числовых параметров
	bool syntaxBits(const intData& bits, vector <CString>& error);					// Проверка используемых разрядов
	bool syntaxTitle(const vector <CString>& title, vector <CString>& error);		// Проверка синтаксиса идентификатора

	void checkValueByFlag(const CString& field, const int& indx, const bool& flag, vector <CString>& error); // Проверка числовых параметров по набору флагов

	// Семантический анализ
	bool simanticNumWord(const intData& numWord,/* bool* repiter,*/ vector <CString>& error);		// Проверка номера слова
	bool simanticTitle(sheetData* sheet, const int& indx, const CString& title, const bool& flag, vector <CString>& error);				// Проверка наименований сигнала
	bool simanticValue(const signalData& signal, vector <CString>& error);						// Проверка минимального, максимального и цср
	bool simanticBits(const signalData& signal, const CString& prevTitle, vector<bitRepiter>& repiter, vector <CString>& error, const bool& arinc);		// Проверка используемых разрядов
	
	bool checkCrossBits(const vector <int>& bits, const vector <int>& numWord, vector<bitRepiter>& repiter, const bool& arinc); // Проверка перекрытия битов
	bool checkTitle(const CString& next, const CString& prev);							// Проверка двух наименований на совпадение
	bool findRepiteInSheet(const CString& field, sheetData* sheet, const int& start);	// Поиск повторений на листе
	int findRepiterIndex(const vector <bitRepiter>& bits, const int& numeric);
};
