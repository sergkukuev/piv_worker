#pragma once

#include <fstream>
#include "StructPIV.h"
#include "MyException.h"

#define SYNTAX_FOLDER L"\\Error\\Syntax"
#define SIMANTIC_FOLDER L"\\Error\\Simantic"
#define WARNING_FOLDER L"\\Warning"
#define OTHER_FOLDER L"\\Other"
#define PROJECT_FOLDER L"\\Project"

// Количество наборов данных и их ошибок
struct Amount	
{
	int all = 0;
	int withError = 0;
	int withoutError = 0;
	int error = 0;
	int warning = 0;
};

// Информация со страницы протокола
struct sheetInfo 
{
	int np;		// Номер набора
	int pk;		// Номер подкадра
	bool arinc;	// страница arinc или мкио
	bool bPK;	// Нужно ли устанавливать номер подкадра
};

// Класс генерации отчета
class PIV_DECLARE CReport 
{
public:
	CReport();	// Конструктор
	~CReport();	// Деструктор

	void getReport(pivData& data, const CString& pathToSave, const bool& isProj);			// Генерация отчета о ошибках
	void getTxt(list <bookData>& books, const CString& pathToSave, const bool& bNumPK);		// Генерация txt файлов
	void getTxt(const bookData& book, const CString& pathToSave, const bool& bNumPK);		// Генерация txt для одного протокола

private:
	bool isProject;	// Отчет для проекта или "остальных"
	CString path;	// Путь для сохранения отчета
	Amount amount;	// Информация о количестве сигналов, с ошибками и без и прочее

	// ГЕНЕРАЦИЯ ОТЧЕТА О ЗАМЕЧАНИЯХ
	void makeReport(list <errorSet>& Db);	// Генерация отчета

	void startWrite(CStdioFile& file, list <errorSet>& Db);				// Начало генерации отчета о замечаниях
	void writeBook(CStdioFile& file, list <errorSet>::iterator& it);		// Запись всех ошибок из книги
	void writeSheets(CStdioFile& file, list <errorSet>::iterator& it);	// Запись всех ошибок с листов 
	CString writeErrors(sheetData* sheet, const vector <errorSignal>& errors, const CString& folder, const CString& bookName);	// Запись ошибок с одного листа
	void writeSignal(CStdioFile& file, const errorSignal& set);		// Запись сигнала
	CString writeParam(const CString& field, const bool& color, const int& width);	// Запись параметра сигнала

	// Вспомогательные функции
	void errorTable(CStdioFile& file);				// Таблица с общей информацией о количестве ошибок
	int countError(const vector<errorSignal>& set);	// Подсчет количества ошибок на листе
	void setAmount(list <bookData>& books);			// Установка количества набора данных (всего, с ошибками, без)
	void setAmountError(list <errorSet>& Db);		// Установка количества ошибок и замечаний
	CString IntToCString(const int& number);		// Преобразование int в CString

	// ГЕНЕРАЦИЯ TXT ФАЙЛОВ
	void writeTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info, const int& arincNum);	// Запись сигнала в txt файл
	void Generate(const bookData& book, const bool& bNumPK);								// Генерация txt для книги
};

