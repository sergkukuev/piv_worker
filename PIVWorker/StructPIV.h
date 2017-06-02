#pragma once

#include <cstring>
#include <list>
#include <vector>

using namespace std;

///////////////////////////////////////////// Хранение ПИВ /////////////////////////////////////////////

//	Данные сигнала
struct signalData
{
	CString sNumWordField;											// Строка с номерами слов
	int iNumWord[2] = { 0, 0 };										// Числовые значения номеров слов (максимум 2 числа)
	bool bNumWordField;												// Присутствие ошибки строке с номером слова
	bool b2NumWordField;											// Присутствие двух номеров слова 

	CString sTitleParamField[2] = { _T(""), _T("") };				// Наименование парамметра (максимум 2 строки) 
	bool bTitleParamField;											// Присутствие ошибки в наименование парамметра  

	CString sDimensionField;										// Размерность
	CString sMinMaxCsrValField[3] = { _T(""), _T("") , _T("") };	// Строковое значение мин., макс. и цена старшего разряда
	double dMinMaxCsrVal[3] = { 0, 0, 0 };							// Значения мин., макс. и цена старшего разряда
	bool bMinValField;												// Присутствие ошибки в мин. 
	bool bMaxValField;												// Присутствие ошибки в макс.
	bool bCsrValField;												// Присутствие ошибки в ЦСР.

	CString sBitField;												// Строковое значение используемые категории
	int iBit[4] = { 0, 0, 0, 0 };									// Используемые категории (максимум 4 числа)
	bool bBitField;													// Присутствие ошибка в используемые категории
	bool b2BitField;												// Присутствие двух используемых категорий 

	CString sCommentField;											// Примечание
	int iBitSigns;													// Знаковая переменная описано в примечании
	bool bCommentField;												// Присутствие знакового описания
	bool bBitSigns;													// Ошибка в описании знакого бита
};

// Листа
struct sheetData
{
	list <signalData> signals;	// Считанная информация о сигналах

	CString name;	// название листа в книге
	int iFieldNP;	// номер набора параметров
	int iNumPK;		// номер подкадра
	bool bError;	// Присутствие ошибки на листе
};

// Книги
struct bookData
{
	vector <sheetData> sheets;	// Считанные из книги листы
	CString name;	// Название книги
	bool bNumPK;	// Присутствие номера подкадра
};

///////////////////////////////////////////// Ошибки ПИВ /////////////////////////////////////////////

// Сигналы
struct errorSignalData
{
	CString sNumWordField;											// Строка с номерами слов
	CString sTitleParamField[2] = { _T(""), _T("") };				// Наименование парамметра (максимум 2 строки)
	CString sDimensionField;										// Размерность
	CString sMinMaxCsrValField[3] = { _T(""), _T("") , _T("") };	// Строковое значение мин., макс. и цена старшего разряда
	CString sBitField;												// Строковое значение используемые категории
	CString sCommentField;											// Примечание 
	list <CString> sErrorField;										// Вся информация об ошибках
};

// Листы
struct errorSheetData
{
	list <errorSignalData> signals;	// Данные сигнала
	CString name;					// Название листа
};

// Книги
struct errorBookData
{
	vector <errorSheetData> sheets;	// Листы считанные из книги
	CString name;					// Название книги
};

// Все ошибки
struct errorSet
{
	vector <errorBookData> syntax;		// Синтаксические
	vector <errorBookData> simantic;	// Семантические
	vector <errorBookData> warning;		// Замечания
};

struct errorOneSet
{
	errorBookData syntax;
	errorBookData simantic;
	errorBookData warning;
};

// Шапки таблицы
const CString errRemarks[7] =
{
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