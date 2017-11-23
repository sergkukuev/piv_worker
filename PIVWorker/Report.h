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
	amountInfo amount;	// ���������� � ���������� ������� ������

////////////////////////////////////	��������� ������ � ����������	////////////////////////////////////
	void StartWrite(CStdioFile& file, list <errorSet>& Db);				// ������ ������ ���������
	void WriteBook(CStdioFile& file, list <errorSet>::iterator& it);	// ������ ������ �������� ���������
	CString WriteSheets(sheetData* sheet, const vector <errorSignal>& errors, const CString& folder, const CString& bookName);	// ������ ������� �������� �����
	void WriteSignal(CStdioFile& file, const errorSignal& set);			// ������ ������ ������ ���������� �������
	CString WriteParam(const CString& field, const bool& color, const int& width);	// ������ ������ ��������� �� ������

	CString IntToCString(const int& number);		// �������������� int � CString
	
	void ErrorTable(CStdioFile& file);				// ������� ����� ���������� � ������� ������ (����� amountInfo)
	// ������� ���������� � ������� ������
	int CountError(const vector<errorSignal>& set);	// ���������� ������ � ������� �������
	void SetAmount(list <bookData>& books);			// ������� ���������� ������� ������ (�����, � ��������, ��� ������)
	void SetAmountError(list <errorSet>& Db);		// ������� ������ � ��������������

////////////////////////////////////	��������� TXT ������	////////////////////////////////////
	void Generate(const bookData& book, const pivParam& bNumPK);	// ��������� txt ���������
	void WriteTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info, const int& arincNum);	// ������ ������ ������ ������ �� ������� � txt ����
	
};

