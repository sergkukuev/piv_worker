#pragma once

#include <fstream>
#include "StructPIV.h"
#include "MyException.h"

#define SYNTAX_FOLDER _T("\\Error\\Syntax")
#define SIMANTIC_FOLDER _T("\\Error\\Simantic")
#define WARNING_FOLDER _T("\\Warning")

// Количество наборов данных и их ошибок
struct Amount	{
	int all = 0;
	int withError = 0;
	int withoutError = 0;
	int error = 0;
	int warning = 0;
};

// Информация со страницы протокола
struct sheetInfo {
	int np;		// Номер набора
	int pk;		// Номер подкадра
	bool bPK;	// Нужно ли устанавливать номер подкадра
};

// Класс генерации отчета
class CReport {
public:
	CReport();	// Конструктор
	~CReport();	// Деструктор

	void getReport(list <bookData>& books, list <errorSet>& Db, const CString& pathToSave);	// Генерация отчета о ошибках
	void getTxt(list <bookData>& books, const CString& pathToSave, const bool& bNumPK);			// Генерация txt файлов

private:
	CString path;	// Путь для хранения отчетов
	Amount amount;	// Информация о количестве сигналов, с ошибками и без и прочее

	void makeReport(list <bookData>& books, list <errorSet>& errorDB);		// Генерация отчета

	void startWrite(ofstream& file, list <errorSet>& errorDB);			// Запись ошибок в отчет
	void writeSheets(ofstream& file, vector <errorSheet>& set);	// Создание страниц
	CString CReport::writeError(sheetData* sheet, const vector <errorSignal>& errors, const CString& folder, const CString& bookName);
	//void writeError(ofstream& file, const vector <errorSignal>& error, const CString& folder);	// Создание страницы с ошибками
	
	void setAmount(list <bookData>& books);			// Установка количества набора данных (всего, с ошибками, без)
	void setAmountError(list <errorSet>& Db);		// Установка количества ошибок и замечаний

	void hightlightError(ofstream& file, const vector <int>& light, const int& indx, const CString& field);	// Подсветка нужного параметра
	int getIndexErrorField(list<errorSignalData>::iterator it);					// Поиск индекса параметра

	void writeTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info);	// Запись сигнала в txt файл*/
};

