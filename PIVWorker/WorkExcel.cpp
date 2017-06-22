#include "stdafx.h"
#include "WorkExcel.h"

// Конструктор
CWorkExcel::CWorkExcel(void) {
	app.CreateDispatch(_T("Excel.Application"), NULL);
	app.put_Visible(FALSE);
	app.put_UserControl(FALSE);

	books.AttachDispatch(app.get_Workbooks());
	cells = NULL;
}

// Деструктор
CWorkExcel::~CWorkExcel(void) {
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();
	book.ReleaseDispatch();
	books.ReleaseDispatch();
	app.Quit();
	app.ReleaseDispatch();
	delete cells;
}

// Открытие книги
bool CWorkExcel::openBook(CString path) {
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	LPDISPATCH Lp = books.Open(path, covOptional, covOptional, covOptional, covOptional, covOptional,
						  covOptional, covOptional, covOptional, covOptional, covOptional,
						  covOptional, covOptional, covOptional, covOptional);
	if (Lp == NULL)	
		return false;

	book.AttachDispatch(Lp);
	sheets.AttachDispatch(book.get_Sheets());

	return true;
}

// Получение имени книги
CString CWorkExcel::bookName() { return book.get_Name(); }

// Открытие листа
bool CWorkExcel::openSheet(long index) {
	LPDISPATCH Lp = sheets.get_Item(COleVariant(index));

	if (Lp == NULL)	
		return false;
	
	CRange range;
	sheet.AttachDispatch(Lp);
	Lp = sheet.get_UsedRange();
	range.AttachDispatch(Lp);

	VARIANT items = range.get_Value2();

	if (cells != NULL)
		delete cells;

	cells = new COleSafeArray(items);
	
	// Получение границ листа
	cells->GetLBound(1, &first.row);
	cells->GetLBound(2, &first.column);
	cells->GetUBound(1, &last.row);
	cells->GetUBound(2, &last.column);

	return true;
}

// Получение имени текущего листа
CString CWorkExcel::sheetName()	{ return sheet.get_Name(); }

// Получение количества листов в книги
long CWorkExcel::countSheets()	{ return sheets.get_Count(); }

// Получение значения ячейки
VARIANT CWorkExcel::cellValue(Cell cell) {
	VARIANT item;
	long index[2] = { cell.row, cell.column };
	cells->GetElement(index, &item);
	return item;
}

// Перегрузка
VARIANT CWorkExcel::cellValue(long row, long column) {
	VARIANT item;
	long index[2] = { row, column };
	cells->GetElement(index, &item);
	return item;
}

// Получение количества строк в листе
long CWorkExcel::countRows() { return last.row; }

// Поиск заголовков на текущем листе
bool CWorkExcel::findHeader(Header& header) {
	for (size_t i = 0; i < header.list.size(); i++) {

		std::vector<CString>::iterator it = header.list[i].begin();
		bool bFind = false;
		Cell cell = first;
		
		for (it; !bFind; it++) {
			if ((it == header.list[i].end()) && !bFind)
				return false;

			bFind = findCell(*it, cell);
		}

		header.adress[header.iRow] = cell.row;
		header.adress[i + 1] = cell.column;
	}

	return true;
}

// Поиск ячейки по содержимому, в противном cell(-1,-1)
bool CWorkExcel::findCell(CString field, Cell& cell) {
	for (long i = first.row; i <= last.row; i++) {
		for (long j = first.column; j <= last.column; j++) {
			Cell tmp;
			tmp.row = i; tmp.column = j;

			CString item = cellValue(tmp);
			item.Trim();

			if (item.CompareNoCase(field) == 0) {
				cell = tmp;
				return true;
			}
		}
	}

	return false;
}