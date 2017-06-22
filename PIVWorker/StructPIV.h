#pragma once

#include <cstring>
#include <list>
#include <vector>

using namespace std;

#pragma region Data

//	������ �������
struct signalData {
	vector <int> numWord;				// ����� �����
	vector <CString> title = { _T(""), _T("")};	// �������� ��������� � �������������
	CString dimension;		// �����������
	CString min, max, csr;	// ���, ����, ���
	CString bit;			// ������������ �������
	CString comment;		// ����������
};

// ����
struct sheetData {
	vector <signalData> signals;	// ����� ���������� �� �����

	CString name;	// �������� ����� � �����
	CString line;	// ����� ��������
	int np;			// ����� ������ ����������
	int pk;			// ����� ��������
};

// �����
struct bookData {
	vector <sheetData> sheets;	// ����� ������ � �����
	CString name;				// �������� �����
	bool bPK;					// ����������� ������ ��������
};

#pragma endregion

#pragma region Error

// �������
struct errorSignalData {
	CString sNumWordField;											// ������ � �������� ����
	CString sTitleParamField[2] = { _T(""), _T("") };				// ������������ ���������� (�������� 2 ������)
	CString sDimensionField;										// �����������
	CString sMinMaxCsrValField[3] = { _T(""), _T("") , _T("") };	// ��������� �������� ���., ����. � ���� �������� �������
	CString sBitField;												// ��������� �������� ������������ ���������
	CString sCommentField;											// ���������� 
	list <CString> sErrorField;										// ��� ���������� �� �������
};

// �����
struct errorSheetData {
	list <errorSignalData> signals;	// ������ �������
	CString name;					// �������� �����
};

// �����
struct errorBookData {
	vector <errorSheetData> sheets;	// ����� ��������� �� �����
	CString name;					// �������� �����
};

// ��� ������
struct errorSet {
	vector <errorBookData> syntax;		// ��������������
	vector <errorBookData> simantic;	// �������������
	vector <errorBookData> warning;		// ���������
};

struct errorOneSet {
	errorBookData syntax;
	errorBookData simantic;
	errorBookData warning;
};

// ����� �������
const CString errRemarks[7] = {
	// ����� ���������
	_T("���������. ���� \"� �����\" ��������� �� �����."),
	// ����������� ���������
	_T("���������. ���� \"����������� �������\" ��������� �� �����."),
	// �������� ���, ����, ���
	_T("���������. ���� \"����������� ��������\" ��������� �� �����."),
	_T("���������. ���� \"������������ ��������\" ��������� �� �����."),
	_T("���������. ���� \"���� �������� �������\" ��������� �� �����."),
	// �������
	_T("���������. ���� \"������������ �������\" ��������� �� �����."),
	_T("���������. ���� \"����������\" ��������� �� �����.")
};

#pragma endregion