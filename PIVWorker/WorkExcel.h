#pragma once

#include "excel\CApplication.h"
#include "excel\CRange.h"
#include "excel\CWorkbook.h"
#include "excel\CWorkbooks.h"
#include "excel\CWorksheet.h"
#include "excel\CWorksheets.h"

#include <cstring>
#include <vector>

// ��������� ���������
struct Header {
	std::vector <std::vector<CString>> list;	// ������ ���������� ���������� 
	static const int size = 10;		// ���������� ����������
	long adress[size];				// ������ ������� ��� ����������

	const int iRow = 0;			//	������ ������ � ������� �������
	const int iNumWord = 1;		//	������ ����� � ������� �������
	const int iName = 2;		//	������ ������������ ������� � ������� �������
	const int iSignal = 3;		//	������ �������������� ������� � ������� �������
	const int iDimension = 4;	//	������ ����������� � ������� �������
	const int iMin = 5;			//	������ ���.����. � ������� �������
	const int iMax = 6;			//	������ ����.����. � ������� �������
	const int iCSR = 7;			//	������ ��� � ������� �������
	const int iBits = 8;		//	������ ����� � ������� �������
	const int iComment = 9;		//	������ ���������� � ������� �������
};

// ����� ������
struct Cell {
	long row;		// ������
	long column;	// �������
};

#define LINE_FIELD _T("�����")
#define NP_FIELD _T("NP=")
#define PK_FIELD _T('�')

// ����� ������ ������ �� Excel 
class CWorkExcel {
public:
	CWorkExcel(void);	// �����������
	~CWorkExcel(void);	// ����������
	
	bool openBook(CString path);	// �������� �����
	CString bookName();				// ��� �����

	bool openSheet(long iSheet);	// �������� �����
	CString sheetName();			// ��� �����
	long countSheets();				// ���������� ������ � �����
	long countRows();				// ���������� �������� �� ������� �����

	VARIANT lineValue();			// ��������� �������� ����� ��������
	int npValue(Header head);		// ��������� ������ ������
	int pkValue(Header head);		// ��������� �������� ������ ��������			

	VARIANT cellValue(long row, long column);	// ��������� �������� ������
	VARIANT cellValue(Cell cell);				// ����������
	int getMerge(long& row, long& column);
	bool findHeader(Header& header);	// ����� ���������� �� ������� �����

private:
	CApplication app;	// ���������� excel
	CWorkbooks books;	// �����
	CWorkbook book;		// �����
	CWorksheets sheets;	// ����� � �����
	CWorksheet sheet;	// ����

	COleSafeArray* cells;	// ������ �����
	Cell first, last;		// ������� ������ � ��������� ������

	bool findCell(CString field, Cell& cell);	// ����� ������ �� �����������, � ��������� cell(-1,-1)
};

