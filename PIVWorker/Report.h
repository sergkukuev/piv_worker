#pragma once

#include <fstream>

#include "StructPIV.h"
#include "Settings.h"
#include "MyException.h"

using namespace stgdll;

#define REPORT_NAME L"�����.html"

// ���������� � ���������� ������� ������
struct amountInfo	
{
	int all = 0;
	int withError = 0;
	int withoutError = 0;
	int error = 0;
	int warning = 0;
};

// ���������� ������� �������� ���������
struct sheetInfo 
{
	CString name;
	int np;		
	int pk;		
	bool arinc;	// ARINC || MKIO
	bool bPK;
};

// ����� ��������� �������� ����������
// ��������� ��������� �������:
//		- ��������� ������ �� ������� ���������
//		- �������� TXT ������
class PIV_DECLARE CReport 
{
public:
	CReport();	// �����������
	~CReport();	// ����������

	void GetReport(pivData& data, const bool& isProj);			// ��������� ������ �� �������
	// ��������� txt ������
	void GetTxt(list <bookData>& books);		// ��������� ������
	void GetTxt(const bookData& book);		// ���� ����

private:
	CSettings& settings = CSettings::Instance();	// ��������� �� ���������
	bool isProject;		// ����� � �������� ������ ������� ��� ��������� ��� (true - ������, false - ��������� ���������)

	// GENERATE_REPORT
	void CreateMainFile(pivData& data, CString path);	// �������� ��������� �����
	void CreateFolders(const CString& path);	// �������� ��������� ������ (�������� �������)
	void MainTable(CStdioFile& file, list <errorData>& db); // �������� ������� � ������������ �����������
	void InfoTable(CStdioFile& file, const amountInfo& amount);	// ������� ����� ���������� � ������� ������
	void WriteBook(CStdioFile& file, list <errorData>::iterator& it);	// ������ ������ �������� ���������
	
	void SheetTableHeader(CStdioFile& file, const CString& book, const CString& sheet, bool arinc);	// ����� ������� � �������� �����
	CString WriteSheets(sheetData* sheet, const vector <errorSignal>& errors, const CString& folder, const CString& bookName);	// ������ ������� �������� �����
	void WriteSignal(CStdioFile& file, const errorSignal& set);			// ������ ������ ������ ���������� �������
	CString FormValue(CString field, const bool& color, const int& width);	// ������������ ������, ��� ������, ������ ��������� �� ������

	CString IntToCString(const int& number);		// �������������� int � CString

	int CountError(const vector<errorSignal>& set);	// ���������� ������ � ������� �������
	amountInfo SetAmount(pivData& data);			// ������� ���������� ������( �����, � �������� � ��� ������), ������ � ��������������

	// GENERATE_TXT
	void Generate(const bookData& book);	// ��������� txt ���������
	void WriteTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info, const int& arincNum);	// ������ ������ ������ ������ �� ������� � txt ����
	bool IsInt(const double& numeric);	// �������� �� int ��������
};

