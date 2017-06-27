#pragma once

#include <cstring>
#include <list>
#include <vector>

using namespace std;

#pragma region Data

#define RESERVE_SIGNAL L"Резерв"
#define REMARKS_SIZE 7
// Флаги ошибки преобразования данных из ячейки
struct convertError {
	bool num = false;	// Для номера слова
	bool min = false;	// Минимум
	bool max = false;	// Максимум
	bool csr = false;	// Цср
	bool bit = false;	// Для битов
};

//	Данные сигнала
struct signalData {
	vector <int> numWord;						// Номера слов
	vector <CString> title = { L"", L""};		// Название параметра и идентификатор
	CString dimension;							// Размерность
	double min = DBL_MIN, max = DBL_MIN,		// Мин, макс
			csr = DBL_MIN;						// Цср
	vector <int> bit;							// Используемые разряды
	CString comment;							// Примечание
	bool bitSign = false;						// Флаг наличия знака

	convertError flags;						// Флаги ошибки преобразования данных ячейки в числа
};

// Лист
struct sheetData {
	vector <signalData> signals;	// Набор параметров на листе

	CString name;		// название листа в книге
	CString line;		// линия передачи
	int np = -1;		// номер набора параметров
	int pk = -1;		// номер подкадра
	bool error = false;	// наличие ошибки на листе	
};

// Книга
struct bookData {
	vector <sheetData> sheets;	// Набор листов в книге
	CString name;				// Название книги
	bool bPK;					// Присутствие номера подкадра
};

#pragma endregion

#pragma region Error

struct errorSignal {
	signalData* signal = nullptr;	// Указатель на данные сигнала с ошибками
	vector <CString> error;			// Набор ошибок параметра
};

struct errorSheet {
	sheetData* sheet = nullptr;		// Указатель на лист, в котором содержатся данные ошибки
	vector <errorSignal> syntax;	// Синтаксические ошибки
	vector <errorSignal> simantic;	// Семантические ошибки
	vector <errorSignal> warning;	// Предупреждения
};

struct errorSet {
	list <bookData>::iterator book;		// Указатель на книгу, в которой содержатся данные ошибки
	vector <errorSheet> set;	// Наборы ошибок
};

// Шапки таблицы
const CString errRemarks[REMARKS_SIZE] = {
	// Номер параметра
	L"Замечание. Поле \"№ слова\" заполнено не верно.",
	// Обозначение параметра
	L"Замечание. Поле \"Обозначение сигнала\" заполнено не верно.",
	// Проверка мин, макс, ЦСР
	L"Замечание. Поле \"Минимальное значение\" заполнено не верно.",
	L"Замечание. Поле \"Максимальное значение\" заполнено не верно.",
	L"Замечание. Поле \"Цена старшего разряда\" заполнено не верно.",
	// Разряды
	L"Замечание. Поле \"Используемые разряды\" заполнено не верно.",
	L"Замечание. Поле \"Примечание\" заполнено не верно."
};

#pragma endregion