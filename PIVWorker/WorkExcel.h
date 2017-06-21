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
	CWorkExcel(void);
	~CWorkExcel(void);
	
	bool openBook(CString path);
	CString bookName();

	bool setActiveSheet(long& iSheet);
	CString sheetName();
	int countSheet();

	int countSignal();
	VARIANT cellValue(long& row, long& column);

private:
	CApplication app;
	CWorkbooks books;
	CWorkbook book;
	CWorksheets sheets;
	CWorksheet sheet;
	CRange range;
	LPDISPATCH Lp;
	
	// Данные листа
	COleSafeArray* cells;
	int count = 0;
	// Размеры листа
	long RowLeft;
	long RowRight;
	long ColumnLeft;
	long ColumnRight;
};

