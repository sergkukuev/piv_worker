#pragma once

#include <cstring>
#include <list>
#include <vector>

using namespace std;

// ����������� ������� � ������� ������ � ���������� �����35
namespace dwPart 
{
	// ������� �������� ������ ���� ���������
	const vector <CString> low = { L"(��.�)", L"(��.�.)", L"(��. �)", L"(��. �.)" };
	const vector <CString> hight = { L"(��.�)", L"(��.�.)", L"(��. �)", L"(��. �.)" };
	const vector <CString> hight2 = { L"(c�.�)", L"(c�.�.)", L"(c�. �)", L"(c�. �.)" }; // ������ ������� ����� � ���������� 'c'

	// �������� �� ����������� � ������ (��.�.)
	static bool checkLow(const CString& str)
	{
		for (size_t i = 0; i < low.size(); i++)
			if (str.Find(low[i]) != -1)
				return true;
		return false;
	}
	// �������� �� ����������� � ������ (��.�.)
	static bool checkHight(const CString& str)
	{
		for (size_t i = 0; i < hight.size(); i++)
			if (str.Find(hight[i]) != -1 || str.Find(hight2[i]) != -1)
				return true;
		return false;
	}
	// �������� (��.�.) - ��� ��������� txt ������
	static bool deleleHight(CString& str)
	{
		for (size_t i = 0; i < hight.size(); i++)
		{
			int index = str.Find(hight[i]);
			int index2 = str.Find(hight2[i]);

			if (index != -1)
			{
				str.Delete(index, hight[i].GetLength());
				str.TrimRight();
				return true;
			}
			else if (index2 != -1)
			{
				str.Delete(index2, hight2[i].GetLength());
				str.TrimRight();
				return true;
			}
		}
		return false;
	}
}

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
};

// ������ � ������ ���������� / ������������� �����
struct npData
{
	int value = -1;				// ��������
	signalData* data = nullptr;	// ��������� �� ��������
};

// ����
struct sheetData 
{
	vector <signalData> signals;	// ����� ���������� �� �����

	CString name;		// �������� ����� � �����
	CString line;		// ����� ��������
	npData np;			// 930� - ����� ������ ����������, ����� - ������������� ����
	int pk = -1;		// 930� - ����� ��������, ����� - �������� ���
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