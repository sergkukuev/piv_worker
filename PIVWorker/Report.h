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
	CString path;	// Путь для сохранения отчета
	Amount amount;	// Информация о количестве сигналов, с ошибками и без и прочее

	// ГЕНЕРАЦИЯ ОТЧЕТА О ЗАМЕЧАНИЯХ
	void makeReport(list <bookData>& books, list <errorSet>& Db);	// Генерация отчета

	void startWrite(ofstream& file, list <errorSet>& Db);				// Начало генерации отчета о замечаниях
	void writeBook(ofstream& file, list <errorSet>::iterator& it);		// Запись всех ошибок из книги
	void writeSheets(ofstream& file, list <errorSet>::iterator& it);	// Запись всех ошибок с листов 
	CString writeErrors(sheetData* sheet, const vector <errorSignal>& errors, const CString& folder, const CString& bookName);	// Запись ошибок с одного листа
	void writeSignal(ofstream& file, const errorSignal& set);	// Запись сигнала

	CString getNumWord(const vector <int>& numWord, const vector <CString>& error);
	CString getTitle(const vector <CString>& title, const vector <CString>& error);
	CString getDouble(const double& value, const vector <CString>& error, const CString& remark);
	CString getComment(const CString& comment, const vector <CString>& error);
	CString getBit(const vector <int>& bit, const vector <CString>& error);

	// Вспомогательные функции
	void errorTable(ofstream& file);				// Таблица с общей информацией о количестве ошибок
	int countError(const vector<errorSignal>& set);	// Подсчет количества ошибок на листе
	void setAmount(list <bookData>& books);			// Установка количества набора данных (всего, с ошибками, без)
	void setAmountError(list <errorSet>& Db);		// Установка количества ошибок и замечаний

	bool findRemark(const vector <CString>& error, const CString& remark);	// Найти строку заголовка в ошибках
	bool IsRemark(const CString& field);	// Является ли строка ошибкой или заголовком
	void hightlightError(ofstream& file, const vector <int>& light, const int& indx, const CString& field);	// Подсветка нужного параметра

	// ГЕНЕРАЦИЯ TXT ФАЙЛОВ
	void writeTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info);	// Запись сигнала в txt файл*/
};

