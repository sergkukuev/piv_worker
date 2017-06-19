#pragma once

#include <fstream>
#include "StructPIV.h"
#include "MyException.h"

// ������� (��������� � ��������)
#define signal_set(iBook, iSheet) books[iBook].sheets[iSheet].signals

struct Amount	// ��������� �������� ������� ������ � �� ������
{
	int all = 0;
	int withError = 0;
	int withoutError = 0;
	int error = 0;
	int warning = 0;
};

struct sheetInfo	// ���������� �� �������� ���������
{
	int NP;			// ����� ������
	int NumPk;		// ����� ��������
	bool bNumPk;	// ����� �� ������������� ����� ��������
};

// ����� ��������� ������
class CReport
{
public:
	CReport();	// �����������
	~CReport();	// ����������

	void getReport(vector <bookData> books, errorSet errorDB, CString pathToSave);	// ��������� ������ � �������
	void getTxt(vector <bookData> books, CString pathToSave, bool bNumPK);			// ��������� txt ������

private:
	CString path;	// ���� ���������� ������
	Amount amount;	// ���������� ������� ������ � �� ������
	
	void makeReport(vector <bookData> books, errorSet errorDB);		// ��������� ������

	void startWrite(ofstream& file, vector <bookData> books, errorSet errorDB);	// ������ ������ � �����
	void writeSheets(ofstream& file, vector <bookData> books, int cBooks);	// �������� �������
	void writeError(ofstream& file, errorBookData error, CString folder);	// �������� �������� � ��������
	
	void setAmount(vector <bookData> books);			// ��������� ���������� ������ ������ (�����, � ��������, ���)
	int getAmountError(vector <errorBookData> error);	// ���������� ������

	void hightlightError(ofstream& file, int tIndx, int Indx, CString field);	// ��������� ������� ���������
	int getIndexErrorField(list<errorSignalData>::iterator it);					// ����� ������� ���������

	void writeTxtParam(ofstream& file, list <signalData>::iterator it, sheetInfo info);	// ������ ������� � txt ����
};

