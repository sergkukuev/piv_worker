#pragma once

#include <cstring>
#include <list>
#include <vector>

using namespace std;

#pragma region Data
// �������������� �������� �������
#define RESERVE_SIGNAL L"������"

// �����������
const enum check {numword, title, min, max, csr, bits, comment, size}; // ������� ����� � �������
const enum stats { empty = -2 /* ���������� ������ (���������) */, failed /* ��������� �������� */, opt /* ��-� ������������ */ };		// ������� ������ ����������

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
	bool repWord = false;						// ���� ���������� ������� �� ������ ������
	signalData* part = nullptr;					// ��������� �� ������� (�������) ����� � �����35
	bool error = false;							// ������� ������ � ���������
};

// ������ � ������ ���������� / ������������� �����
struct npData
{
	int value = -1;		// ��������
	int index = -1;		// ������ ���������
};

// ����
struct sheetData 
{
	vector <signalData> signals;	// ����� ���������� �� �����

	CString name;		// �������� ����� � �����
	CString line;		// ����� ��������
	npData np;			// 930� - ����� ������ ����������, ����� - ������������� ����
	int pk = -1;		// 930� - ����� ��������, ����� - �������� ���
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
	signalData* data = nullptr;	// ��������� �� ������ ������� � ��������
	vector <CString> error;			// ����� ������ ���������
	vector <bool> check = { false, false, false, false, false, false, false };	// ������� ������� ������ � ���������
};

struct errorSheet 
{
	sheetData* data = nullptr;		// ��������� �� ����, � ������� ���������� ������ ������
	vector <errorSignal> syntax;	// �������������� ������
	vector <errorSignal> simantic;	// ������������� ������
	vector <errorSignal> warning;	// ��������������
	vector <CString> common;		// ������ ������ ����� (��� ���������� �� �������)
};

struct errorData 
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
	list <errorData> db;		// ���� ������
};