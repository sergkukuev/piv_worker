#pragma once

#include <cstring>
#include <list>
#include <vector>

using namespace std;

#pragma region Data

// ����� ������ �������������� ������ �� ������
struct convertError {
	bool num = false;	// ��� ������ �����
	bool min = false;	// �������
	bool max = false;	// ��������
	bool csr = false;	// ���
	bool bit = false;	// ��� �����
};

//	������ �������
struct signalData {
	vector <int> numWord;						// ������ ����
	vector <CString> title = { _T(""), _T("")};	// �������� ��������� � �������������
	CString dimension;							// �����������
	double min = DBL_MIN, max = DBL_MIN,		// ���, ����
			csr = DBL_MIN;						// ���
	vector <int> bit;							// ������������ �������
	CString comment;							// ����������
	bool bitSign = false;						// ���� ������� �����

	convertError flags;						// ����� ������ �������������� ������ ������ � �����
};

// ����
struct sheetData {
	vector <signalData> signals;	// ����� ���������� �� �����

	CString name;		// �������� ����� � �����
	CString line;		// ����� ��������
	int np = -1;		// ����� ������ ����������
	int pk = -1;		// ����� ��������
	bool error = false;	// ������� ������ �� �����	
};

// �����
struct bookData {
	vector <sheetData> sheets;	// ����� ������ � �����
	CString name;				// �������� �����
	bool bPK;					// ����������� ������ ��������
};

#pragma endregion

#pragma region Error

struct errorSignal {
	signalData* signal;		// ��������� �� ������ ������� � ��������
	vector <CString> error;	// ����� ������ ���������
};

struct errorSheet {
	sheetData* sheet;				// ��������� �� ����, � ������� ���������� ������ ������
	vector <errorSignal> syntax;	// �������������� ������
	vector <errorSignal> simantic;	// ������������� ������
	vector <errorSignal> warning;	// ��������������
};

struct errorSet {
	list <bookData>::iterator book;	// ��������� �� �����, � ������� ���������� ������ ������
	vector <errorSheet> set;	// ������ ������
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