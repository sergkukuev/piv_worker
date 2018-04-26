#include "stdafx.h"
#include "WorkExcel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Конструктор
CWorkExcel::CWorkExcel(const CString& path)
{
	Initialize();
	OpenBook(path);
}

// Базовый конструктор
CWorkExcel::CWorkExcel() 
{
	Initialize();
}

// Деструктор
CWorkExcel::~CWorkExcel() 
{
	// TODO: Не запускается десктруктор при преждевременном закрытии приложения
	Destroy();
	CApplication::Quit();
	CApplication::ReleaseDispatch();
}

// Инициализация excel
void CWorkExcel::Initialize()
{
	if (!CApplication::CreateDispatch(L"Excel.Application", NULL))
		throw AccessExcelException();
	CApplication::put_Visible(FALSE);
	CApplication::put_UserControl(FALSE);
}

// Закрытие всех компонентов excel
void CWorkExcel::Destroy()
{
	CWorksheet::ReleaseDispatch();
	CWorksheets::ReleaseDispatch();
	CWorkbook::ReleaseDispatch();
	CWorkbooks::ReleaseDispatch();
	first = last = { 0, 0 };
	delete cells;
}

// Открытие книги
bool CWorkExcel::OpenBook(const CString& path) 
{
	CWorkbooks::AttachDispatch(CApplication::get_Workbooks());
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	LPDISPATCH Lp = CWorkbooks::Open(path, covOptional, covOptional, covOptional, covOptional, covOptional,
						  covOptional, covOptional, covOptional, covOptional, covOptional,
						  covOptional, covOptional, covOptional, covOptional);
	if (Lp == NULL)	
		return false;

	CWorkbook::AttachDispatch(Lp);
	CWorksheets::AttachDispatch(CWorkbook::get_Sheets());
	return true;
}

// Закрытие книги
void CWorkExcel::CloseBook()
{
	Destroy();
}

// Имя книги
CString CWorkExcel::BookName() 
{
	CString result = CWorkbook::get_Name();
	int startPos = result.ReverseFind(L'.');
	result.Delete(startPos, result.GetLength() - startPos);
	return result;
}

// Имя книги из пути файла
CString CWorkExcel::BookName(const CString& path)
{
	return path.Mid(path.ReverseFind(L'\\') + 1, path.ReverseFind(L'.') - path.ReverseFind(L'\\') - 1);
}

// Открытие листа
bool CWorkExcel::OpenSheet(const long& index) 
{
	LPDISPATCH Lp = CWorksheets::get_Item(COleVariant(index));
	if (Lp == NULL)	
		return false;
	
	CWorksheet::AttachDispatch(Lp);
	Lp = CWorksheet::get_UsedRange();
	CRange::AttachDispatch(Lp);

	VARIANT items = CRange::get_Value2();
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

// Имя текущего листа
CString CWorkExcel::SheetName()	{ return CWorksheet::get_Name(); }

// Количество листов в текущей книги
long CWorkExcel::CountSheets()	{ return CWorksheets::get_Count(); }

/// Количество строк на текущем листе
long CWorkExcel::CountRows() { return last.row; }

// Значение ячейки
CString CWorkExcel::CellValue(const Cell& cell) { return CellValue(cell.row, cell.column); }

// Значение ячейки
CString CWorkExcel::CellValue(const long& row, const long& column) 
{
	VARIANT item;
	if (row > last.row || column > last.column || row < first.row || column < first.row)
		throw ExcelOverflow(SheetName());
	long index[2] = { row, column };
	cells->GetElement(index, &item);
	CString result(item);
	// TODO: Поставить проверку результата, иногда сбоит
	result.Trim();
	return result;
}

// Границы листа
CWorkExcel::Cell CWorkExcel::Boundary() { return last; }

// Поиск ячейки по содержимому
// Результат не найден, ячейка (-1,-1)
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

// Количество пустых ячеек выше указанной
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

// Количество пустых ячеек ниже указанной
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
	
	CRange range = CWorksheet::get_Range(COleVariant(cell), COleVariant(cell));
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