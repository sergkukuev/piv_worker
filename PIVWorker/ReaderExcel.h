#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"
#include "MyException.h"

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
	CReaderExcel();		// �����������
	~CReaderExcel();	// ����������


	bookData getBook(CString pathToExcel);			// ������ ����� �����
	vector<bookData> getBooks(vector <CString>);	// ������ ����

private:
	vector <CString> extension;
	vector <list<CString>> HeaderTable;	// ����� ����������
	indexHeader iHeader;				// ������� ������������ ����������

	vector <sheetData> getSheets(CWorkExcel& work);		// ������ ������
	list <signalData> getSignals(CWorkExcel& work);		// ������ ���������� �� �����

	int getNumPK(CWorkExcel& work);				// ����� ������ ����� (� ��������� ������ ����� ����� -1)
	bool findHeader(CWorkExcel& work);			// ����� �������� ����������
	void setHeader(int index, adrCell cell);	// ��������� ���������

	string convertString(CString cStr);			// ������� �������������� CString � string
	bool checkPath(CString path);				// �������� ���������� ������
};

