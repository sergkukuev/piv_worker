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
	CString path;	// ���� ��� ���������� ������
	Amount amount;	// ���������� � ���������� ��������, � �������� � ��� � ������

	// ��������� ������ � ����������
	void makeReport(list <bookData>& books, list <errorSet>& Db);	// ��������� ������

	void startWrite(ofstream& file, list <errorSet>& Db);				// ������ ��������� ������ � ����������
	void writeBook(ofstream& file, list <errorSet>::iterator& it);		// ������ ���� ������ �� �����
	void writeSheets(ofstream& file, list <errorSet>::iterator& it);	// ������ ���� ������ � ������ 
	CString writeErrors(sheetData* sheet, const vector <errorSignal>& errors, const CString& folder, const CString& bookName);	// ������ ������ � ������ �����
	void writeSignal(ofstream& file, const errorSignal& set);	// ������ �������

	CString getNumWord(const vector <int>& numWord, const vector <CString>& error);
	CString getTitle(const vector <CString>& title, const vector <CString>& error);
	CString getDouble(const double& value, const vector <CString>& error, const CString& remark);
	CString getComment(const CString& comment, const vector <CString>& error);
	CString getBit(const vector <int>& bit, const vector <CString>& error);

	// ��������������� �������
	void errorTable(ofstream& file);				// ������� � ����� ����������� � ���������� ������
	int countError(const vector<errorSignal>& set);	// ������� ���������� ������ �� �����
	void setAmount(list <bookData>& books);			// ��������� ���������� ������ ������ (�����, � ��������, ���)
	void setAmountError(list <errorSet>& Db);		// ��������� ���������� ������ � ���������

	bool findRemark(const vector <CString>& error, const CString& remark);	// ����� ������ ��������� � �������
	bool IsRemark(const CString& field);	// �������� �� ������ ������� ��� ����������
	void hightlightError(ofstream& file, const vector <int>& light, const int& indx, const CString& field);	// ��������� ������� ���������

	// ��������� TXT ������
	void writeTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info);	// ������ ������� � txt ����*/
};

