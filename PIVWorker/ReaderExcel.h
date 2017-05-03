#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"

#include <regex>

const int INDEX_NUM = 1;
const int INDEX_NAME = 2;
const int INDEX_SIGNAL = 3;
const int INDEX_DIMENSION = 4;
const int INDEX_MIN = 5;
const int INDEX_MAX = 6;
const int INDEX_CSR = 7;
const int INDEX_BITS = 8;
const int INDEX_COMMENTS = 9;

// ����� ��� ������ ���������� �� excel ������
class CReaderExcel	{
public:
	CReaderExcel();									// �����������
	CReaderExcel(vector <CString> pathToExcel);	// ����������� � ����������
	~CReaderExcel();								// ����������


	bool getBook(CString pathToExcel, bookData& book);				// ������ ����� �����
	bool getBooks(vector <bookData>& books);	// ������ ����
	
	int getSize();		// �������� ���������� ����

private:
	vector <list<CString>> HeaderTable;	// ����� ����������
	indexHeader iHeader;				// ������� ������������ ����������
	bool bHeader;						// ������� �� ���������
	CWorkExcel work;					// ������� �������

	vector <sheetData> getSheets();		// ������ ������
	list <signalData> getSignals();		// ������ ���������� �� �����

	int getNumPK();								// ����� ������ ����� (� ��������� ������ ����� ����� -1)
	bool findHeader();							// ����� �������� ����������
	void setHeader(int index, adrCell cell);	// ��������� ���������

	string convertString(CString cStr);			// ������� �������������� CString � string
};

