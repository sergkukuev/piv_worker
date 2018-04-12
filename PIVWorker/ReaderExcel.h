#pragma once

#include <cstdlib>
#include <cerrno>

#include "StructPIV.h"
#include "Settings.h"
#include "Logger.h"
#include "WorkExcel.h"
#include "MyException.h"

#define PK_FIELD L'�'	// ������ ������ ��������
#define ARINC L"���"	// ����� �������� arinc
#define MKIO L"����"	// ����� �������� 

namespace readdll
{
	// ���������� � ���������� � ARINC ����������
	struct arincData
	{
		CString symbol;		// ������ ������ � ��������������
		int current = 0;	// ������� ����������
		int amount = 0;		// ����� ����������
		int startRow;		// ������ ����� ����������
		bool flag = true;	// ���� ���������� ����������
	};
}

// ����� ��� ������ ���������� �� Excel
class PIV_DECLARE CReaderExcel
{
public:
	CReaderExcel();		// �����������
	~CReaderExcel();	// ����������

	bookData GetBook(const CString& pathToExcel);	// ������ ���������
private:
	stgdll::CSettings& settings = stgdll::CSettings::Instance();	// ���������
	logdll::CLogger& logger = logdll::CLogger::Instance();			// �����������
	vector <CString> extension;	// ���������� ���������� ������
	CWorkExcel work;			// ������ ������������ excel �����
	exceldll::Header header;				// ���������� � ����������

	// ����������� ��������, ��������� ��� ������ ���
	const CString line = L"�����";	// ����������� ����� �������� ������
	const CString arincRemark = L"*����������:";	// ����������� ���������� � ARINC
	const vector<CString> remark = { L"����������:", L"����������:" };	// ���������� � ����� �������
	const vector <CString> npId = { L"NP" /* ��� �������, ����� ����35 */, L"IK_MFPI" /* ����35 */ };	// ��������������, � ������� ������ ������ ������� � ���
	const vector <CString> puiId = { L"PagePUI" };			// ��������������, � ������� ������ ������ ������� ��� (��� ��� 35)
	const vector<CString> sign = { L"��-", L"��.", L"��.", L"��-" };	// ������� �������������� �������

	// �������� ������
	void Initialize();	// �������������
	void GetSheets(vector <sheetData>& sheets);	// ������ ������ ���������� (������)
	void GetSignals(vector <signalData>& signals, const bool& bArinc);	// ������ ���������� �� �����

	// DoubleWord (��������� ������� ����)
	void ConcatDW(vector <signalData>& signals);	// ����������� ������� ���� (��������� ���������)
	void findDW(vector<signalData>& signals, size_t start, CString old, vector <CString> revert);	// ����� ������ ����� �������� �����

	// Arinc (������ � ����������� ��� ����� �������� arinc)
	void ArincChecker(readdll::arincData& arinc, long& row);	//  ����� ������������� ������
	void GetArinc(readdll::arincData& arinc, const long& row, CString field);	// ������ ������ ���������� � ARINC ��������� (���������� ����� � �����)
	intData GetAdress(CString field, int current);	// ������ ������
	int GetSubIndex(CString& numeric);				// ��������� ������������ ������� ������ � �������� ��� �� ������
	vector <int> StepAdress(CString adress, bool& flag);	// ������� ���� ������ � �������������� ��� �������� � int

	// SheetInfo (��������� ��� ���������� �������� �����)
	int FindSignalById(const vector<signalData>& signals, const CString& id);	// ����� ������� �� ��������������
	int GetNp(const vector<signalData>& signals);		// �������� ������ ������ ����������
	int GetPuiPage(const vector<signalData>& signals);	// �������� �������� ���
	int ParseValueById(const vector<signalData>&, const vector<CString>& id);	// ��������� �������� �� ����������� �� �������������� (��� ��� � ��)
	int GetPk();	// �������� ������ ��������

	// ReadParameters (������ ����������)
	intData GetNumWord(CString field);	// ������ �����
	void GetMinMaxCsr(signalData& signal, const long& row);		// ���, ���� � ���
	intData GetBits(CString field, const int& size);			// ������������ ��������
	vector <int> StepBits(CString bits, bool& flag);			// ������� ���� �������� � �������������� � int
	CString GetComment(long row, const int& size, bool& flag);	// ����������
	
	// Converters (��������������� ����� � ������ �������)
	int GetInt(CString field, bool& flag);		// ��������� int ��������
	int GetInt(string value, bool& flag);		// ���������� ��� string
	double GetDouble(CString field, bool& flag);	// ��������� double ��������

	// Checkers (��������� ��������)
	bool IsTitle(const long& row);	// �������� ������ �� ���������
	bool IsEmpty(const long& row);	// �������� ������ �� �������
	bool IsRemark(const long& row);	// �������� ������ �� ����������
	bool IsArinc();					// �������� ����� ��������
	bool ExtensionChecker(const CString& path); // �������� ���������� �����
};