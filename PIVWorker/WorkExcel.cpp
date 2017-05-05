#include "stdafx.h"
#include "WorkExcel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ������������� ������� Excel � ��������� 
bool CWorkExcel::initApplication()
{

	if (!_application.CreateDispatch(_T("Excel.Application"))) //��������� ������
	{
		AfxMessageBox(_T("������ ��� ������ Excel!"));
		return false;
	}

	_excelBooks = _application.get_Workbooks(); // ��������� �� �������� �����

	if (!_excelBooks)
	{
		AfxMessageBox(_T("������ ��� ������������� ��������� �� �����!"));
		ASSERT(_excelBooks != NULL);
		return false;
	}

	return true;
}

// ����������� ������� Excel � ���������
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

//�����������
CWorkExcel::CWorkExcel()
{
	initApplication();
}

//����������� � ����������
CWorkExcel::CWorkExcel(CString& pathToTemplate)
{
	initApplication();
	openWorkBook(pathToTemplate);
}

// ����������
CWorkExcel::~CWorkExcel()
{
	destroyApplication();
}

//�������� ��� ���������� ������� �����
void CWorkExcel::openWorkBook(CString& pathToTemplate)
{
	if (!_excelBooks)
		initApplication();

	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	_excelBooks.Open(pathToTemplate, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, 
									covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional);
	setCountBooks();
}

//�������� ���� ����
void CWorkExcel::closeWorkBooks()
{
	destroyApplication();
}

//������� �������� �����
void CWorkExcel::setActivBook(long& iBook)
{
	_workBook = _excelBooks.get_Item(COleVariant(iBook));
	_excelSheets = _workBook.get_Sheets();
	setCountSheets();
}

//������� ��������� ����� � �����
void CWorkExcel::setActivSheet(long& iSheet)
{
	_workSheet = _excelSheets.get_Item(COleVariant(iSheet));
}

//��������� �������� �����
CString CWorkExcel::getNameBook()
{
	return _workBook.get_Name();
}

//��������� �������� �����
CString CWorkExcel::getNameSheet()
{
	return _workSheet.get_Name();
}

//��������� ���������� �������� ����
int CWorkExcel::getCountBooks()
{
	return _countBooks;
}

//��������� ���������� ������ � �������� �����
int CWorkExcel::getCountSheets()
{
	return _countSheetsActivBook;
}

//��������� ���������� ������������ �����
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

//��������� ������� ��������� ������ �����������
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

// ��������� �������� �� �����
CString CWorkExcel::getCellValue(Cell& cell)
{
	CRange cellRange = _workSheet.get_Cells();

	return cellRange.get_Item(COleVariant(cell.row), COleVariant(cell.column));
}

// ����� �� ������ � �������� �����
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

// ��������� �������� �������� ����
void CWorkExcel::setCountBooks()
{
	_countBooks = _excelBooks.get_Count();
}

//��������� ���������� ������ � �������� �����
void CWorkExcel::setCountSheets()
{
	_countSheetsActivBook = _excelSheets.get_Count();
}

//��������� ���������� ����������� ������ �� ������ �������
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