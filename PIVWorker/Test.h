#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>
#include <set>

#define MAX_BITS 32

// ��������� ��� �������� ���������� ������� ����(�������) � �����
struct repiter 
{
	int adr;	// ����� ����� (�����)
	bool* bits;	// 0 - ����������� ������ �����(������), 1...MAX_BITS - ����������� �����
};

// ��������� ��� ������� ������, ������� ���������� ��������
struct errorFilter {

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


	set<CString> exception;	// ��������� ���������� �������� � ������������

	void getErrors(vector <errorSignal>& syntax, vector <errorSignal>& simantic);	// �������� �� �������������� � ������������� ������
	void getWarnings(vector <errorSignal>& warning);	// �������� �� �������������� ������ (���������) 

	void initRepiter(vector<repiter>& repit);	// ������������� �������� ��� �������� ����������
	int isContain(const vector<repiter>& repit, const int& numeric);	// ������� �� ��� ����� ����� ����� (�����) (� ������ ������� �����. ������, ����� -1)

	// �������������� ������
	bool checkNP(vector <errorSignal>& error);	// �������� ������ ������ ����������
	bool syntaxValue(errorSignal& set);	// �������� ���� �������� ����������
	bool syntaxTitle(errorSignal& set);	// �������� ���������� ��������������
	bool syntaxBits(errorSignal& set);	// �������� ������������ ��������

	// ������������� ������
	bool simanticValue(errorSignal& set, vector <repiter>& repit);		// �������� ���� �������� ����������
	bool findRepiteTitleInSheet(errorSignal& set, const int& index);	// ����� ���������� �������������� �� �����
	void findRepiteTitleInBook(errorSignal& set, const int& index);		// ����� ���������� �������������� � �����
	bool simanticBits(errorSignal& set, const CString& prevTitle, vector<repiter>& repit);	// �������� ������������ ��������
	vector<int> checkCrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit); // �������� ���������� �����
	bool checkSameTitle(const CString& next, const CString& prev);		// �������� ���� �������� ������������ �� ����������

	bool writeError(errorSignal& result, const CString& msg, const int& index);	// ������ ������ 
};