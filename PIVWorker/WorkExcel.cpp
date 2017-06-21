#include "stdafx.h"
#include "WorkExcel.h"

CWorkExcel::CWorkExcel(void)
{
	if (!app.CreateDispatch(_T("Excel.Application"), NULL))
		AfxMessageBox(_T("Не удалось открыть приложение Excel!"));

	app.put_Visible(FALSE);
	app.put_UserControl(FALSE);

	books.AttachDispatch(app.get_Workbooks());
	cells = NULL;
}

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

bool CWorkExcel::openBook(CString path)
{
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	Lp = books.Open(path, covOptional, covOptional, covOptional, covOptional, covOptional,
						  covOptional, covOptional, covOptional, covOptional, covOptional,
						  covOptional, covOptional, covOptional, covOptional);

	if (Lp == NULL)	
		return false;

	book.AttachDispatch(Lp);
	sheets.AttachDispatch(book.get_Sheets());
	count = sheets.get_Count();

	return true;
}

CString CWorkExcel::bookName()
{
	return book.get_Name();
}

bool CWorkExcel::setActiveSheet(long& iSheet)
{
	Lp = sheets.get_Item(COleVariant(iSheet));

	if (Lp == NULL)	
		return false;
	
	sheet.AttachDispatch(Lp);
	Lp = sheet.get_UsedRange();
	range.AttachDispatch(Lp);

	VARIANT tmp = range.get_Value2();

	if (cells != NULL)
		delete cells;

	cells = new COleSafeArray(tmp);
	cells->GetLBound(1, &RowLeft);
	cells->GetUBound(1, &RowRight);
	cells->GetLBound(2, &ColumnLeft);
	cells->GetUBound(2, &ColumnRight);

	return true;
}

CString CWorkExcel::sheetName()
{
	return sheet.get_Name();
}

int CWorkExcel::countSheet()
{
	return count;
}

VARIANT CWorkExcel::cellValue(long& row, long& column)
{
	VARIANT item;
	long index[2] = { row, column };
	cells->GetElement(index, &item);
	return item;
}

int CWorkExcel::countSignal()
{
	long result;
	cells->GetUBound(1, &result);
	return result;
}

/*bool CWorkExcel::findHeader(CString& findString, Cell& cell)
{
	for (int i = ColumnLeft; i < ColumnRight; i++)
	{
		for (int j = RowLeft; j < RowRight; j++)
		{
			Cell c;
			c.row = j; c.column = i;
			CString res = getCellValue(c);
			if (res.Find(findString) != -1)
			{
				cell = c;
				return true;
			}
		}
	}
	return false;
}*/