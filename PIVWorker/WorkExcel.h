#pragma once
// ������������� ��������� ������ ��� ������ � Excel
#include "excel\CApplication.h"
#include "excel\CRange.h"
#include "excel\CWorkbook.h"
#include "excel\CWorkbooks.h"
#include "excel\CWorksheet.h"
#include "excel\CWorksheets.h"

#include "MyException.h"

// ����� ������� � ������ excel ������ 
class PIV_DECLARE CWorkExcel : private CApplication, private CRange, private CWorkbooks, private CWorkbook, private CWorksheets, private CWorksheet
{
public:
	// �������� excel-�����
	CWorkExcel(const CString&);
	CWorkExcel();
	~CWorkExcel();
	
	// ����� ������ � excel �������
	struct Cell
	{
		long row = 0;		// ������
		long column = 0;	// �������
	};

	// �������� �����
	bool OpenBook(const CString&);
	// �������� �����
	void CloseBook();
	// ��� ����� 
	CString BookName();
	// ��� ����� �� ���� �����
	CString BookName(const CString&);

	// �������� �����
	bool OpenSheet(const long& index);
	// ��� �������� �����
	CString SheetName();		
	// ���������� ������ � ������� �����
	long CountSheets();	
	// ���������� ����� �� ������� �����
	long CountRows();

	// �������� ������
	CString CellValue(const long& row, const long& column);	
	// �������� ������
	CString CellValue(const Cell&);
	// ����� ������ �� �����������
	// ��������� �� ������, ������ (-1,-1)
	bool FindCell(const CString& field, Cell&);	
	// ������� �����
	Cell Boundary();
	// ���������� ������ ����� ���� ���������
	long CPrevEmpty(const long& row, const long& column);
	// ���������� ������ ����� ���� ���������
	long CNextEmpty(const long& row, const long& column);
	// ���������� ������ �����
	long GetMerge(long& row, const long& column);
protected:
	Cell Start() { return first; }	// ������ ������ excel
	Cell End() { return last; }		// ��������� ������ excel
private:
	COleSafeArray* cells = NULL;	// ������ �������� �����
	Cell first;		// ������ ������ �������� �����
	Cell last;		// �������� ������ �������� �����

	void Initialize();	// ������������� excel
	void Destroy();		// �������� ���� ����������� excel
	CString LongToChar(const long& column);		// �������������� long � char
	void StepLongToChar(const long& column, CString& result);	// ��� �������: ���� � ����������� ������ ��� ������ ����� �����
};