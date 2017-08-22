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
struct Amount	
{
	int all = 0;
	int withError = 0;
	int withoutError = 0;
	int error = 0;
	int warning = 0;
};

// ���������� �� �������� ���������
struct sheetInfo 
{
	int np;		// ����� ������
	int pk;		// ����� ��������
	bool arinc;	// �������� arinc ��� ����
	bool bPK;	// ����� �� ������������� ����� ��������
};

// ����� ��������� ������
class PIV_DECLARE CReport 
{
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

	void startWrite(CStdioFile& file, list <errorSet>& Db);				// ������ ��������� ������ � ����������
	void writeBook(CStdioFile& file, list <errorSet>::iterator& it);		// ������ ���� ������ �� �����
	void writeSheets(CStdioFile& file, list <errorSet>::iterator& it);	// ������ ���� ������ � ������ 
	CString writeErrors(sheetData* sheet, const vector <errorSignal>& errors, const CString& folder, const CString& bookName);	// ������ ������ � ������ �����
	void writeSignal(CStdioFile& file, const errorSignal& set);		// ������ �������
	CString writeParam(const CString& field, const bool& color, const int& width);	// ������ ��������� �������

	// ��������������� �������
	void errorTable(CStdioFile& file);				// ������� � ����� ����������� � ���������� ������
	int countError(const vector<errorSignal>& set);	// ������� ���������� ������ �� �����
	void setAmount(list <bookData>& books);			// ��������� ���������� ������ ������ (�����, � ��������, ���)
	void setAmountError(list <errorSet>& Db);		// ��������� ���������� ������ � ���������
	CString IntToCString(const int& number);		// �������������� int � CString

	// ��������� TXT ������
	void writeTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info, const int& arincNum);	// ������ ������� � txt ����
	void Generate(const bookData& book, const bool& bNumPK);								// ��������� txt ��� �����
};

