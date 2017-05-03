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
	_excelBooks.Close();
	_excelBooks.ReleaseDispatch();
	_application.Quit();
	_application.ReleaseDispatch();
	_application = nullptr;
	_excelBooks = nullptr;
}

//�������� ��� ���������� ������� �����
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

// ��������� �������� �� �����
CString CWorkExcel::getCellValue(long& rowIndex, long& columnIndex)
{
	CString cellValue = nullptr;
	CRange cellRange = _workSheet.get_Cells();
	cellValue = cellRange.get_Item(COleVariant(rowIndex), COleVariant(columnIndex));
	return cellValue;
}

// ����� �� ������ � �������� �����
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