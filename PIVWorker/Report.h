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
	CString name;
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

	// GENERATE_REPORT
	void CreateMainFile(pivData& data, CString path);	// Создание основного файла
	void CreateFolders(const CString& path);	// Создание структуры отчета (создание папочек)
	void MainTable(CStdioFile& file, list <errorData>& db); // Создание таблицы с проверенными протоколами
	void InfoTable(CStdioFile& file, const amountInfo& amount);	// Таблица общей информации о наборах данных
	void WriteBook(CStdioFile& file, list <errorData>::iterator& it);	// Запись листов текущего протокола
	
	void SheetTableHeader(CStdioFile& file, const CString& book, const CString& sheet, bool arinc);	// Шапка таблицы с ошибками листа
	CString WriteSheets(sheetData* sheet, const vector <errorSignal>& errors, const CString& folder, const CString& bookName);	// Запись таблицы текущего листа
	void WriteSignal(CStdioFile& file, const errorSignal& set);			// Запись одного набора параметров таблицы
	CString FormValue(CString field, const bool& color, const int& width);	// Формирование строки, для записи, одного параметра из набора

	CString IntToCString(const int& number);		// Преобразование int в CString

	int CountError(const vector<errorSignal>& set);	// Количество ошибок в текущей таблице
	amountInfo SetAmount(pivData& data);			// Подсчет количества данных( всего, с ошибками и без ошибок), ошибок и предупреждений

	// GENERATE_TXT
	void Generate(const bookData& book, const pivParam& bNumPK);	// Генерация txt протокола
	void WriteTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info, const int& iProject, const int& arincNum);	// Запись одного набора данных из таблицы в txt файл
	bool IsInt(const double& numeric);	// Проверка на int значение
};

