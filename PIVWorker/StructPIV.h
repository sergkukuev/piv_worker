#pragma once

#include <cstring>
#include <list>
#include <vector>

using namespace std;

///////////////////////////////////////////// �������� ��� /////////////////////////////////////////////

//	������ �������
struct signalData
{
	CString sNumWordField;											// ������ � �������� ����
	int iNumWord[2] = { 0, 0 };										// �������� �������� ������� ���� (�������� 2 �����)
	bool bNumWordField;												// ����������� ������ ������ � ������� �����
	bool b2NumWordField;											// ����������� ���� ������� ����� 

	CString sTitleParamField[2] = { _T(""), _T("") };				// ������������ ���������� (�������� 2 ������) 
	bool bTitleParamField;											// ����������� ������ � ������������ ����������  

	CString sDimensionField;										// �����������
	CString sMinMaxCsrValField[3] = { _T(""), _T("") , _T("") };	// ��������� �������� ���., ����. � ���� �������� �������
	double dMinMaxCsrVal[3] = { 0, 0, 0 };							// �������� ���., ����. � ���� �������� �������
	bool bMinValField;												// ����������� ������ � ���. 
	bool bMaxValField;												// ����������� ������ � ����.
	bool bCsrValField;												// ����������� ������ � ���.

	CString sBitField;												// ��������� �������� ������������ ���������
	int iBit[4] = { 0, 0, 0, 0 };									// ������������ ��������� (�������� 4 �����)
	bool bBitField;													// ����������� ������ � ������������ ���������
	bool b2BitField;												// ����������� ���� ������������ ��������� 

	CString sCommentField;											// ����������
	int iBitSigns;													// �������� ���������� ������� � ����������
	bool bCommentField;												// ����������� ��������� ��������
	bool bBitSigns;													// ������ � �������� ������� ����
};

// �����
struct sheetData
{
	list <signalData> signals;	// ��������� ���������� � ��������

	CString name;	// �������� ����� � �����
	int iFieldNP;	// ����� ������ ����������
	int iNumPK;		// ����� ��������
	bool bError;	// ����������� ������ �� �����
};

// �����
struct bookData
{
	vector <sheetData> sheets;	// ��������� �� ����� �����
	CString name;	// �������� �����
	bool bNumPK;	// ����������� ������ ��������
};

///////////////////////////////////////////// ������ ��� /////////////////////////////////////////////

// �������
struct errorSignalData
{
	CString sNumWordField;											// ������ � �������� ����
	CString sTitleParamField[2] = { _T(""), _T("") };				// ������������ ���������� (�������� 2 ������)
	CString sDimensionField;										// �����������
	CString sMinMaxCsrValField[3] = { _T(""), _T("") , _T("") };	// ��������� �������� ���., ����. � ���� �������� �������
	CString sBitField;												// ��������� �������� ������������ ���������
	CString sCommentField;											// ���������� 
	list <CString> sErrorField;										// ��� ���������� �� �������
};

// �����
struct errorSheetData
{
	list <errorSignalData> signals;	// ������ �������
	CString name;					// �������� �����
};

// �����
struct errorBookData
{
	vector <errorSheetData> sheets;	// ����� ��������� �� �����
	CString name;					// �������� �����
};

// ��� ������
struct errorSet
{
	vector <errorBookData> syntax;		// ��������������
	vector <errorBookData> simantic;	// �������������
	vector <errorBookData> warning;		// ���������
};

struct errorOneSet
{
	errorBookData syntax;
	errorBookData simantic;
	errorBookData warning;
};

// ����� �������
const CString errRemarks[7] =
{
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