#pragma once

#include <cstring>
#include <list>
#include <vector>

#include "Defines.h"

using namespace std;

#pragma region Data

const enum check {numword, title, min, max, csr, bits, comment, size}; // ������� ����� � �������

// �������� ������ ����� � �����
struct intData 
{
	vector <int> value;	// ��������
	int sys = 10;		// ������� ���������
	CString field;		// ������ � �������� ����
	bool flag = false;	// ���� ������� ������ ��� �����������
};

// �������� ���, ���� � ���
struct doubleData 
{
	double value = DBL_MIN;	// ��������
	CString field;			// ������ � �������� ����
	bool flag = false;		// ���� ������� ������ ��� �����������
};

//	������ �������
struct signalData 
{
	intData numWord;							// ������ ����
	vector <CString> title = { L"", L""};		// �������� ��������� � �������������
	CString dimension;							// �����������
	doubleData min, max, csr;					// ���, ����, ���
	intData bit;								// ������������ �������
	CString comment;							// ����������
	bool bitSign = false;						// ���� ������� �����
};

// ����
struct sheetData 
{
	vector <signalData> signals;	// ����� ���������� �� �����

	CString name;		// �������� ����� � �����
	CString line;		// ����� ��������
	int np = -1;		// ����� ������ ����������
	int pk = -1;		// ����� ��������
	bool error = false;	// ������� ������ �� �����
	bool arinc = false;	// ��� ����� ��������: Arinc(true), ����(false)
};

// �����
struct bookData 
{
	vector <sheetData> sheets;	// ����� ������ � �����
	CString name;				// �������� �����
	bool bPK;					// ����������� ������ ��������
};

#pragma endregion

#pragma region Error

struct errorSignal 
{
	signalData* signal = nullptr;	// ��������� �� ������ ������� � ��������
	vector <CString> error;			// ����� ������ ���������
	vector <bool> check = { false, false, false, false, false, false, false };	// ������� ������� ������ � ���������
};

struct errorSheet 
{
	sheetData* sheet = nullptr;		// ��������� �� ����, � ������� ���������� ������ ������
	vector <errorSignal> syntax;	// �������������� ������
	vector <errorSignal> simantic;	// ������������� ������
	vector <errorSignal> warning;	// ��������������
};

struct errorSet 
{
	bookData* book;		// ��������� �� �����, � ������� ���������� ������ ������
	vector <errorSheet> set;	// ������ ������
};

// ����� �������
const CString errRemarks[check::size] = 
{
	// ����� ���������
	L"���������. ���� \"� �����\" ��������� �� �����.",
	// ����������� ���������
	L"���������. ���� \"����������� �������\" ��������� �� �����.",
	// �������� ���, ����, ���
	L"���������. ���� \"����������� ��������\" ��������� �� �����.",
	L"���������. ���� \"������������ ��������\" ��������� �� �����.",
	L"���������. ���� \"���� �������� �������\" ��������� �� �����.",
	// �������
	L"���������. ���� \"������������ �������\" ��������� �� �����.",
	L"���������. ���� \"����������\" ��������� �� �����."
};

#pragma endregion

// �������� ������ ���
struct pivData 
{
	list <bookData> books;	// ������ � ����������� ���
	list <errorSet> db;		// ���� ������
};