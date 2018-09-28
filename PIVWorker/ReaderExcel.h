#pragma once
//#include "StructPIV.h"
#include "WorkExcel.h"

#include "Data.h"
#include "Settings.h"
#include "Logger.h"
#include "MyException.h"

// ����� ������ ���������� �� excel �����
class PIV_DECLARE CReader : private CWorkExcel
{
protected:
	CReader();
	~CReader();
	// ������ ���������
	bool ReadBook(const CString& path, CData& data);	
private:
	CSettings& stg = CSettings::Instance();
	CLogger& log = CLogger::Instance();

	// ������� ���������� � ������� �������
	const enum index {
		hRow = 0,	// ������� ������ 
		hNumWord,	// ����� �����
		hName,		// ������������ ������� 
		hSignal,	// ������������� �������
		hDimension,	// �����������
		hMin,		// ���.����.
		hMax,		// ����.����.
		hCSR,		// ��� 
		hBits,		// ������������ �������
		hComment,	// ����������
		hAdress,	// ����� (��� arinc)
		hSize		// ����� ���������� ����������
	};
	// ��������� ������ ����������
	struct Header
	{
		vector <vector<CString>> list;	// ������ ���������� ���������� 
		long adress[hSize];				// ������ ����������
	};
	Header header;	// ��������� ������
	vector <CString> extension = { L"xlsx", L"xlsb", L"xls" };	// ���������� ���������� ������

	// ����������� ��������, ��������� ��� ������ ���
	const vector<CString> remark1 = { L"����������:", L"����������:" };	// ���������� � ����� ������
	const vector<CString> remark2 = { L"*����������:" };				// ���������� � ARINC ��������
	const vector<CString> sign = { L"��-", L"��.", L"��.", L"��-" };	// ������� �������������� �������

	void GetSheets(vector <CData::Sheet>&);			// ������ ������ ���������� (������)
	void GetSignals(vector <CData::Signal>&, vector<CData::DevRemark>&);	// ������ ���������� �� �����
	bool FindHeader(const int& lineType);	// ����� ���������� �� �����

	// DoubleWord (��������� ������� ����)
	void ConcatDW(vector <signalData>& signals);	// ����������� ������� ���� (��������� ���������)
	void findDW(vector<signalData>& signals, size_t start, CString old, vector <CString> revert);	// ����� ������ ����� �������� �����

	// Arinc (������ � ����������� ��� ����� �������� arinc)
	void ArincChecker(CReader::arincData& arinc, long& row);	//  ����� ������������� ������
	void GetArinc(CReader::arincData& arinc, const long& row, CString field);	// ������ ������ ���������� � ARINC ��������� (���������� ����� � �����)
	intData GetAdress(CString field, int current);	// ������ ������
	int GetSubIndex(CString& numeric);				// ��������� ������������ ������� ������ � �������� ��� �� ������
	vector <int> StepAdress(CString adress, bool& flag);	// ������� ���� ������ � �������������� ��� �������� � int

	// SheetInfo (��������� ��� ���������� �������� �����)
	vector<int> ParseValueById(const vector<CData::Signal>&, const vector<CString>& id);	// ��������� �������� �� ����������� �� �������������� (��� ��� � ��)
	int FindSignalById(const vector<CData::Signal>&, const CString& id);	// ����� ������� �� ��������������
	void SetNp(CData::Sheet&);		// �������� ������ ������ ����������
	void SetPuiPage(CData::Sheet&);	// �������� �������� ���
	int GetPk();	// �������� ������ ��������

	// ReadParameters (������ ����������)
	intData GetNumWord(CString field);	// ������ �����
	void GetMinMaxCsr(signalData& signal, const long& row);		// ���, ���� � ���
	intData GetBits(CString field, const int& size);			// ������������ ��������
	vector <int> StepBits(CString bits, bool& flag);			// ������� ���� �������� � �������������� � int
	CString GetComment(long row, const int& size);				// ����������

	// Converters (��������������� ����� � ������ �������)
	int GetInt(CString field, bool& flag);		// ��������� int ��������
	int GetInt(string value, bool& flag);		// ���������� ��� string
	double GetDouble(CString field, bool& flag);	// ��������� double ��������

	// Checkers (��������� ��������)
	bool ExtensionChecker(const CString&); // �������� ���������� �����
	bool IsTitle(const long& row);	// �������� ������ �� ���������
	bool IsEmpty(const long& row);	// �������� ������ �� �������
	bool IsRemark(const long& row);	// �������� ������ �� ����������
	int SetLine();					// ��������� ����� ��������
};