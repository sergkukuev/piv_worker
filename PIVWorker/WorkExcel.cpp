﻿#include "stdafx.h"
#include "WorkExcel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace exceldll;

// Конструктор
CWorkExcel::CWorkExcel(void) 
{
	if (!app.CreateDispatch(L"Excel.Application", NULL))
		throw AccessExcelException();
	app.put_Visible(FALSE);
	app.put_UserControl(FALSE);
	books.AttachDispatch(app.get_Workbooks());
	cells = NULL;
}

// Деструктор
CWorkExcel::~CWorkExcel(void) 
{
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();
	book.ReleaseDispatch();
	books.ReleaseDispatch();
	app.Quit();
	app.ReleaseDispatch();
	delete cells;
}

// Открытие книги
bool CWorkExcel::OpenBook(const CString& path) 
{
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
CString CWorkExcel::BookName() 
{
	CString result = book.get_Name();
	int startPos = result.ReverseFind(L'.');
	result.Delete(startPos, result.GetLength() - startPos);
	return result;
}

// Получение имени книги из пути
CString CWorkExcel::BookName(const CString& path) 
{
	int posSlash = path.ReverseFind(L'\\');
	return path.Mid(posSlash + 1, path.GetLength());
}

// Открытие листа
bool CWorkExcel::OpenSheet(const long& index) 
{
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

	// Пустой лист, с ним делать нечего
	if (items.vt == VT_EMPTY || items.vt == VT_NULL)
		return false;
	
	// Получение границ листа
	cells = new COleSafeArray(items);
	cells->GetLBound(1, &first.row);
	cells->GetLBound(2, &first.column);
	cells->GetUBound(1, &last.row);
	cells->GetUBound(2, &last.column);

	return true;
}

// Получение имени текущего листа
CString CWorkExcel::SheetName()	{ return sheet.get_Name(); }

// Получение количества листов в книге
long CWorkExcel::CountSheets()	{ return sheets.get_Count(); }

// Получение значения ячейки
CString CWorkExcel::CellValue(const Cell& cell) { return CellValue(cell.row, cell.column); }

// Получение значения ячейки
CString CWorkExcel::CellValue(const long& row, const long& column) 
{
	VARIANT item;
	if (row > last.row || column > last.column || row < first.row || column < first.row)
		throw ExcelOverflow(SheetName());
	long index[2] = { row, column };
	cells->GetElement(index, &item);
	CString result(item);
	result.Trim();
	return result;
}

// Получение границы страницы excel листа
Cell CWorkExcel::Boundary() { return last; }

// Получение количества строк в листе
long CWorkExcel::CountRows() { return last.row; }

// Поиск заголовков на текущем листе
bool CWorkExcel::FindHeader(Header& header, const bool& bArinc) 
{
	for (size_t i = 0; i < header.list.size(); i++) 
	{
		std::vector<CString>::iterator it = header.list[i].begin();
		bool bFind = false;
		Cell cell = first;
		
		for (it; !bFind; it++) 
		{
			if (!bArinc && i + 1 == iAdress)
				break;
			if ((it == header.list[i].end()) && !bFind)
				return false;
			bFind = FindCell(*it, cell);
		}
		if (!bArinc && i + 1 == iAdress) 
		{
			header.adress[i + 1] = -1;
			continue;
		}
			
		header.adress[iRow] = cell.row;
		header.adress[i + 1] = cell.column;
	}
	return true;
}

// Поиск ячейки по содержимому, в противном cell(-1,-1)
bool CWorkExcel::FindCell(const CString& field, Cell& cell)
{
	for (long i = first.row; i <= last.row; i++) 
		for (long j = first.column; j <= last.column; j++) 
		{
			Cell tmp;
			tmp.row = i; tmp.column = j;
			CString item = CellValue(tmp);

			if (item.CompareNoCase(field) == 0)
			{
				cell = tmp;
				return true;
			}
		}
	return false;
}

// Кол-во пустых ячеек после 
long CWorkExcel::CPrevEmpty(const long& row, const long& column) 
{
	CString field = CellValue(row, column);
	long tmpRow = row;

	// Вверх до первого непустого значения
	while (field.IsEmpty() && tmpRow > first.row)
	{
		tmpRow--;
		field = CellValue(tmpRow, column);
	}

	return row - tmpRow;
}

// Кол-во пустых ячеек после 
long CWorkExcel::CNextEmpty(const long& row, const long& column) 
{
	CString field = CellValue(row, column);
	long tmpRow = row;

	do // Вниз до первого непустого значения
	{
		tmpRow++;
		if (tmpRow <= last.row) 
			field = CellValue(tmpRow, column);
	} 
	while (field.IsEmpty() && tmpRow <= last.row);

	return tmpRow - row;
}

// Количество слитых ячеек
long CWorkExcel::GetMerge(long& row, const long& column) 
{
	CString cell;
	cell.Format(L"%s%d", LongToChar(column), row);
	
	CRange range = sheet.get_Range(COleVariant(cell), COleVariant(cell));
	range = range.get_MergeArea();
	range = range.get_Rows();
	row = range.get_Row();

	return range.get_Count();
}

// Преобразование long к char
CString CWorkExcel::LongToChar(const long& column) 
{
	CString result;
	(column <= 26) ? result.Format(L"%c", static_cast<char>(column + 64)) : StepLongToChar(column, result);
	return result;
}

// Если в обозначении ячейки уже больше одной буквы
void CWorkExcel::StepLongToChar(const long& column, CString& result) 
{
	long iA = column / 26;
	long iB = column % 26;

	if (iB == 0)	
		iB = 26;
	result.Format(L"%s%s%s", result, LongToChar(iA), static_cast<char>(iB + 64));
}