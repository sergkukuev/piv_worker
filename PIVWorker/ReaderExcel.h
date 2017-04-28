#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"

#include <regex>

// ����� ��� ������ ���������� �� excel ������
class CReaderExcel	{
public:
	CReaderExcel();									// �����������
	CReaderExcel(vector <CString>& pathToExcel);	// ����������� � ����������
	~CReaderExcel();								// ����������

	vector <bookData> getBooks();	// ������ ����
	int getSize();					// �������� ���������� ����

private:
	vector <CString> path;				// ������������ ����
	vector <list<CString>> HeaderTable;	// ����� ����������
	indexHeader iHeader;				// ������� ������������ ����������
	bool bHeader;						// ������� �� ���������
	CWorkExcel work;					// ������� �������

	vector <sheetData> getSheets();		// ������ ������
	list <signalData> getSignals();		// ������ ���������� �� �����

	int getNumPK();								// ����� ������ ����� (� ��������� ������ ����� ����� -1)
	bool findHeader();							// ����� �������� ����������
	void setHeader(int index, adrCell& cell);	// ��������� ���������

	string convertString(CString cStr);			// ������� �������������� CString � string
};

