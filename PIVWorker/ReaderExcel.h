#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"
#include "MyException.h"

struct Header 
{ 
	static const int size = 10;		// ���������� ����������
	vector <list<CString>> list;	// ������ ���������� ���������� 
	int adress[size];				// ������ ������� ��� ����������

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

const int MAX_EMPTY_STRING = 5;	// ����������� ���������� ����� ������ ����� ������


// ����� ��� ������ ���������� �� excel ������
class CReaderExcel	{
public:
	CReaderExcel();		// �����������
	~CReaderExcel();	// ����������


	bookData getBook(CString pathToExcel);			// ������ ����� �����
	vector<bookData> getBooks(vector <CString>);	// ������ ����

private:
	vector <CString> extension;	// ���������� ���������� ������
	Header header;				// ���������� � ����������

	vector <sheetData> getSheets(CWorkExcel& work);		// ������ ������
	list <signalData> getSignals(CWorkExcel& work);		// ������ ���������� �� �����
	CString getCell(CWorkExcel& work, Cell cell, long cName = 1);	// ������ ������

	int getNumPK(CWorkExcel& work);				// ����� ������ ����� (� ��������� ������ ����� ����� -1)
	bool findHeader(CWorkExcel& work);			// ����� �������� ����������

	bool IsEmpty(CWorkExcel& work, long row);	// �������� ������ �� �������
	bool IsRemark(CWorkExcel& work, long row);	// �������� ������ �� ������� ����������
	bool checkExtension(CString path);			// �������� ���������� ������
};

