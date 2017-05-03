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

// Диструктор
CWorkExcel::~CWorkExcel()
{
	_excelBooks.Close();
	_excelBooks.ReleaseDispatch();
	_application.Quit();
	_application.ReleaseDispatch();
	_application = nullptr;
	_excelBooks = nullptr;
}

//Открытие или добавление рабочей книги
void CWorkExcel::openWorkBook(CString& pathToTemplate)
{
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	_excelBooks.Open(pathToTemplate, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional);
	setCountBooks();
}

void CWorkExcel::closeWorkBook()
{
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	_excelBooks.Close();
	_countBooks = 0;
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
int CWorkExcel::getMergeCount(long& rowIndex, long& columnIndex)
{
	long cellsMerge = 1;
	CString rangeCellX;
	int iCol = static_cast<int>(columnIndex);
	rangeCellX.Format(_T("%s%d"), convertToChar(iCol), rowIndex);
	//CString rangeCellY = rangeCellX;
	CRange cellRange = _workSheet.get_Range(COleVariant(rangeCellX), COleVariant(rangeCellX));
	cellRange = cellRange.get_MergeArea();
	cellRange = cellRange.get_Rows();
	cellsMerge = cellRange.get_Count();
	return cellsMerge;
}

// Получение значения из ячеки
CString CWorkExcel::getCellValue(long& rowIndex, long& columnIndex)
{
	CString cellValue = nullptr;
	CRange cellRange = _workSheet.get_Cells();
	cellValue = cellRange.get_Item(COleVariant(rowIndex), COleVariant(columnIndex));
	return cellValue;
}

// Поиск по ячекам в активном листе
void CWorkExcel::findOneDateCells(CString& findString, int& rowNum, int& columnNum)
{
	CRange firstFind = _workSheet.get_Cells();
	CRange Find;

	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	COleVariant covTrue((short)TRUE, VT_BOOL), covFalse((short)FALSE, VT_BOOL);
	Find = firstFind.Find(COleVariant(findString), covOptional, COleVariant(long(xlValues)), COleVariant(long(xlPart)), COleVariant(long(xlByRows)), (long)xlNext, covFalse, covFalse, covFalse);
	columnNum = -1;
	rowNum = -1;
	if (Find)
	{
		columnNum = Find.get_Column();
		rowNum = Find.get_Row();
	}
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
	{
		rez.Format(_T("%c"), static_cast<char>(iCol + 64));
	}
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