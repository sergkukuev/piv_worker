#pragma once

#ifdef WORK_EXCEL_EXPORTS
#define WORK_EXCEL_API __declspec(dllexport)
#else
#define WORK_EXCEL_API __declspec(dllimport)
#endif

#include "excel\CApplication.h"
#include "excel\CRange.h"
#include "excel\CWorkbook.h"
#include "excel\CWorkbooks.h"
#include "excel\CWorksheet.h"
#include "excel\CWorksheets.h"
#include "excel\Enum.h"

#include <cstring>

// Адрес ячейки
struct Cell {
	long row;
	long column;
};

class WORK_EXCEL_API CWorkExcel {
public:
	CWorkExcel(void);						// конструктор
	CWorkExcel(CString& pathToTemplate);	// конструктор с параметром
	~CWorkExcel();							// деструктор

	void openWorkBook(CString& pathToTemplate);		// открытие рабочей книги
	void closeWorkBooks();							// закрытие всех книг

	void setActivBook(long& iBook);					// задание активной книги
	void setActivSheet(long& iSheet);				// задание активного листа
	
	CString getNameBook();							// получение названия книги
	CString getNameSheet();							// получение названия листа
	
	int getCountBooks();							// получение кол-во открытых книг
	int getCountSheets();							// получение кол-ва листов в активной книге
	
	int getMergeCount(Cell& cell);						// количество объедененных ячеек
	long getStartMerge(Cell& cell);						// стартовая позиция объеденения ячеек
	CString getCellValue(Cell& cell);					// получение значения их ячейки 
	bool findOneDateCells(CString& findString, Cell& cell);	// поиск строки в активном листе

private:
	CApplication _application = nullptr;	// объект Excel
	CWorkbooks _excelBooks = nullptr;		// все открытые книги
	CWorkbook _workBook = nullptr;			// активная книга
	CWorksheets _excelSheets = nullptr;		// все листы в активной книге
	CWorksheet _workSheet = nullptr;		// активный лист в активной книге

	int _countBooks = 0;					// количество открытых книг
	int _countSheetsActivBook = 0;			// количество листов в активной книге

	bool initApplication();					// инициализация объекта Excel и указателя
	void destroyApplication();				// уничтожение объекта Excel и указателя
	void setCountBooks();					// задание количества открытых книг
	void setCountSheets();					// задание количества листов в активной книге

	CString convertToChar(int& iCol);		// функция получения буквенного обозначения ячейки из номера колонки
};

