#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>

#define ERROR_DATA _T("�� ������� ��������� �������� %s. (���� �������� ������������ ������� ��� ������ '.' � ',')")
#define RESERVE_SIGNAL _T("������")
#define MAX_NUMWORD 32
#define MAX_BITS 32

class CTest {
public:
	CTest();	// �����������
	~CTest();	// ����������

	list <errorSet> Start(list <bookData>& books);	// �������� �� ��� ������

private:
	void getErrors(sheetData* sheet, vector <errorSignal>& syntax, vector <errorSignal>& simantic); // �������� �� ������
	void getWarnings(sheetData* sheet, vector <errorSignal>& warning);		// �������� �� ���������

	void checkNP(signalData& signal, const int& np, vector <errorSignal>& syntax);	// �������� ������, ��������� � ������� ������
	void initRepiter(bool* num, bool** bits);	// ������������� ���������
	
	// �������������� ������
	void syntaxValue(const convertError& flags, vector <CString>& error);	// �������� �������� ����������
	bool syntaxTitle(const vector <CString>& title, vector <CString>& error);		// �������� ���������� ��������������

	void checkValueByFlag(const CString& field, const int& indx, const bool& flag, vector <CString>& error); // �������� �������� ���������� �� ������ ������

	// ������������� ������
	void simanticNumWord(const vector <int>& numeric, const bool& flag, bool* repiter, vector <CString>& error);		// �������� ������ �����
	void simanticTitle(sheetData* sheet, const CString& title, const bool& flag, vector <CString>& error);				// �������� ������������ �������
	void simanticValue(const signalData& signal, vector <CString>& error);						// �������� ������������, ������������� � ���
	void simanticBits(const signalData& signal, bool** repiter, vector <CString>& error);		// �������� ������������ ��������
	
	bool checkCrossBits(const vector <int>& bits, const vector <int>& numWord, bool** repiter); // �������� ���������� �����
	bool findRepiteInSheet(const CString& field, sheetData* sheet);
};
