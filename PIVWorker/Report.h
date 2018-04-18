#pragma once

#include "StructPIV.h"
#include "Settings.h"
#include "Logger.h"
#include "MyException.h"

using namespace stgdll;

#define REPORT_NAME L"Отчет.html"

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
};

// Дополнительная информация о адресе текущего параметра
struct paramInfo
{
	int arincNum;	// Если ARINC, то текущий номер параметра
	bool arinc;		// ARINC || MKIO
	bool dwKprno;	// Двойное слово проекта КПРНО-35
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

	bool GetReport(pivData& data, const bool& isProj);	// Генерация отчета об ошибках
	// Генерация txt файлов
	void GetTxt(list <bookData>& books);	// несколько файлов
	void GetTxt(const bookData& book);		// один файл

private:
	CSettings& settings = CSettings::Instance();			// Указатель на настройки
	logdll::CLogger& logger = logdll::CLogger::Instance(); // Логирование
	bool isProject;		// Метка о создании отчета проекта или отдельных ПИВ (true - проект, false - отдельные протоколы)
	bool bGenRep = true;	// Операция создания отчета завершена с ошибками - false, без ошибок - true 

	// Report.cpp
	void CreateMainFile(pivData& data, CString path);	// Создание основного файла
	bool CreateFolders(const CString& path);			// Создание структуры отчета (создание папочек)
	void MainTable(CStdioFile& file, list <errorData>& db);				// Создание таблицы с проверенными протоколами
	void InfoTable(CStdioFile& file, const amountInfo& amount);			// Таблица общей информации о наборах данных
	void WriteBook(CStdioFile& file, list <errorData>::iterator& it);	// Запись листов текущего протокола
	
	void SheetTableHeader(CStdioFile& file, const CString& book, const CString& sheet, bool arinc);	// Шапка таблицы с ошибками листа
	CString WriteSheets(sheetData* sheet, const vector <errorSignal>& errors, const CString& folder, const CString& bookName);	// Запись таблицы текущего листа
	void WriteSignal(CStdioFile& file, const errorSignal& set);				// Запись одного набора параметров таблицы
	CString FormValue(CString field, const bool& color, const int& width);	// Формирование строки, для записи, одного параметра из набора

	// CSS_STYLES (функции для формирования html файла)
	void ScriptMain(CStdioFile& file);	// Скрипт для скрытия статистики в главном файле отчета
	void CssStyle(CStdioFile& file, const bool& isMain = false); // Запись CSS стиля
	CString CssTable(const bool& isMain);	// Стиль таблицы
	CString CssTh();		// Стиль хедера таблицы
	CString CssTrTd();		// Стиль внутренних блоков таблицы
	CString CssCaption();	// Стиль имени таблицы
	CString CssLinks();		// Стиль ссылок
	CString CssTabs();		// Стиль вкладок
	CString CssClasses1();	// Особенности ячеек основной таблицы отчета
	CString CssClasses2();	// Особености ячеек таблицы ошибок
	CString T(int n);		// Табуляция (для лучшего чтения html файлов)

	CString IntToCString(const int& number);		// Преобразование int в CString
	void WriteFile(CStdioFile&, const CString&);	// Запись текста в файл в формате utf-8
	int CountError(const vector<errorSignal>& set);	// Количество ошибок в текущей таблице
	amountInfo SetAmount(pivData& data);			// Подсчет количества данных( всего, с ошибками и без ошибок), ошибок и предупреждений

	// TxtGenerate.cpp
	void Generate(const bookData& book);	// Генерация txt протокола
	void WriteTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info, const int& arincNum);	// Запись одного набора данных из таблицы в txt файл
	bool WriteParamTitle(ofstream& file, const signalData& signal, const sheetInfo& info);	// Запись наименования параметра
	void WriteParamAdr(ofstream& file, const signalData& signal, const paramInfo& par);		// Запись адреса параметра
	bool IsInt(const double& numeric);	// Проверка на int значение
};
