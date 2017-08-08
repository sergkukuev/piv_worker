#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>

#define ERROR_DATA L"�� ������� ��������� �������� %s. (���� ������ ��� �������� ������������ �������)"
#define MAX_NUMWORD 32
#define MAX_BITS 32

struct bitRepiter {
	int adr;	// ����� ����� ��� �����
	bool* bits;	// ������ ����� ��� ����������
};

class PIV_DECLARE CTest {
public:
	CTest();	// �����������
	~CTest();	// ����������

	errorSet Start(bookData& book);					// �������� �� ������ ������ ���������
	list <errorSet> Start(list <bookData>& books);	// �������� �� ��� ������

private:
	void getErrors(sheetData* sheet, vector <errorSignal>& syntax, vector <errorSignal>& simantic); // �������� �� ������
	void getWarnings(sheetData* sheet, vector <errorSignal>& warning);								// �������� �� ���������

	void checkNP(signalData& signal, const int& np, vector <errorSignal>& syntax);	// �������� ������, ��������� � ������� ������
	
	void initRepiter(vector <bitRepiter>& repit, sheetData* sheet);							// ������������� �������� ��� �������� ����������
	bool isContain(const vector<bitRepiter>& repit, const int& numeric);	// ������� �� ��� ����� ����� �����

	// �������������� ������
	bool syntaxValue(const signalData& signal, vector <CString>& error);			// �������� �������� ����������
	bool syntaxBits(const intData& bits, vector <CString>& error);					// �������� ������������ ��������
	bool syntaxTitle(const vector <CString>& title, vector <CString>& error);		// �������� ���������� ��������������

	void checkValueByFlag(const CString& field, const int& indx, const bool& flag, vector <CString>& error); // �������� �������� ���������� �� ������ ������

	// ������������� ������
	bool simanticNumWord(const intData& numWord,/* bool* repiter,*/ vector <CString>& error);		// �������� ������ �����
	bool simanticTitle(sheetData* sheet, const int& indx, const CString& title, const bool& flag, vector <CString>& error);				// �������� ������������ �������
	bool simanticValue(const signalData& signal, vector <CString>& error);						// �������� ������������, ������������� � ���
	bool simanticBits(const signalData& signal, const CString& prevTitle, vector<bitRepiter>& repiter, vector <CString>& error, const bool& arinc);		// �������� ������������ ��������
	
	bool checkCrossBits(const vector <int>& bits, const vector <int>& numWord, vector<bitRepiter>& repiter, const bool& arinc); // �������� ���������� �����
	bool checkTitle(const CString& next, const CString& prev);							// �������� ���� ������������ �� ����������
	bool findRepiteInSheet(const CString& field, sheetData* sheet, const int& start);	// ����� ���������� �� �����
	int findRepiterIndex(const vector <bitRepiter>& bits, const int& numeric);
};
