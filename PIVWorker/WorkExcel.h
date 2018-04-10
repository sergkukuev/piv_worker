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

namespace exceldll
{
	// ������� ���������� � ������� �������
	const enum index {
		iRow = 0,	// ������� ������ 
		iNumWord,	// ����� �����
		iName,		// ������������ ������� 
		iSignal,	// ������������� �������
		iDimension,	// �����������
		iMin,		// ���.����.
		iMax,		// ����.����.
		iCSR,		// ��� 
		iBits,		// ������������ �������
		iComment,	// ����������
		iAdress,	// ����� (��� arinc)
	};
	// �������� ���������� ������� ����������
	struct Header
	{
		vector <vector<CString>> list;	// ������ ���������� ���������� 
		static const int size = 11;		// ���������� ����������
		long adress[size];				// ������ ����������
	};

	// ������ � ������� Excel
	struct Cell
	{
		long row;		// ������
		long column;	// �������
	};
}

using namespace exceldll;

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

