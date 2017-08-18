#pragma once

#include "excel\CApplication.h"
#include "excel\CRange.h"
#include "excel\CWorkbook.h"
#include "excel\CWorkbooks.h"
#include "excel\CWorksheet.h"
#include "excel\CWorksheets.h"
#include "MyException.h"

#include <cstring>
#include <vector>

// ��������� ���������
struct Header 
{
	std::vector <std::vector<CString>> list;	// ������ ���������� ���������� 
	static const int size = 11;		// ���������� ����������
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
	const int iAdress = 10;		//	������ ������ (arinc)
};

// ����� ������
struct Cell 
{
	long row;		// ������
	long column;	// �������
};

#define LINE_FIELD L"�����"
#define NP_FIELD L"NP="
#define PK_FIELD L'�'
#define ARINC_REMARK L"*����������:"
#define REMARK1 L"����������:"
#define REMARK2 L"����������:"
#define ARINC L"���"

// ����� ������ ������ �� Excel 
class PIV_DECLARE CWorkExcel 
{
public:
	CWorkExcel(void);	// �����������
	~CWorkExcel(void);	// ����������
	
	bool openBook(const CString& path);		// �������� �����
	CString bookName();						// ��� ����� �� �����
	CString bookName(const CString& path);	// ��� ����� �� ����

	bool openSheet(const long& index);	// �������� �����
	CString sheetName();				// ��� �����
	long countSheets();					// ���������� ������ � �����
	long countRows();					// ���������� �������� �� ������� �����

	bool isArinc();						// ��������� �������� ����� �������� (arinc ��� ����)
	int npValue(const Header& head);	// ��������� ������ ������
	int pkValue(const Header& head);	// ��������� �������� ������ ��������			

	CString cellValue(const long& row, const long& column);		// ��������� �������� ������
	CString cellValue(const Cell& cell);						// ����������
	long cPrevEmpty(const long& row, const long& column);				// ���������� ������ ����� ��
	long cNextEmpty(const long& row, const long& column);		// ���������� ������ ����� ����� 
	long getMerge(long& row, const long& column);				// ���-�� ������ �����
	bool findHeader(Header& header);							// ����� ���������� �� ������� �����

private:
	CApplication app;	// ���������� excel
	CWorkbooks books;	// �����
	CWorkbook book;		// �����
	CWorksheets sheets;	// ����� � �����
	CWorksheet sheet;	// ����

	COleSafeArray* cells;	// ������ �����
	Cell first, last;		// ������� ������ � ��������� ������

	bool findCell(const CString& field, Cell& cell);	// ����� ������ �� �����������, � ��������� cell(-1,-1)
	CString longToChar(const long& column);				// �������������� long � char
	void stepLongToChar(const long& column, CString& result);	// ���� � ����������� ������ ��� ������ ����� �����
};

