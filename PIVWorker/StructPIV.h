#pragma once

#include <cstring>
#include <list>
#include <vector>

using namespace std;

// Адрес ячейки
struct adrCell
{
	int row;
	int column;
};

//	Индексы параметров сигнала
struct indexHeader
{
	int iRows;
	int iNumber;
	int iName;
	int iNameSignal;
	int iDimension;
	int iMin;
	int iMax;
	int iCSR;
	int iBits;
	int iComments;
};

///////////////////////////////////////////// Хранение ПИВ /////////////////////////////////////////////

//	Данные сигнала
struct signalData
{
	CString sNumWordField;											// Строка с номерами слов
	int iNumWord[2] = { 0, 0 };										// Числовые значения номеров слов (максимум 2 числа)
	bool bNumWordField;												// Присутствие ошибки строке с номером слова
	bool b2NumWordField;											// Присутствие двух номеров слова 
	bool bRepitNumWordField;										// Первый параметр составного слова

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
	bool b2BitField;												// Присутствие двух используемыы категорий 

	list <CString> sCommentField;									// Примечание
	int iBitSigns;													// Знаковая переменная описано в примечании
	bool bCommentField;												// Присутствие знакового описания
	bool bBitSigns;													// Ошибка в описании знакого бита
};

// Листа
struct sheetData
{
	list <signalData> signals;	// Считанная информация о сигналах

	CString nameSheet;			// название листа в книге
	int iCommentFieldNP;		// номер набора параметров
	int iNumPodKadra;			// номер подкадра

	bool bErrorExcelSheet;
	bool bErrorSheet;
};

// Книги
struct bookData
{
	vector <sheetData> sheets;	// Считанные из книги листы
	CString nameBook;			// Название книги
	bool bNumPodKadra;			// Присутствие номера подкадра
};

struct repitParametr
{
	CString sNameBook;			// Имя книги
	CString sNameSheet;			// Имя листа
	int iNumWord[2] = { 0,0 };	// Номер слова в листе
	CString sNameParam;			// Обозначение парамметра
	int iRepit;					// Количество повторений
};

struct repitWord
{
	bool sNumWordField;						// Строка с номерами слов
	bool sTitleParamField[32] = { false };	// Наименование парамметра (максимум 2 строки)
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
	CString nameSheet;				// Название листа
};

// Книги
struct errorBookData
{
	vector <errorSheetData> sheets;	// Листы считанные из книги
	CString nameBook;				// Название книги
};
