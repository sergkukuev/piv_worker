#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"
#include "MyException.h"

#define RESERVE_SIGNAL _T("������")

// ����� ��� ������ ���������� �� excel ������
class CReaderExcel	{
public:
	CReaderExcel();		// �����������
	~CReaderExcel();	// ����������

	bookData getBook(CString pathToExcel);			// ������ ����� �����
private:
	vector <CString> extension;	// ���������� ���������� ������
	Header header;				// ���������� � ����������

	void getSheets(vector <sheetData>& book, CWorkExcel& work);			// ������ ������ �� �����
	void getSignals(vector <signalData>& signals, CWorkExcel& work);	// ������ ���������� �� �����

	void getNumWord(CString field, vector <int>& result);		// �������� ������ ���� �� ������ � �����
	void getDouble(CString field, double& result);				// ��������� �������� double (���� ����)
	void getBits(CString bits, vector <int>& result);			// �������� ������������ �������
	vector <int> CReaderExcel::stepGetBits(CString bits);		// ��� ������� ��� ��������
	CString getComment(CWorkExcel& work, long row, int size);	// ������ ����������


	bool isEmpty(CWorkExcel& work, long row);	// �������� ������ �� �������
	bool isRemark(CWorkExcel& work, long row);	// �������� ������ �� ������� ����������
	bool checkExtension(CString path);			// �������� ���������� ������
};

