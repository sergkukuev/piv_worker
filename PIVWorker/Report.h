#pragma once

#include <fstream>
#include "StructPIV.h"
#include "MyException.h"

#define SYNTAX_FOLDER _T("\\Error\\Syntax")
#define SIMANTIC_FOLDER _T("\\Error\\Simantic")
#define WARNING_FOLDER _T("\\Warning")

// ���������� ������� ������ � �� ������
struct Amount	{
	int all = 0;
	int withError = 0;
	int withoutError = 0;
	int error = 0;
	int warning = 0;
};

// ���������� �� �������� ���������
struct sheetInfo {
	int np;		// ����� ������
	int pk;		// ����� ��������
	bool bPK;	// ����� �� ������������� ����� ��������
};

// ����� ��������� ������
class CReport {
public:
	CReport();	// �����������
	~CReport();	// ����������

	void getReport(list <bookData>& books, list <errorSet>& Db, const CString& pathToSave);	// ��������� ������ � �������
	void getTxt(list <bookData>& books, const CString& pathToSave, const bool& bNumPK);			// ��������� txt ������

private:
	CString path;	// ���� ��� �������� �������
	Amount amount;	// ���������� � ���������� ��������, � �������� � ��� � ������

	void makeReport(list <bookData>& books, list <errorSet>& errorDB);		// ��������� ������

	void startWrite(ofstream& file, list <errorSet>& errorDB);			// ������ ������ � �����
	void writeSheets(ofstream& file, vector <errorSheet>& set);	// �������� �������
	CString CReport::writeError(sheetData* sheet, const vector <errorSignal>& errors, const CString& folder, const CString& bookName);
	//void writeError(ofstream& file, const vector <errorSignal>& error, const CString& folder);	// �������� �������� � ��������
	
	void setAmount(list <bookData>& books);			// ��������� ���������� ������ ������ (�����, � ��������, ���)
	void setAmountError(list <errorSet>& Db);		// ��������� ���������� ������ � ���������

	void hightlightError(ofstream& file, const vector <int>& light, const int& indx, const CString& field);	// ��������� ������� ���������
	int getIndexErrorField(list<errorSignalData>::iterator it);					// ����� ������� ���������

	void writeTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info);	// ������ ������� � txt ����*/
};

