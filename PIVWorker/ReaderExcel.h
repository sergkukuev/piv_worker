#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"
#include "MyException.h"

// ����� ��� ������ ���������� �� excel ������
class CReaderExcel	{
public:
	CReaderExcel();		// �����������
	~CReaderExcel();	// ����������

	bookData getBook(CString pathToExcel);			// ������ ����� �����
private:
	vector <CString> extension;	// ���������� ���������� ������
	Header header;				// ���������� � ����������

	vector <sheetData> getSheets(CWorkExcel& work);		// ������ ������
	list <signalData> getSignals(CWorkExcel& work);		// ������ ���������� �� �����
	CString getCell(CWorkExcel& work, Cell cell, long cName = 1);	// ������ ������

	int getNumPK(CWorkExcel& work);				// ����� ������ ����� (� ��������� ������ ����� ����� -1)

	bool isEmpty(CWorkExcel& work, long row);	// �������� ������ �� �������
	bool isRemark(CWorkExcel& work, long row);	// �������� ������ �� ������� ����������
	bool checkExtension(CString path);			// �������� ���������� ������
};

