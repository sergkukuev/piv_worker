#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>
#include <set>

#define MAX_BITS 32

#define SIZE_BASE 4		// ���� 4, ����� 5 �����, ��� �������� �������� ������ ������ ��� arinc
#define NUMWORD_BASE 0
#define TITLE_BASE 1
#define VALUE_BASE 2	// min, max, csr � ����� �������
#define BITS_BASE 3
#define ADR_BASE 4

// ��������� ��� �������� ���������� ������� ����(�������) � �����
struct repiter 
{
	int adr;	// ����� ����� (�����)
	bool* bits;	// 0 - ����������� ������ �����(������), 1...MAX_BITS - ����������� �����
};

// ��������� ������������ ��������� � ������������
struct regular
{
	string reg;		// ���������
	CString desc;	// ����������
};

// ��������� ��� ���� ������
struct regexBase 
{
	string correct;		// ���������� ���������
	vector <regular> incorrect;	// ������������ ���������
};

class PIV_DECLARE CTest 
{
public:
	CTest();	// �����������
	~CTest();	// ����������

	errorSet Start(bookData& book);					// �������� �� ������ ������ ���������
	list <errorSet> Start(list <bookData>& books);	// �������� �� ��� ������

private:
	bookData* book = nullptr;	// ��������� �� ������� �����
	sheetData* sheet = nullptr;	// ����������� ������� ����

	vector <regexBase> base;	// ���� ������ ����������
	set<CString> exception;		// ��������� ���������� �������� � ������������

	void getErrors(vector <errorSignal>& syntax, vector <errorSignal>& simantic);	// �������� �� �������������� � ������������� ������
	void getWarnings(vector <errorSignal>& warning);	// �������� �� �������������� ������ (���������) 

	void initRepiter(vector<repiter>& repit);	// ������������� �������� ��� �������� ����������
	int isContain(const vector<repiter>& repit, const int& numeric);	// ������� �� ��� ����� ����� ����� (�����) (� ������ ������� �����. ������, ����� -1)

	// �������������� ������
	bool checkNP(vector <errorSignal>& error);	// �������� ������ ������ ����������
	bool syntaxValue(errorSignal& set);	// �������� ���� �������� ����������
	bool syntaxTemplate(const CString& field, const int& check, const regexBase& test, errorSignal& set); // �������������� �������� ��������� �������\

	// ������������� ������
	bool simanticValue(errorSignal& set, vector <repiter>& repit);		// �������� ���� �������� ����������
	bool findRepiteTitleInSheet(errorSignal& set, const int& index);	// ����� ���������� �������������� �� �����
	void findRepiteTitleInBook(errorSignal& set, const int& index);		// ����� ���������� �������������� � �����
	bool simanticBits(errorSignal& set, const CString& prevTitle, vector<repiter>& repit);	// �������� ������������ ��������
	vector<int> checkCrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit); // �������� ���������� �����
	bool checkSameTitle(const CString& next, const CString& prev);		// �������� ���� �������� ������������ �� ����������

	bool writeError(errorSignal& result, const CString& msg, const int& index);	// ������ ������ 
};