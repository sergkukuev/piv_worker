#pragma once

#include "StructPIV.h"
#include "Settings.h"
#include "Logger.h"
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
};

// �������������� ���������� � ������ �������� ���������
struct paramInfo
{
	int arincNum;	// ���� ARINC, �� ������� ����� ���������
	bool arinc;		// ARINC || MKIO
	bool dwKprno;	// ������� ����� ������� �����-35
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

	bool GetReport(pivData& data, const bool& isProj);	// ��������� ������ �� �������
	// ��������� txt ������
	void GetTxt(list <bookData>& books);	// ��������� ������
	void GetTxt(const bookData& book);		// ���� ����

private:
	CSettings& settings = CSettings::Instance();			// ��������� �� ���������
	logdll::CLogger& logger = logdll::CLogger::Instance(); // �����������
	bool isProject;		// ����� � �������� ������ ������� ��� ��������� ��� (true - ������, false - ��������� ���������)
	bool bGenRep = true;	// �������� �������� ������ ��������� � �������� - false, ��� ������ - true 

	// Report.cpp
	void CreateMainFile(pivData& data, CString path);	// �������� ��������� �����
	bool CreateFolders(const CString& path);			// �������� ��������� ������ (�������� �������)
	void MainTable(CStdioFile& file, list <errorData>& db);				// �������� ������� � ������������ �����������
	void InfoTable(CStdioFile& file, const amountInfo& amount);			// ������� ����� ���������� � ������� ������
	void WriteBook(CStdioFile& file, list <errorData>::iterator& it);	// ������ ������ �������� ���������
	
	void SheetTableHeader(CStdioFile& file, const CString& book, const CString& sheet, bool arinc);	// ����� ������� � �������� �����
	CString WriteSheets(sheetData* sheet, const vector <errorSignal>& errors, const CString& folder, const CString& bookName);	// ������ ������� �������� �����
	void WriteSignal(CStdioFile& file, const errorSignal& set);				// ������ ������ ������ ���������� �������
	CString FormValue(CString field, const bool& color, const int& width);	// ������������ ������, ��� ������, ������ ��������� �� ������

	// CSS_STYLES (������� ��� ������������ html �����)
	void ScriptMain(CStdioFile& file);	// ������ ��� ������� ���������� � ������� ����� ������
	void CssStyle(CStdioFile& file, const bool& isMain = false); // ������ CSS �����
	CString CssTable(const bool& isMain);	// ����� �������
	CString CssTh();		// ����� ������ �������
	CString CssTrTd();		// ����� ���������� ������ �������
	CString CssCaption();	// ����� ����� �������
	CString CssLinks();		// ����� ������
	CString CssTabs();		// ����� �������
	CString CssClasses1();	// ����������� ����� �������� ������� ������
	CString CssClasses2();	// ���������� ����� ������� ������
	CString T(int n);		// ��������� (��� ������� ������ html ������)

	CString IntToCString(const int& number);		// �������������� int � CString
	void WriteFile(CStdioFile&, const CString&);	// ������ ������ � ���� � ������� utf-8
	int CountError(const vector<errorSignal>& set);	// ���������� ������ � ������� �������
	amountInfo SetAmount(pivData& data);			// ������� ���������� ������( �����, � �������� � ��� ������), ������ � ��������������

	// TxtGenerate.cpp
	void Generate(const bookData& book);	// ��������� txt ���������
	void WriteTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info, const int& arincNum);	// ������ ������ ������ ������ �� ������� � txt ����
	bool WriteParamTitle(ofstream& file, const signalData& signal, const sheetInfo& info);	// ������ ������������ ���������
	void WriteParamAdr(ofstream& file, const signalData& signal, const paramInfo& par);		// ������ ������ ���������
	bool IsInt(const double& numeric);	// �������� �� int ��������
};
