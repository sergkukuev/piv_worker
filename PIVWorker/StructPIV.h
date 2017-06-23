#pragma once

#include <cstring>
#include <list>
#include <vector>

using namespace std;

#pragma region Data

//	Данные сигнала
struct signalData {
	vector <int> numWord = {-1, -1};			// Номер слова
	vector <CString> title = { _T(""), _T("")};	// Название параметра и идентификатор
	CString dimension;							// Размерность
	double min = DBL_MIN, max = DBL_MIN,		// Мин, макс
			csr = DBL_MIN;						// Цср
	vector <int> bit = {-1, -1, -1, -1};		// Используемые разряды
	CString comment;							// Примечание
};

// Лист
struct sheetData {
	vector <signalData> signals;	// Набор параметров на листе

	CString name;	// название листа в книге
	CString line;	// линия передачи
	int np;			// номер набора параметров
	int pk;			// номер подкадра
};

// Книга
struct bookData {
	vector <sheetData> sheets;	// Набор листов в книге
	CString name;				// Название книги
	bool bPK;					// Присутствие номера подкадра
};

#pragma endregion

#pragma region Error

struct errorSet {
	bookData* book;		// Указатель на книгу, в которой содержатся данные ошибки
	vector <errorSheet> yupi;	// Наборы ошибок
};

struct errorSheet {
	sheetData* sheet;				// Указатель на лист, в котором содержатся данные ошибки
	vector <errorSignal> syntax;	// Синтаксические ошибки
	vector <errorSignal> simantic;	// Семантические ошибки
	vector <errorSignal> warning;	// Предупреждения
};

struct errorSignal {
	signalData* signal;		// Указатель на данные сигнала с ошибками
	vector <CString> error;	// Набор ошибок параметра
};

// Сигналы
/*struct errorSignalData {
	CString sNumWordField;											// Строка с номерами слов
	CString sTitleParamField[2] = { _T(""), _T("") };				// Наименование парамметра (максимум 2 строки)
	CString sDimensionField;										// Размерность
	CString sMinMaxCsrValField[3] = { _T(""), _T("") , _T("") };	// Строковое значение мин., макс. и цена старшего разряда
	CString sBitField;												// Строковое значение используемые категории
	CString sCommentField;											// Примечание 
	list <CString> sErrorField;										// Вся информация об ошибках
};

// Листы
struct errorSheetData {
	list <errorSignalData> signals;	// Данные сигнала
	CString name;					// Название листа
};

// Книги
struct errorBookData {
	vector <errorSheetData> sheets;	// Листы считанные из книги
	CString name;					// Название книги
};

// Все ошибки
struct errorSet {
	vector <errorBookData> syntax;		// Синтаксические
	vector <errorBookData> simantic;	// Семантические
	vector <errorBookData> warning;		// Замечания
};

struct errorOneSet {
	errorBookData syntax;
	errorBookData simantic;
	errorBookData warning;
};*/

// Шапки таблицы
const CString errRemarks[7] = {
	// Номер параметра
	_T("Замечание. Поле \"№ слова\" заполнено не верно."),
	// Обозначение параметра
	_T("Замечание. Поле \"Обозначение сигнала\" заполнено не верно."),
	// Проверка мин, макс, ЦСР
	_T("Замечание. Поле \"Минимальное значение\" заполнено не верно."),
	_T("Замечание. Поле \"Максимальное значение\" заполнено не верно."),
	_T("Замечание. Поле \"Цена старшего разряда\" заполнено не верно."),
	// Разряды
	_T("Замечание. Поле \"Используемые разряды\" заполнено не верно."),
	_T("Замечание. Поле \"Примечание\" заполнено не верно.")
};

#pragma endregion