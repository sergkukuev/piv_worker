#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>

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
	bookData* tmpBook = nullptr;	// Указатель на текущую книгу
	sheetData* tmpSheet = nullptr;	// Указатель на текущий лист

	void getErrors(sheetData* sheet, vector <errorSignal>& syntax, vector <errorSignal>& simantic); // Проверка на ошибки
	void getWarnings(sheetData* sheet, vector <errorSignal>& warning);								// Проверка на замечания

	void initRepiter(vector<repiter>& repit);	// Инициализация репитера для проверки перекрытия
	int isContain(const vector<repiter>& repit, const int& numeric);	// Имеется ли уже такой номер слова (адрес) (в случае удачи возвр. индекс, иначе -1)

	// Синтаксический анализ
	void checkNP(signalData& signal, const int& np, vector <errorSignal>& syntax);	// Проверка номера набора
	bool syntaxValue(const signalData& signal, const bool& arinc, vector <CString>& error);	// Проверка числовых параметров
	bool syntaxTitle(const vector <CString>& title, vector <CString>& error);		// Проверка синтаксиса идентификатора
	bool syntaxBits(const intData& bits, vector <CString>& error);					// Проверка используемых разрядов

	// Семантический анализ
	bool simanticValue(const signalData& signal, vector <CString>& error);			// Проверка минимального, максимального и цср
	bool simanticNumWord(const intData& numeric, const bool& arinc, vector <CString>& error);		// Проверка номера слова
	bool simanticTitle(const int& indx, const CString& title, const bool& flag, vector <CString>& error);	// Проверка наименований сигнала
	bool simanticBits(const signalData& signal, const CString& prevTitle, vector<repiter>& repit, vector <CString>& error);	// Проверка используемых разрядов

	// Вспомогательные функции
	void checkValueByFlag(const int& indx, const bool& flag, vector <CString>& error);					// Доп функция для проверки числовых параметров
	bool checkCrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit); // Проверка перекрытия битов
	bool checkSameTitle(const CString& next, const CString& prev);		// Проверка двух соседних наименований на совпадение
	bool findRepiteInSheet(const CString& title, const int& indx);		// Поиск повторений на листе
	bool findRepiteInBook(const CString& title, const int& indx);		// Поиск повторений в книге
};
