#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>

#define ERROR_DATA L"�� ������� ��������� �������� %s. (���� ������ ��� �������� ������������ �������)"
#define MAX_NUMWORD 32
#define MAX_BITS 32

class CTest {
public:
	CTest();	// �����������
	~CTest();	// ����������

	list <errorSet> Start(list <bookData>& books);	// �������� �� ��� ������

private:
	void getErrors(sheetData* sheet, vector <errorSignal>& syntax, vector <errorSignal>& simantic); // �������� �� ������
	void getWarnings(sheetData* sheet, vector <errorSignal>& warning);								// �������� �� ���������

	void checkNP(signalData& signal, const int& np, vector <errorSignal>& syntax);	// �������� ������, ��������� � ������� ������
	void initRepiter(bool* num, bool** bits);	// ������������� ���������
	
	// �������������� ������
	void syntaxValue(const signalData& signal, vector <CString>& error);			// �������� �������� ����������
	bool syntaxTitle(const vector <CString>& title, vector <CString>& error);		// �������� ���������� ��������������

	void checkValueByFlag(const CString& field, const int& indx, const bool& flag, vector <CString>& error); // �������� �������� ���������� �� ������ ������

	// ������������� ������
	void simanticNumWord(const intData& numWord, bool* repiter, vector <CString>& error);		// �������� ������ �����
	void simanticTitle(sheetData* sheet, const int& indx, const CString& title, const bool& flag, vector <CString>& error);				// �������� ������������ �������
	void simanticValue(const signalData& signal, vector <CString>& error);						// �������� ������������, ������������� � ���
	void simanticBits(const signalData& signal, const CString& prevTitle, bool** repiter, vector <CString>& error);		// �������� ������������ ��������
	
	bool checkCrossBits(const vector <int>& bits, const vector <int>& numWord, bool** repiter); // �������� ���������� �����
	bool checkTitle(const CString& next, const CString& prev);							// �������� ���� ������������ �� ����������
	bool findRepiteInSheet(const CString& field, sheetData* sheet, const int& start);	// ����� ���������� �� �����
};
