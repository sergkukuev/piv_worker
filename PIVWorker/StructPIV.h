#pragma once

#include <cstring>
#include <list>
#include <vector>

using namespace std;

// ����� ������
struct adrCell
{
	int row;
	int column;
};

//	������� ���������� �������
struct indexHeader
{
	int iRows;
	int iNumber;
	int iName;
	int iNameSignal;
	int iDimension;
	int iMin;
	int iMax;
	int iCSR;
	int iBits;
	int iComments;
};

///////////////////////////////////////////// �������� ��� /////////////////////////////////////////////

//	������ �������
struct signalData
{
	CString sNumWordField;											// ������ � �������� ����
	int iNumWord[2] = { 0, 0 };										// �������� �������� ������� ���� (�������� 2 �����)
	bool bNumWordField;												// ����������� ������ ������ � ������� �����
	bool b2NumWordField;											// ����������� ���� ������� ����� 
	bool bRepitNumWordField;										// ������ �������� ���������� �����

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

	list <CString> sCommentField;									// ����������
	int iBitSigns;													// �������� ���������� ������� � ����������
	bool bCommentField;												// ����������� ��������� ��������
	bool bBitSigns;													// ������ � �������� ������� ����
};

// �����
struct sheetData
{
	list <signalData> signals;	// ��������� ���������� � ��������

	CString nameSheet;			// �������� ����� � �����
	int iCommentFieldNP;		// ����� ������ ����������
	int iNumPodKadra;			// ����� ��������

	bool bErrorExcelSheet;
	bool bErrorSheet;
};

// �����
struct bookData
{
	vector <sheetData> sheets;	// ��������� �� ����� �����
	CString nameBook;			// �������� �����
	bool bNumPodKadra;			// ����������� ������ ��������
};

struct repitParametr
{
	CString sNameBook;			// ��� �����
	CString sNameSheet;			// ��� �����
	int iNumWord[2] = { 0,0 };	// ����� ����� � �����
	CString sNameParam;			// ����������� ����������
	int iRepit;					// ���������� ����������
};

struct repitWord
{
	bool sNumWordField;						// ������ � �������� ����
	bool sTitleParamField[32] = { false };	// ������������ ���������� (�������� 2 ������)
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
	CString nameSheet;				// �������� �����
};

// �����
struct errorBookData
{
	vector <errorSheetData> sheets;	// ����� ��������� �� �����
	CString nameBook;				// �������� �����
};
