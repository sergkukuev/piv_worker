#pragma once

#include "StructPIV.h"	// Структуры ПИВ
#include <fstream>

// Максрос (обращение к сигналам)
#define signal_set(iBook, iSheet) books[iBook].sheets[iSheet].signals

// Информация со страницы протокола
struct sheetInfo
{
	int NP;			// Номер набора
	int NumPk;		// Номер подкадра
	bool bNumPk;	// Нужно ли устанавливать номер подкадра
};

// Класс генерации txt файлов
class CRepTxt
{
public:
	CRepTxt() {};	// Контруктор
	~CRepTxt() {};	// Деструктор

	void Generate(vector <bookData> books, CString pathToSave, bool bNumPK);	// Начало генерации txt файлов
private:
	void WriteParameter(ofstream& file, list <signalData>::iterator it, sheetInfo info);	// Запись сигнала в txt файл
};

