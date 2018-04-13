#pragma once

#include "StructPIV.h"
#include "Settings.h"
#include "Logger.h"
#include "MyException.h"

#include <regex>
#include <set>

#define MAX_BITS 32

namespace testdll
{
	// ���������� ������� ����(�������) � �����
	typedef struct
	{
		int adr;	// ����� ����� (�����)
		vector <signalData*> signals;	// ��������� �� ������� � ������ �������
		vector <bool> bits;	// 0...MAX_BITS - ����������� �����
	} repiter;

	// ������������ ���������� ���������
	typedef struct
	{
		string reg;		// ������ ���������
		CString desc;	// ��������� (����������)
	} regular;

	// ���� ���������� ���������
	typedef struct
	{
		string correct;
		vector <regular> incorrect;
	} regBase;

	// ����������
	typedef struct
	{
		set<CString> mkio;
		set<CString> arinc;
	} exceptTitle;
}

class PIV_DECLARE CTest 
{
public:
	CTest();	// �����������
	~CTest();	// ����������

	// ������ �������� �� ������
	errorData Start(bookData& book);
	list <errorData> Start(list <bookData>& books);

private:
	stgdll::CSettings& settings = stgdll::CSettings::Instance();	// ��������� �� ���������
	logdll::CLogger& logger = logdll::CLogger::Instance();			// �����������
	bookData* book = nullptr;			// ��������� �� ������� �����
	sheetData* sheet = nullptr;			// ��������� �� ������� ����
	testdll::exceptTitle exception;		// ��������� ���������� (�������� � ������������)

	vector <testdll::repiter> repit;	// ����� ���������� ����� (��� ������ �����)
	vector <testdll::regBase> base;	// ���� ���������� ���������
	const enum index {numword, title, value, bits, /*adress, */size};	// ������� ���������� � ���� ���������� ��������� (value = min, max, csr � ����� �������)

	void Initialize();	// �������������
	void GetErrors(errorSheet&);	// �������� ����� �� �������������� � ������������� ������
	void WriteInfoToLog();	// ������ � ���-���� ���������� � �������� ���������
	void WriteBookStats(const errorData&);	// ������ ���������� ��� ������� �����������
	bool WriteError(errorSignal& signal, CString msg, const int& index);	// ������ ������ 
	
	// Syntax
	void SyntaxChecker(errorSignal& signal, const int& index);	// �������� ���� ���������� ������� �� �������������� ������
	bool TemplateTest(const CString& field, const int& check, const int& index, errorSignal& signal); // �������� ��������
	bool SimpleTest(errorSignal& signal);		// ������� �������� ������ ���� �������� ����������

	// Simantic
	void SimanticCheker(errorSignal& signal, const int& index, vector <testdll::repiter>& repit);	// �������� ���� ���������� ������� �� ������������� ������
	bool NpTest(errorSheet&);	// �������� ������ ������ ���������� / �������������� �����
	bool ValueTest(errorSignal& signal);	// �������� ���� �������� ����������
	bool TitleRepitTest(errorSignal& signal, const int& index);	// ����� ���������� �������������� �� �����
	bool PartTest(errorSignal& signal);	// �������� �������� ����� (�����35)
	bool BitsTest(errorSignal& signal);	// �������� ������������ ��������
	bool CheckReplace(CString title, const vector <signalData*> signals);	// �������� ����, ������� ��������� �������� � ���� ����� 
	vector<int> CrossBits(vector <int>& bits, const vector <int>& numWord, const bool& sign);		// �������� ���������� �����

	// Repiter
	void InitRepiter();		// ������������� �������� ��� �������� ���������� �����
	void ClearRepiter();	// ������� ��������
	void AddRepiter(const int& numWord, const int& index);	// ���������� ������ ������ ����� (������) � �����
	int GetIndexRepiter(const int& numWord);	// �������� ������ ����� ����� �� ������ ����� (� ������ ������� �����. ������, ����� -1)
	
	// Warning
	void WarningChecker(errorSignal& signal, const int& index);	// �������� �� �������������� ������ (���������)
	void FindRepiteTitleInBook(errorSignal& signal, const int& index);	// ����� ���������� �������������� � �����
};