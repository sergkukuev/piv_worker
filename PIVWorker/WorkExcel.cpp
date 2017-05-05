#include "stdafx.h"
#include "WorkExcel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Инициализация объекта Excel и указателя 
bool CWorkExcel::initApplication()
{

	if (!_application.CreateDispatch(_T("Excel.Application"))) //запустить сервер
	{
		AfxMessageBox(_T("Ошибка при старте Excel!"));
		return false;
	}

	_excelBooks = _application.get_Workbooks(); // указатель на открытые книги

	if (!_excelBooks)
	{
		AfxMessageBox(_T("Ошибка при инициализации указателя на Книги!"));
		ASSERT(_excelBooks != NULL);
		return false;
	}

	return true;
}

// Уничтожение объекта Excel и указателя
void CWorkExcel::destroyApplication()
{
	_excelBooks.Close();
	_excelBooks.ReleaseDispatch();
	_application.Quit();
	_application.ReleaseDispatch();
	_application = nullptr;
	_excelBooks = nullptr;
	_countBooks = 0;
}

//Конструктор
CWorkExcel::CWorkExcel()
{
	initApplication();
}

//Конструктор с параметром
CWorkExcel::CWorkExcel(CString& pathToTemplate)
{
	initApplication();
	openWorkBook(pathToTemplate);
}

// Деструктор
CWorkExcel::~CWorkExcel()
{
	destroyApplication();
}

//Открытие или добавление рабочей книги
void CWorkExcel::openWorkBook(CString& pathToTemplate)
{
	if (!_excelBooks)
		initApplication();

	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	_excelBooks.Open(pathToTemplate, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, 
									covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional);
	setCountBooks();
}

//Закрытие всех книг
void CWorkExcel::closeWorkBooks()
{
	destroyApplication();
}

//Задание активной книги
void CWorkExcel::setActivBook(long& iBook)
{
	_workBook = _excelBooks.get_Item(COleVariant(iBook));
	_excelSheets = _workBook.get_Sheets();
	setCountSheets();
}

//Задание активного листа в книге
void CWorkExcel::setActivSheet(long& iSheet)
{
	_workSheet = _excelSheets.get_Item(COleVariant(iSheet));
}

//Получение названия книги
CString CWorkExcel::getNameBook()
{
	return _workBook.get_Name();
}

//Получение названия листа
CString CWorkExcel::getNameSheet()
{
	return _workSheet.get_Name();
}

//Получение количества открытых книг
int CWorkExcel::getCountBooks()
{
	return _countBooks;
}

//Получение количества листов в активной книге
int CWorkExcel::getCountSheets()
{
	return _countSheetsActivBook;
}

//Получение количества объедененных ячеек
int CWorkExcel::getMergeCount(Cell& cell)
{
	CString rangeCellX;
	int iCol = static_cast<int>(cell.column);

	rangeCellX.Format(_T("%s%d"), convertToChar(iCol), cell.row);
	
	CRange cellRange = _workSheet.get_Range(COleVariant(rangeCellX), COleVariant(rangeCellX));
	
	cellRange = cellRange.get_MergeArea();
	cellRange = cellRange.get_Rows();
	
	return cellRange.get_Count();
}

//Получение индекса стартовой ячейки объединения
long CWorkExcel::getStartMerge(Cell& cell)
{
	CString rangeCellX;
	int iCol = static_cast<int>(cell.column);

	rangeCellX.Format(_T("%s%d"), convertToChar(iCol), cell.row);

	CRange cellRange = _workSheet.get_Range(COleVariant(rangeCellX), COleVariant(rangeCellX));

	cellRange = cellRange.get_MergeArea();
	cellRange = cellRange.get_Rows();

	return cellRange.get_Row();
}

// Получение значения из ячеки
CString CWorkExcel::getCellValue(Cell& cell)
{
	CRange cellRange = _workSheet.get_Cells();

	return cellRange.get_Item(COleVariant(cell.row), COleVariant(cell.column));
}

// Поиск по ячекам в активном листе
bool CWorkExcel::findOneDateCells(CString& findString, Cell& cell)
{
	CRange firstFind = _workSheet.get_Cells();
	CRange Find;

	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	COleVariant covTrue((short)TRUE, VT_BOOL), covFalse((short)FALSE, VT_BOOL);

	Find = firstFind.Find(COleVariant(findString), covOptional, COleVariant(long(xlValues)), COleVariant(long(xlPart)),
							COleVariant(long(xlByRows)), (long)xlNext, covFalse, covFalse, covFalse);
	
	if (Find)
	{
		cell.column = Find.get_Column();
		cell.row = Find.get_Row();
		return true;
	}
	else
		return false;
}

// Установка значения открытых книг
void CWorkExcel::setCountBooks()
{
	_countBooks = _excelBooks.get_Count();
}

//Установка количества листов в активной книги
void CWorkExcel::setCountSheets()
{
	_countSheetsActivBook = _excelSheets.get_Count();
}

//Получения буквенного обозначения ячейки из номера колонки
CString CWorkExcel::convertToChar(int & iCol)
{
	CString rez = _T("");
	int iA = 0, iB = 0;

	if (iCol <= 26)
		rez.Format(_T("%c"), static_cast<char>(iCol + 64));
	else
	{
		iA = iCol / 26;
		iB = iCol % 26;

		if (iB == 0)
			iB = 26;
		
		rez.Format(_T("%s%s%s"), rez, convertToChar(iA), static_cast<char>(iB + 64));
	}

	return rez;
}