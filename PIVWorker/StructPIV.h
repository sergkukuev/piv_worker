#pragma once

#include <cstring>
#include <list>
#include <vector>

// ����������� ������� ��� �������� DLL

#ifdef PIV_WORKER_EXPORTS
#define PIV_DECLARE __declspec(dllimport)
#else
#define PIV_DECLARE __declspec(dllexport)
#endif

using namespace std;

#pragma region Define

// �����
#define BASE_FOLDER L"\\Artefacts"
#define TEXT_FOLDER L"\\Text"
#define PROJECT_FOLDER L"\\Project"
#define OTHER_FOLDER L"\\Other"
#define SYNTAX_FOLDER L"\\Error\\Syntax"
#define SIMANTIC_FOLDER L"\\Error\\Simantic"
#define WARNING_FOLDER L"\\Warning"

// ����� �������� �������� �����
#define PROJECT_SIZE 8
#define OTHER_SIZE 6

// �����
#define REPORT_NAME L"�����.html"
#define LOG_FILE_NAME L"log.txt"

// �������������� �������� �������
#define RESERVE_SIGNAL L"������"
#define FIELD_SIZE 7	

#define NUMWORD_CHECK 0
#define TITLE_CHECK 1
#define MIN_CHECK 2
#define MAX_CHECK 3
#define CSR_CHECK 4
#define BITS_CHECK 5
#define COMMENT_CHECK 6

#pragma endregion

#pragma region Data

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
const CString errRemarks[FIELD_SIZE] = 
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