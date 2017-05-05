#pragma once

#ifdef WORK_EXCEL_EXPORTS
#define WORK_EXCEL_API __declspec(dllexport)
#else
#define WORK_EXCEL_API __declspec(dllimport)
#endif

#include "excel\CApplication.h"
#include "excel\CRange.h"
#include "excel\CWorkbook.h"
#include "excel\CWorkbooks.h"
#include "excel\CWorksheet.h"
#include "excel\CWorksheets.h"
#include "excel\Enum.h"

#include <cstring>

// ����� ������
struct Cell {
	long row;
	long column;
};

class WORK_EXCEL_API CWorkExcel {
public:
	CWorkExcel(void);						// �����������
	CWorkExcel(CString& pathToTemplate);	// ����������� � ����������
	~CWorkExcel();							// ����������

	void openWorkBook(CString& pathToTemplate);		// �������� ������� �����
	void closeWorkBooks();							// �������� ���� ����

	void setActivBook(long& iBook);					// ������� �������� �����
	void setActivSheet(long& iSheet);				// ������� ��������� �����
	
	CString getNameBook();							// ��������� �������� �����
	CString getNameSheet();							// ��������� �������� �����
	
	int getCountBooks();							// ��������� ���-�� �������� ����
	int getCountSheets();							// ��������� ���-�� ������ � �������� �����
	
	int getMergeCount(Cell& cell);						// ���������� ������������ �����
	long getStartMerge(Cell& cell);						// ��������� ������� ����������� �����
	CString getCellValue(Cell& cell);					// ��������� �������� �� ������ 
	bool findOneDateCells(CString& findString, Cell& cell);	// ����� ������ � �������� �����

private:
	CApplication _application = nullptr;	// ������ Excel
	CWorkbooks _excelBooks = nullptr;		// ��� �������� �����
	CWorkbook _workBook = nullptr;			// �������� �����
	CWorksheets _excelSheets = nullptr;		// ��� ����� � �������� �����
	CWorksheet _workSheet = nullptr;		// �������� ���� � �������� �����

	int _countBooks = 0;					// ���������� �������� ����
	int _countSheetsActivBook = 0;			// ���������� ������ � �������� �����

	bool initApplication();					// ������������� ������� Excel � ���������
	void destroyApplication();				// ����������� ������� Excel � ���������
	void setCountBooks();					// ������� ���������� �������� ����
	void setCountSheets();					// ������� ���������� ������ � �������� �����

	CString convertToChar(int& iCol);		// ������� ��������� ���������� ����������� ������ �� ������ �������
};

