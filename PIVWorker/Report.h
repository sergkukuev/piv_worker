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
	void writeError(ofstream& file, errorBookData error, CString folder);	// �������� �������� � ��������
	
	void Clear();			// ������� ����������
	int getSetsAmount();	// ���������� ������� ������
	int getSetsAmountWithError();	// ���������� ������� ������ � �������
	int getErrorAmount(vector <errorBookData> error);	// ���������� ������

	void hightlightError(ofstream& file, int tIndx, int Indx, CString field);	// ��������� ������� ���������
	int getIndexErrorField(list<errorSignalData>::iterator it);					// ����� ������� ���������
};

