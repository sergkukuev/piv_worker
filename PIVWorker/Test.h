#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>
#include <set>

#define MAX_BITS 32
#define BASE_SIZE 4

// ���������� ������� ����(�������) � �����
typedef struct
{
	int adr;	// ����� ����� (�����)
	bool* bits;	// 0 - ����������� ������ �����(������), 1...MAX_BITS - ����������� �����
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
	errorSet Start(bookData& book);
	list <errorSet> Start(list <bookData>& books);	

private:
	bookData* book = nullptr;	// ��������� �� ������� �����
	sheetData* sheet = nullptr;	// ��������� �� ������� ����
	set<CString> exception;	// ��������� ���������� �������� � ������������

	vector <regBase> base;	// ���� ���������� ���������
	const enum index {numword, title, value, bits, adress};	// ������� ���������� � ���� ���������� ��������� (value = min, max, csr � ����� �������)

	void GetErrors(vector <errorSignal>& syntax, vector <errorSignal>& simantic);	// �������� ����� �� �������������� � ������������� ������
	void GetWarnings(vector <errorSignal>& warning);	// �������� ����� �� �������������� ������ (���������) 

	void InitRepiter(vector<repiter>& repit);	// ������������� �������� ��� �������� ����������
	int IsRepitContain(const vector<repiter>& repit, const int& numeric);	// ������� �� ��� ����� ����� ����� (�����) (� ������ ������� �����. ������, ����� -1)

	// �������������� ������
	bool NpTest(vector <errorSignal>& error);	// �������� ������ ������ ����������
	bool SimpleTest(errorSignal& set);			// ������� �������� ������ ���� �������� ����������
	bool TemplateTest(const CString& field, const int& check, const regBase& test, errorSignal& set); // �������� ��������

	// ������������� ������
	bool ValueTest(errorSignal& set, vector <repiter>& repit);	// �������� ���� �������� ����������
	bool RepiterTest(errorSignal& set, const int& index);		// ����� ���������� �������������� �� �����
	bool BitsTest(errorSignal& set, const CString& prevTitle, vector<repiter>& repit);	// �������� ������������ ��������
	vector<int> CrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit); // �������� ���������� �����
	bool CheckSameTitle(const CString& next, const CString& prev);		// �������� ���� �������� ������������ �� ����������
	
	// ���������
	void FindRepiteTitleInBook(errorSignal& set, const int& index);		// ����� ���������� �������������� � �����

	bool WriteError(errorSignal& result, const CString& msg, const int& index);	// ������ ������ 
};