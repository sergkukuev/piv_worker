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
	set<CString> exception;	// ��������� ���������� �������� � ������������
	int iMethod;			// ����� ��������

	vector <regBase> base;	// ���� ���������� ���������
	const enum index {numword, title, value, bits, /*adress, */size};	// ������� ���������� � ���� ���������� ��������� (value = min, max, csr � ����� �������)

	void GetErrors(vector <errorSignal>& syntax, vector <errorSignal>& simantic);	// �������� ����� �� �������������� � ������������� ������
	void GetWarnings(vector <errorSignal>& warning);	// �������� ����� �� �������������� ������ (���������) 
	bool WriteError(errorSignal& signal, CString msg, const int& index);	// ������ ������ 
	
	void InitRepiter(vector<repiter>& repit);		// ������������� �������� ��� �������� ����������
	int IsRepitContain(const vector<repiter>& repit, const int& numeric);	// ������� �� ��� ����� ����� ����� (�����) (� ������ ������� �����. ������, ����� -1)

	// Syntax
	void SyntaxChecker(errorSignal& signal, const int& index);	// �������� ���� ���������� ������� �� �������������� ������
	bool TemplateTest(const CString& field, const int& check, const int& index, errorSignal& signal); // �������� ��������
	bool NpTest(vector <errorSignal>& signals);	// �������� ������ ������ ����������
	bool SimpleTest(errorSignal& set);			// ������� �������� ������ ���� �������� ����������

	// Semantic
	void SemanticCheker(errorSignal& signal, const int& index, vector <repiter>& repit);	// �������� ���� ���������� ������� �� ������������� ������
	bool ValueTest(errorSignal& set);	// �������� ���� �������� ����������
	bool RepiterTest(errorSignal& set, const int& index);		// ����� ���������� �������������� �� �����
	bool BitsTest(errorSignal& set, vector<repiter>& repit);	// �������� ������������ ��������
	vector<int> CrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit); // �������� ���������� �����
	bool IsReplaceable(const CString& title, const vector <signalData*> signals);		// �������� �� ������������
	
	// ���������
	void FindRepiteTitleInBook(errorSignal& set, const int& index);		// ����� ���������� �������������� � �����
};