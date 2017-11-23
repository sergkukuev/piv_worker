#pragma once

#include <fstream>

#include "StructPIV.h"
#include "MyException.h"

// Информация о количестве наборов данных
struct amountInfo	
{
	int all = 0;
	int withError = 0;
	int withoutError = 0;
	int error = 0;
	int warning = 0;
};

// Информация текущей страницы протокола
struct sheetInfo 
{
	int np;		
	int pk;		
	bool arinc;	// ARINC || MKIO
	bool bPK;
};

// Класс генерации тестовых артефактов
// Выполняет следующие функции:
//		- Генерация отчета об ошибках протокола
//		- Создание TXT файлов
class PIV_DECLARE CReport 
{
public:
	CReport();	// Конструктор
	~CReport();	// Деструктор

	void GetReport(pivData& data, const CString& pathToSave, const bool& isProj);			// Генерация отчета об ошибках
	// Генерация txt файлов
	void GetTxt(list <bookData>& books, const CString& pathToSave, const pivParam& bNumPK);		// несколько файлов
	void GetTxt(const bookData& book, const CString& pathToSave, const pivParam& bNumPK);		// один файл

private:
	bool isProject;		// Метка о создании отчета проекта или отдельных ПИВ (true - проект, false - отдельные протоколы)
	CString path;		// Путь сохранения отчета
	amountInfo amount;	// Информация о количестве наборов данных

////////////////////////////////////	ГЕНЕРАЦИЯ ОТЧЕТА О ЗАМЕЧАНИЯХ	////////////////////////////////////
	void StartWrite(CStdioFile& file, list <errorSet>& Db);				// Начало записи замечаний
	void WriteBook(CStdioFile& file, list <errorSet>::iterator& it);	// Запись листов текущего протокола
	CString WriteSheets(sheetData* sheet, const vector <errorSignal>& errors, const CString& folder, const CString& bookName);	// Запись таблицы текущего листа
	void WriteSignal(CStdioFile& file, const errorSignal& set);			// Запись одного набора параметров таблицы
	CString WriteParam(const CString& field, const bool& color, const int& width);	// Запись одного параметра из набора

	CString IntToCString(const int& number);		// Преобразование int в CString
	
	void ErrorTable(CStdioFile& file);				// Таблица общей информации о наборах данных (вывод amountInfo)
	// Подсчет информации о наборах данных
	int CountError(const vector<errorSignal>& set);	// Количество ошибок в текущей таблице
	void SetAmount(list <bookData>& books);			// Подсчет количества наборов данных (всего, с ошибками, без ошибок)
	void SetAmountError(list <errorSet>& Db);		// Подсчет ошибок и предупреждений

////////////////////////////////////	ГЕНЕРАЦИЯ TXT ФАЙЛОВ	////////////////////////////////////
	void Generate(const bookData& book, const pivParam& bNumPK);	// Генерация txt протокола
	void WriteTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info, const int& arincNum);	// Запись одного набора данных из таблицы в txt файл
	
};

