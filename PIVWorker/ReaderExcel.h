#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"

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
};

