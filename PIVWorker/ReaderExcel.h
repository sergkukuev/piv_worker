#pragma once

#include <cstdlib>
#include <cerrno>

#include "StructPIV.h"
#include "Settings.h"
#include "Logger.h"
#include "WorkExcel.h"
#include "MyException.h"

// ���������� � ���������� � ARINC ����������
struct arincData
{
	CString symbol;		// ������ ������ � ��������������
	int current = 0;	// ������� ����������
	int amount = 0;		// ����� ����������
	int startRow;		// ������ ����� ����������
	bool flag = true;	// ���� ���������� ����������
};

// ����� ��� ������ ���������� �� Excel
class PIV_DECLARE CReaderExcel
{
public:
	CReaderExcel();		// �����������
	~CReaderExcel();	// ����������

	bookData GetBook(const CString& pathToExcel);	// ������ ���������
private:
	stgdll::CSettings& settings = stgdll::CSettings::Instance();	// ��������� �� ���������
	logdll::CLogger& logger = logdll::CLogger::Instance();	// �����������
	vector <CString> extension;	// ���������� ���������� ������
	CWorkExcel work;			// ������ ������������ excel �����
	Header header;				// ���������� � ����������

	void Initialize();
	void GetSheets(vector <sheetData>& sheets);	// ������ ������ ���������� (������)
	void GetSignals(vector <signalData>& signals, const bool& bArinc);	// ������ ���������� �� �����

	void ConcatDW(vector <signalData>& signals);	// ����������� ������� ���� (��������� ���������)
	void findDW(vector<signalData>& signals, size_t start, CString old, vector <CString> revert);	// ����� ������ ����� �������� �����

	// ARINC
	void ArincChecker(arincData& arinc, long& row);	//  ����� ������������� ������
	void GetArinc(arincData& arinc, const long& row, CString field);	// ������ ������ ���������� � ARINC ��������� (���������� ����� � �����)

	// Read_params
	// ������ ����������
	intData GetNumWord(CString field);	// ������ �����
	intData GetAdress(CString field, int current);	// ������
	int GetSubIndex(CString& numeric);				// ��������� ������������ ������� ������ � �������� ��� �� ������
	vector <int> StepAdress(CString adress, bool& flag);		// ������� ���� ������ � �������������� ��� �������� � int
	void GetMinMaxCsr(signalData& signal, const long& row);		// ���, ���� � ���
	intData GetBits(CString field, const int& size);			// ������������ ��������
	vector <int> StepBits(CString bits, bool& flag);			// ������� ���� �������� � �������������� � int
	CString GetComment(long row, const int& size, bool& flag);	// ����������
	
	// Converters
	int GetInt(CString field, bool& flag);		// ��������� int ��������
	double GetDouble(CString field, bool& flag);	// ��������� double ��������

	// Checkers
	bool IsTitle(const long& row);	// �������� ������ �� ���������
	bool IsEmpty(const long& row);	// �������� ������ �� �������
	bool IsRemark(const long& row);	// �������� ������ �� ����������
	bool ExtensionChecker(const CString& path); // �������� ���������� �����
};