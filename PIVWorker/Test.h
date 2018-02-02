#pragma once

#include "StructPIV.h"
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
	set<CString> m930;
	set<CString> arinc;
} exceptTitle;

class PIV_DECLARE CTest 
{
public:
	CTest();	// �����������
	~CTest();	// ����������

	// ������ �������� �� ������
	errorData Start(bookData& book, const int& iMethod);
	list <errorData> Start(list <bookData>& books, const int& iMethod);	

private:
	bookData* book = nullptr;	// ��������� �� ������� �����
	sheetData* sheet = nullptr;	// ��������� �� ������� ����
	exceptTitle exception;		// ��������� ���������� (�������� � ������������)
	int iMethod;				// ����� ��������

	vector <repiter> repit;	// ����� ���������� ����� (��� ������ �����)
	vector <regBase> base;	// ���� ���������� ���������
	const enum index {numword, title, value, bits, /*adress, */size};	// ������� ���������� � ���� ���������� ��������� (value = min, max, csr � ����� �������)

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
	bool ValueTest(errorSignal& set);	// �������� ���� �������� ����������
	bool TitleRepitTest(errorSignal& signal, const int& index);	// ����� ���������� �������������� �� �����
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