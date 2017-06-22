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

	void getSheets(vector <sheetData>& book, CWorkExcel& work);			// ������ ������ �� �����
	void getSignals(vector <signalData>& signals, CWorkExcel& work);	// ������ ���������� �� �����

	vector <int> getNumWord(CWorkExcel& work, Cell cell, int& step);	// �������� ������ ����
	void translateNumWord(CString field, vector <int>& numWord);		// ������� ������� ���� �� ������ � �����
	vector <CString> getTitle(CWorkExcel& work, Cell cell, long& step, int& size);
	CString getComment(CWorkExcel& work, Cell cell, int size);


	bool isEmpty(CWorkExcel& work, long row);	// �������� ������ �� �������
	bool isRemark(CWorkExcel& work, long row);	// �������� ������ �� ������� ����������
	bool checkExtension(CString path);			// �������� ���������� ������
};

