#pragma once

#include <fstream>
#include "StructPIV.h"
#include "MyException.h"

// Максрос (обращение к сигналам)
#define signal_set(iBook, iSheet) books[iBook].sheets[iSheet].signals

struct Amount	// Структура подсчета наборов данных и их ошибок
{
	int all = 0;
	int withError = 0;
	int withoutError = 0;
	int error = 0;
	int warning = 0;
};

struct sheetInfo	// Информация со страницы протокола
{
	int NP;			// Номер набора
	int NumPk;		// Номер подкадра
	bool bNumPk;	// Нужно ли устанавливать номер подкадра
};

// Класс генерации отчета
class CReport
{
public:
	CReport();	// Конструктор
	~CReport();	// Деструктор

	void getReport(vector <bookData> books, errorSet errorDB, CString pathToSave);	// Генерация отчета о ошибках
	void getTxt(vector <bookData> books, CString pathToSave, bool bNumPK);			// Генерация txt файлов

private:
	CString path;	// Путь сохранения отчета
	Amount amount;	// Количество наборов данных и их ошибок
	
	void makeReport(vector <bookData> books, errorSet errorDB);		// Генерация отчета

	void startWrite(ofstream& file, vector <bookData> books, errorSet errorDB);	// Запись ошибок в отчет
	void writeSheets(ofstream& file, vector <bookData> books, int cBooks);	// Создание страниц
	void writeError(ofstream& file, errorBookData error, CString folder);	// Создание страницы с ошибками
	
	void setAmount(vector <bookData> books);			// Установка количества набора данных (всего, с ошибками, без)
	int getAmountError(vector <errorBookData> error);	// Количество ошибок

	void hightlightError(ofstream& file, int tIndx, int Indx, CString field);	// Подсветка нужного параметра
	int getIndexErrorField(list<errorSignalData>::iterator it);					// Поиск индекса параметра

	void writeTxtParam(ofstream& file, list <signalData>::iterator it, sheetInfo info);	// Запись сигнала в txt файл
};

