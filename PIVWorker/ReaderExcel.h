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

	void getSheets(vector <sheetData>& book, CWorkExcel& work);		// ������ ������
	void getSignals(vector <signalData>& signals, CWorkExcel& work);		// ������ ���������� �� �����
	CString getCell(CWorkExcel& work, Cell cell);	// ������ ������

	int getNumPK(CWorkExcel& work);				// ����� ������ ����� (� ��������� ������ ����� ����� -1)

	bool isEmpty(CWorkExcel& work, long row);	// �������� ������ �� �������
	bool isRemark(CWorkExcel& work, long row);	// �������� ������ �� ������� ����������
	bool checkExtension(CString path);			// �������� ���������� ������
};

