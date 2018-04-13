#pragma once

#include <cstring>
#include <list>
#include <vector>

using namespace std;

// Обозначения старшей и младшей частей в протоколах КПРНО35
namespace dwPart 
{
	// Размеры массивов должны быть одинаковы
	const vector <CString> low = { L"(мл.ч)", L"(мл.ч.)", L"(мл. ч)", L"(мл. ч.)" };
	const vector <CString> hight = { L"(ст.ч)", L"(ст.ч.)", L"(ст. ч)", L"(ст. ч.)" };
	const vector <CString> hight2 = { L"(cт.ч)", L"(cт.ч.)", L"(cт. ч)", L"(cт. ч.)" }; // Версия старшей части с английской 'c'

	// Проверка на присутствии в строке (мл.ч.)
	static bool checkLow(const CString& str)
	{
		for (size_t i = 0; i < low.size(); i++)
			if (str.Find(low[i]) != -1)
				return true;
		return false;
	}
	// Проверка на присутствии в строке (ст.ч.)
	static bool checkHight(const CString& str)
	{
		for (size_t i = 0; i < hight.size(); i++)
			if (str.Find(hight[i]) != -1 || str.Find(hight2[i]) != -1)
				return true;
		return false;
	}
	// Удаление (ст.ч.) - для генерации txt файлов
	static bool deleleHight(CString& str)
	{
		for (size_t i = 0; i < hight.size(); i++)
		{
			int index = str.Find(hight[i]);
			int index2 = str.Find(hight2[i]);

			if (index != -1)
			{
				str.Delete(index, hight[i].GetLength());
				str.TrimRight();
				return true;
			}
			else if (index2 != -1)
			{
				str.Delete(index2, hight2[i].GetLength());
				str.TrimRight();
				return true;
			}
		}
		return false;
	}
}

#pragma region Data
// Идентификаторы значений таблицы
#define RESERVE_SIGNAL L"Резерв"

// Объединения
const enum check {numword, title, min, max, csr, bits, comment, size}; // Индексы ячеек в таблице
const enum stats { empty = -2 /* отсутствие ячейки (параметра) */, failed /* неудалось прочесть */, opt /* эл-т необязателен */ };		// Статусы чтения параметров

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
	bool repWord = false;						// Флаг повторения сигнала на других листах
	signalData* part = nullptr;					// Указатель на старшую (младшую) часть в КПРНО35
};

// Данные о наборе параметров / индикационном кадре
struct npData
{
	int value = -1;				// Значение
	signalData* data = nullptr;	// Указатель на параметр
};

// Лист
struct sheetData 
{
	vector <signalData> signals;	// Набор параметров на листе

	CString name;		// название листа в книге
	CString line;		// линия передачи
	npData np;			// 930м - номер набора параметров, кпрно - индикационный кадр
	int pk = -1;		// 930м - номер подкадра, кпрно - страница пуи
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
	signalData* data = nullptr;	// Указатель на данные сигнала с ошибками
	vector <CString> error;			// Набор ошибок параметра
	vector <bool> check = { false, false, false, false, false, false, false };	// Отметки наличия ошибки в параметре
};

struct errorSheet 
{
	sheetData* data = nullptr;		// Указатель на лист, в котором содержатся данные ошибки
	vector <errorSignal> syntax;	// Синтаксические ошибки
	vector <errorSignal> simantic;	// Семантические ошибки
	vector <errorSignal> warning;	// Предупреждения
	vector <CString> common;		// Ошибки общего плана (без указателей на сигналы)
};

struct errorData 
{
	bookData* book;		// Указатель на книгу, в которой содержатся данные ошибки
	vector <errorSheet> set;	// Наборы ошибок
};

// Шапки таблицы
const CString errRemarks[check::size] = 
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

// Хранение данных ПИВ
struct pivData 
{
	list <bookData> books;	// Данные о прочитанных ПИВ
	list <errorData> db;		// База ошибок
};