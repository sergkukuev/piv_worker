#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"
#include "MyException.h"

#include <regex>

const int SIZE_IHEADER = 10;
const int INDEX_ROW = 0;
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
	CReaderExcel();		// �����������
	~CReaderExcel();	// ����������


	bookData getBook(CString pathToExcel);			// ������ ����� �����
	vector<bookData> getBooks(vector <CString>);	// ������ ����

private:
	vector <CString> extension;
	vector <list<CString>> HeaderTable;	// ����� ����������
	int* iHeader;				// ������� ������������ ����������

	vector <sheetData> getSheets(CWorkExcel& work);		// ������ ������
	list <signalData> getSignals(CWorkExcel& work);		// ������ ���������� �� �����
	CString getCell(CWorkExcel& work, adrCell cell, long cName = 1);	// ������ ������

	int getNumPK(CWorkExcel& work);				// ����� ������ ����� (� ��������� ������ ����� ����� -1)
	bool findHeader(CWorkExcel& work);			// ����� �������� ����������
	void setHeader(int index, adrCell cell);	// ��������� ���������

	bool IsEmpty(CWorkExcel& work, long row);	// �������� ������ �� �������
	bool IsRemark(CWorkExcel& work, long row);	// �������� ������ �� ������� ����������

	string convertString(CString cStr);			// ������� �������������� CString � string
	bool checkExtension(CString path);			// �������� ���������� ������
};

