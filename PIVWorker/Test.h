#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>

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
	bookData* tmpBook = nullptr;	// ��������� �� ������� �����
	sheetData* tmpSheet = nullptr;	// ��������� �� ������� ����

	void getErrors(sheetData* sheet, vector <errorSignal>& syntax, vector <errorSignal>& simantic); // �������� �� ������
	void getWarnings(sheetData* sheet, vector <errorSignal>& warning);								// �������� �� ���������

	void initRepiter(vector<repiter>& repit);	// ������������� �������� ��� �������� ����������
	int isContain(const vector<repiter>& repit, const int& numeric);	// ������� �� ��� ����� ����� ����� (�����) (� ������ ����� �����. ������, ����� -1)

	// �������������� ������
	void checkNP(signalData& signal, const int& np, vector <errorSignal>& syntax);	// �������� ������ ������
	bool syntaxValue(const signalData& signal, const bool& arinc, vector <CString>& error);	// �������� �������� ����������
	bool syntaxTitle(const vector <CString>& title, vector <CString>& error);		// �������� ���������� ��������������
	bool syntaxBits(const intData& bits, vector <CString>& error);					// �������� ������������ ��������

	// ������������� ������
	bool simanticValue(const signalData& signal, vector <CString>& error);			// �������� ������������, ������������� � ���
	bool simanticNumWord(const intData& numeric, const bool& arinc, vector <CString>& error);		// �������� ������ �����
	bool simanticTitle(const int& indx, const CString& title, const bool& flag, vector <CString>& error);	// �������� ������������ �������
	bool simanticBits(const signalData& signal, const CString& prevTitle, vector<repiter>& repit, vector <CString>& error);	// �������� ������������ ��������

	// ��������������� �������
	void checkValueByFlag(const int& indx, const bool& flag, vector <CString>& error);					// ��� ������� ��� �������� �������� ����������
	bool checkCrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit); // �������� ���������� �����
	bool checkSameTitle(const CString& next, const CString& prev);		// �������� ���� �������� ������������ �� ����������
	bool findRepiteInSheet(const CString& title, const int& indx);		// ����� ���������� �� �����
	bool findRepiteInBook(const CString& title, const int& indx);		// ����� ���������� � �����
};
