#pragma once

#include <cstring>
#include <vector>

// ������������� ��������� ������ ��� ������ � Excel
#include "excel\CApplication.h"
#include "excel\CRange.h"
#include "excel\CWorkbook.h"
#include "excel\CWorkbooks.h"
#include "excel\CWorksheet.h"
#include "excel\CWorksheets.h"

#include "MyException.h"

// �������� ���������� ������� ����������
struct Header 
{
	vector <vector<CString>> list;	// ������ ���������� ���������� 
	static const int size = 11;		// ���������� ����������
	long adress[size];				// ������ ����������

	// ������� ���������� � ������� �������:
	const int iRow = 0;			// ������� ������ 
	const int iNumWord = 1;		// ����� �����
	const int iName = 2;		// ������������ ������� 
	const int iSignal = 3;		// ������������� �������
	const int iDimension = 4;	// �����������
	const int iMin = 5;			// ���.����.
	const int iMax = 6;			// ����.����.
	const int iCSR = 7;			// ��� 
	const int iBits = 8;		// ������������ �������
	const int iComment = 9;		// ����������
	const int iAdress = 10;		// ����� (��� arinc)
};

// ������ � ������� Excel
struct Cell 
{
	long row;		// ������
	long column;	// �������
};

// ����� ������ ������ �� ������� Excel 
class PIV_DECLARE CWorkExcel 
{
public:
	CWorkExcel(void);	// �����������
	~CWorkExcel(void);	// ����������
	
	bool OpenBook(const CString& path);		// �������� �����
	// ��������� ����� �����:
	CString BookName(const CString& path);	// �� ����
	CString BookName();						// �� ���������� Excel
	
	bool OpenSheet(const long& index);	// �������� �����
	CString SheetName();		// ��������� ����� �������� �����
	long CountSheets();			// ��������� ���������� ������ � �����
	long CountRows();			// ��������� ���������� ����� �� ������� �����	

	// ��������� �������� ������ 
	CString CellValue(const long& row, const long& column);	
	CString CellValue(const Cell& cell);
	Cell Boundary();	// ��������� ������� �������� excel �����

	// ������ � excel
	long CPrevEmpty(const long& row, const long& column);	// ���������� ������ ����� ��
	long CNextEmpty(const long& row, const long& column);	// ���������� ������ ����� ����� 
	long GetMerge(long& row, const long& column);			// ���-�� ������ �����
	bool FindCell(const CString& field, Cell& cell);		// ����� ������ �� �����������, � ��������� cell(-1,-1)
	bool FindHeader(Header& header, const bool& arinc);		// ����� ���������� �� ������� �����
private:
	CApplication app;	
	CWorkbooks books;	
	CWorkbook book;		
	CWorksheets sheets;	
	CWorksheet sheet;	

	COleSafeArray* cells;	// ������ �������� �����
	Cell first, last;		// �������� ������ � ��������� ������ �������� �����
	CString LongToChar(const long& column);						// �������������� long � char
	void StepLongToChar(const long& column, CString& result);	// ��� �������: ���� � ����������� ������ ��� ������ ����� �����
};

