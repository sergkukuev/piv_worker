#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>
#include <set>

#define MAX_BITS 32

// ��������� ��� �������� ���������� ������� ����(�������) � �����
struct repiter {
	int adr;	// ����� ����� (�����)
	bool* bits;	// 0 - ����������� ������ �����(������), 1...MAX_BITS - ����������� �����
};

class PIV_DECLARE CTest {
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
	bool syntaxValue(const signalData& signal, vector <CString>& error);	// �������� ���� �������� ����������
	void stepSyntaxValue(const bool& flag, const int& indx, vector <CString>& error);	// ��� ������� ��� �������� �������� ����������
	bool syntaxTitle(const vector <CString>& title, vector <CString>& error);	// �������� ���������� ��������������
	bool syntaxBits(const intData& bits, vector <CString>& error);				// �������� ������������ ��������

	// ������������� ������
	bool simanticValue(const signalData& signal, vector <CString>& error);	// �������� ���� �������� ����������
	bool findRepiteTitleInSheet(const int& index, vector <CString>& error);	// ����� ���������� �������������� �� �����
	void findRepiteTitleInBook(const int& index, vector <CString>& error);	// ����� ���������� �������������� � �����
	bool simanticBits(const signalData& signal, const CString& prevTitle, vector<repiter>& repit, vector <CString>& error);	// �������� ������������ ��������
	bool checkCrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit); // �������� ���������� �����
	bool checkSameTitle(const CString& next, const CString& prev);		// �������� ���� �������� ������������ �� ����������
};