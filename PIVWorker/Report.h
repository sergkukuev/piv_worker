#pragma once

#include <fstream>
#include "StructPIV.h"
#include "MyException.h"

#define SYNTAX_FOLDER L"\\Error\\Syntax"
#define SIMANTIC_FOLDER L"\\Error\\Simantic"
#define WARNING_FOLDER L"\\Warning"
#define OTHER_FOLDER L"\\Other"
#define PROJECT_FOLDER L"\\Project"

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
class PIV_DECLARE CReport {
public:
	CReport();	// �����������
	~CReport();	// ����������

	void getReport(pivData& data, const CString& pathToSave, const bool& isProj);			// ��������� ������ � �������
	void getTxt(list <bookData>& books, const CString& pathToSave, const bool& bNumPK);		// ��������� txt ������
	void getTxt(const bookData& book, const CString& pathToSave, const bool& bNumPK);		// ��������� txt ��� ������ ���������

private:
	bool isProject;	// ����� ��� ������� ��� "���������"
	CString path;	// ���� ��� ���������� ������
	Amount amount;	// ���������� � ���������� ��������, � �������� � ��� � ������

	// ��������� ������ � ����������
	void makeReport(list <errorSet>& Db);	// ��������� ������

	void startWrite(ofstream& file, list <errorSet>& Db);				// ������ ��������� ������ � ����������
	void writeBook(ofstream& file, list <errorSet>::iterator& it);		// ������ ���� ������ �� �����
	void writeSheets(ofstream& file, list <errorSet>::iterator& it);	// ������ ���� ������ � ������ 
	CString writeErrors(sheetData* sheet, const vector <errorSignal>& errors, const CString& folder, const CString& bookName);	// ������ ������ � ������ �����
	void writeSignal(ofstream& file, const errorSignal& set);		// ������ �������
	CString writeParam(const CString& field, const bool& color);	// ������ ��������� �������

	// ��������������� �������
	void errorTable(ofstream& file);				// ������� � ����� ����������� � ���������� ������
	int countError(const vector<errorSignal>& set);	// ������� ���������� ������ �� �����
	void setAmount(list <bookData>& books);			// ��������� ���������� ������ ������ (�����, � ��������, ���)
	void setAmountError(list <errorSet>& Db);		// ��������� ���������� ������ � ���������

	bool findRemark(const vector <CString>& error, const CString& remark);	// ����� ������ ��������� � �������
	bool IsRemark(const CString& field);	// �������� �� ������ ������� ��� ����������

	// ��������� TXT ������
	void writeTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info);	// ������ ������� � txt ����
	void Generate(const bookData& book, const bool& bNumPK);								// ��������� txt ��� �����
};

