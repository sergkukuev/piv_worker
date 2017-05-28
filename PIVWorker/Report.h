#pragma once

#include <fstream>
#include "StructPIV.h"

struct Amount	// ��������� �������� ������� ������ � �� ������
{
	int allSets = 0;
	int setsWithError = 0;
	int setsWithoutError = 0;
	int error = 0;
	int warning = 0;
};

// ����� �������
const CString errRemarks[7] =
{
	// ����� ���������
	_T("���������. ���� \"� �����\" ��������� �� �����."),
	// ����������� ���������
	_T("���������. ���� \"����������� �������\" ��������� �� �����."),
	// �������� ���, ����, ���
	_T("���������. ���� \"����������� ��������\" ��������� �� �����."),
	_T("���������. ���� \"������������ ��������\" ��������� �� �����."),
	_T("���������. ���� \"���� �������� �������\" ��������� �� �����."),
	// �������
	_T("���������. ���� \"������������ �������\" ��������� �� �����."),
	_T("���������. ���� \"����������\" ��������� �� �����.")
};

// ����� ��������� ������
class CReport
{
public:
	CReport();	// �����������
	~CReport();	// ����������

	bool Generate(vector <bookData> books, errorSet errorDB);	// ��������� ������
	void setPath(CString path);	// ��������� ���� ����������

private:
	CString path;	// ���� ����������
	Amount amount;	// ���������� ������� ������ � �� ������

	vector <bookData> books;	// �����
	errorSet errorDB;			// ���� ������

	void startGenerate();	// ������ ��������� ������
	void makeReport();		// ��������� �� ������� ������
	void makeEmptyReport();	// ��������� ������� ������

	void writeSheets(ofstream& file, int cBooks);		// �������� �������
	void writeSyntaxError(ofstream& file, int cBooks);	// �������� ������� � ��������������� ��������
	void writeSimanticError(ofstream& file, int cBooks);// �������� ������� � �������������� ��������
	void writeWarning(ofstream& file, int cBooks);		// �������� ������� � �����������
	
	void Clear();			// ������� ����������
	int getSetsAmount();	// ���������� ������� ������
	int getSetsAmountWithError();	// ���������� ������� ������ � �������
	int getErrorAmount(vector <errorBookData> error);	// ���������� ������
};

