#pragma once

#include "StructPIV.h"
#include "MyException.h"
#include "ErrBase.h"

class CTest
{
public:
	CTest();	// �����������
	~CTest();	// ����������

	errorBookData Start(bookData book);	// �������� �� ��� ������

private:

	CErrBase ErrorBase;	// ���� ������

	void testAll(errorBookData& errBook, bookData book);	// �������� �� ��� ������
	list <CString> testField(CString field, errorData errStruct);	// �������� ���� �� ������
	errorSignalData CTest::getErrSignal(list<signalData>::iterator it, list <CString> error);	// �������� ������ ������ �������
	
	void testNumWord(errorSheetData& sheet, list<signalData>::iterator it);		// �������� ������ �����
	void testTitleParam(errorSheetData& sheet, list<signalData>::iterator it);	// �������� ������������ �������
	void testMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator it);	// �������� ������������, ������������� � ���
	void testBits(errorSheetData& sheet, list<signalData>::iterator it);		// �������� ������������ ��������
	void testComment(errorSheetData& sheet, list<signalData>::iterator it);		// �������� �����������

	string convertString(CString cStr);	// ��������� CString � string 
};

