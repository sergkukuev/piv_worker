#pragma once

#include <cstring>
#include <list>
#include <vector>
#include "DLL.h"			// Макрос определения импорта и экспорта DLL

using namespace std;

#pragma region Data

#define RESERVE_SIGNAL L"Резерв"
#define FIELD_SIZE 7

#define NUMWORD_CHECK 0
#define TITLE_CHECK 1
#define MIN_CHECK 2
#define MAX_CHECK 3
#define CSR_CHECK 4
#define BITS_CHECK 5
#define COMMENT_CHECK 6

// Хранение номера слова и битов
struct intData 
{
	vector <int> value;	// Значение
	int sys = 10;		// Система счисления
	CString field;		// Ячейка в исходном виде
	bool flag = false;	// Флаг наличия ошибки при конвертации
};

// Хранение мин, макс и цср
struct doubleData 
{
	double value = DBL_MIN;	// Значение
	CString field;			// Ячейка в исходном виде
	bool flag = false;		// Флаг наличия ошибки при конвертации
};

//	Данные сигнала
struct signalData 
{
	intData numWord;							// Номера слов
	vector <CString> title = { L"", L""};		// Название параметра и идентификатор
	CString dimension;							// Размерность
	doubleData min, max, csr;					// Мин, макс, цср
	intData bit;								// Используемые разряды
	CString comment;							// Примечание
	bool bitSign = false;						// Флаг наличия знака
};

// Лист
struct sheetData 
{
	vector <signalData> signals;	// Набор параметров на листе

	CString name;		// название листа в книге
	CString line;		// линия передачи
	int np = -1;		// номер набора параметров
	int pk = -1;		// номер подкадра
	bool error = false;	// наличие ошибки на листе
	bool arinc = false;	// Тип линии передачи: Arinc(true), мкио(false)
};

// Книга
struct bookData 
{
	vector <sheetData> sheets;	// Набор листов в книге
	CString name;				// Название книги
	bool bPK;					// Присутствие номера подкадра
};

#pragma endregion

#pragma region Error

struct errorSignal 
{
	signalData* signal = nullptr;	// Указатель на данные сигнала с ошибками
	vector <CString> error;			// Набор ошибок параметра
	vector <bool> check = { false, false, false, false, false, false, false };	// Отметки наличия ошибки в параметре
};

struct errorSheet 
{
	sheetData* sheet = nullptr;		// Указатель на лист, в котором содержатся данные ошибки
	vector <errorSignal> syntax;	// Синтаксические ошибки
	vector <errorSignal> simantic;	// Семантические ошибки
	vector <errorSignal> warning;	// Предупреждения
};

struct errorSet 
{
	bookData* book;		// Указатель на книгу, в которой содержатся данные ошибки
	vector <errorSheet> set;	// Наборы ошибок
};

// Шапки таблицы
const CString errRemarks[FIELD_SIZE] = 
{
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

// Структура хранения данных ПИВ
struct pivData 
{
	list <bookData> books;	// Данные о прочитанных ПИВ
	list <errorSet> db;		// База ошибок
};