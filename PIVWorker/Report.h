#pragma once

#include <fstream>

#include "StructPIV.h"
#include "MyException.h"

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

	void GetReport(pivData& data, const CString& pathToSave, const bool& isProj);			// ��������� ������ �� �������
	// ��������� txt ������
	void GetTxt(list <bookData>& books, const CString& pathToSave, const pivParam& bNumPK);		// ��������� ������
	void GetTxt(const bookData& book, const CString& pathToSave, const pivParam& bNumPK);		// ���� ����

private:
	bool isProject;		// ����� � �������� ������ ������� ��� ��������� ��� (true - ������, false - ��������� ���������)
	CString path;		// ���� ���������� ������

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
	void Generate(const bookData& book, const pivParam& bNumPK);	// ��������� txt ���������
	void WriteTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info, const int& iProject, const int& arincNum);	// ������ ������ ������ ������ �� ������� � txt ����
	bool IsInt(const double& numeric);	// �������� �� int ��������
};

