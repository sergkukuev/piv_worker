#pragma once

#include "StructPIV.h"
#include "Settings.h"
#include "MyException.h"

#include <regex>
#include <set>

#define MAX_BITS 32

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

class PIV_DECLARE CTest 
{
public:
	CTest();	// �����������
	~CTest();	// ����������

	// ������ �������� �� ������
	errorData Start(bookData& book);
	list <errorData> Start(list <bookData>& books);

private:
	bookData* book = nullptr;	// ��������� �� ������� �����
	sheetData* sheet = nullptr;	// ��������� �� ������� ����
	exceptTitle exception;		// ��������� ���������� (�������� � ������������)

	vector <repiter> repit;	// ����� ���������� ����� (��� ������ �����)
	vector <regBase> base;	// ���� ���������� ���������
	const enum index {numword, title, value, bits, /*adress, */size};	// ������� ���������� � ���� ���������� ��������� (value = min, max, csr � ����� �������)

	void Initialize();
	void GetErrors(vector <errorSignal>& syntax, vector <errorSignal>& simantic);	// �������� ����� �� �������������� � ������������� ������
	void GetWarnings(vector <errorSignal>& warning);	// �������� ����� �� �������������� ������ (���������) 
	bool WriteError(errorSignal& signal, CString msg, const int& index);	// ������ ������ 

	// Syntax
	void SyntaxChecker(errorSignal& signal, const int& index);	// �������� ���� ���������� ������� �� �������������� ������
	bool TemplateTest(const CString& field, const int& check, const int& index, errorSignal& signal); // �������� ��������
	bool NpTest(vector <errorSignal>& signals);	// �������� ������ ������ ����������
	bool SimpleTest(errorSignal& signal);		// ������� �������� ������ ���� �������� ����������

	// Simantic
	void SimanticCheker(errorSignal& signal, const int& index, vector <repiter>& repit);	// �������� ���� ���������� ������� �� ������������� ������
	bool ValueTest(errorSignal& signal);	// �������� ���� �������� ����������
	bool TitleRepitTest(errorSignal& signal, const int& index);	// ����� ���������� �������������� �� �����
	bool PartTest(errorSignal& signal);	// �������� �������� ����� (�����35)
	bool BitsTest(errorSignal& signal);	// �������� ������������ ��������
	bool CheckReplace(CString title, const vector <signalData*> signals);	// �������� ����, ������� ��������� �������� � ���� ����� 
	vector<int> CrossBits(const vector <int>& bits, const vector <int>& numWord);		// �������� ���������� �����
	
	// Repiter
	void InitRepiter();		// ������������� �������� ��� �������� ���������� �����
	void ClearRepiter();	// ������� ��������
	void AddRepiter(const int& numWord, const int& index);	// ���������� ������ ������ ����� (������) � �����
	int GetIndexRepiter(const int& numWord);	// �������� ������ ����� ����� �� ������ ����� (� ������ ������� �����. ������, ����� -1)
	
	// ���������
	void FindRepiteTitleInBook(errorSignal& signal, const int& index);	// ����� ���������� �������������� � �����
};