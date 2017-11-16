#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>
#include <set>

#define MAX_BITS 32
#define SIZE_BASE 4

// ���������� ������� ����(�������) � �����
struct repiter 
{
	int adr;	// ����� ����� (�����)
	bool* bits;	// 0 - ����������� ������ �����(������), 1...MAX_BITS - ����������� �����
};

// ������������ ���������� ���������
struct regular
{
	string reg;		// ������ ���������
	CString desc;	// ��������� (����������)
};

// ���� ���������� ���������
struct regBase
{
	string correct;
	vector <regular> incorrect;
};

class PIV_DECLARE CTest 
{
public:
	CTest();	// �����������
	~CTest();	// ����������

	// ������ ��������
	errorSet Start(bookData& book);
	list <errorSet> Start(list <bookData>& books);	

private:
	bookData* book = nullptr;	// ��������� �� ������� �����
	sheetData* sheet = nullptr;	// ��������� �� ������� ����

	vector <regBase> base;	// ���� ���������� ���������
	set<CString> exception;	// ��������� ���������� �������� � ������������

	const enum index {numword, title, value, bits, adress};	// ������� ���������� � ���� ���������� ��������� (min, max, csr � ����� �������)

	void GetErrors(vector <errorSignal>& syntax, vector <errorSignal>& simantic);	// �������� �� �������������� � ������������� ������
	void GetWarnings(vector <errorSignal>& warning);	// �������� �� �������������� ������ (���������) 

	void InitRepiter(vector<repiter>& repit);	// ������������� �������� ��� �������� ����������
	int IsContain(const vector<repiter>& repit, const int& numeric);	// ������� �� ��� ����� ����� ����� (�����) (� ������ ������� �����. ������, ����� -1)

	// �������������� ������
	bool CheckNP(vector <errorSignal>& error);	// �������� ������ ������ ����������
	bool SyntaxValue(errorSignal& set);	// �������� ���� �������� ����������
	bool SyntaxTemplate(const CString& field, const int& check, const regBase& test, errorSignal& set); // �������������� �������� ��������� �������\

	// ������������� ������
	bool SimanticValue(errorSignal& set, vector <repiter>& repit);		// �������� ���� �������� ����������
	bool FindRepiteTitleInSheet(errorSignal& set, const int& index);	// ����� ���������� �������������� �� �����
	void FindRepiteTitleInBook(errorSignal& set, const int& index);		// ����� ���������� �������������� � �����
	bool SimanticBits(errorSignal& set, const CString& prevTitle, vector<repiter>& repit);	// �������� ������������ ��������
	vector<int> CheckCrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit); // �������� ���������� �����
	bool CheckSameTitle(const CString& next, const CString& prev);		// �������� ���� �������� ������������ �� ����������

	bool WriteError(errorSignal& result, const CString& msg, const int& index);	// ������ ������ 
};