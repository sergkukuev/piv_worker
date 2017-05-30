#pragma once

#include "StructPIV.h"
#include "MyException.h"
#include "ErrBase.h"

class CTest
{
public:
	CTest();	// �����������
	~CTest();	// ����������

	errorOneSet Start(bookData& book);	// �������� �� ��� ������

private:

	CErrBase ErrorBase;	// ���� ������
	errorOneSet error;

	void Syntax(errorBookData& errBook, bookData& book);	// �������� �� �������������� ������
	void Simantic(errorBookData& errBook, bookData& book);	// �������� �� ������������� ������
	list <CString> testField(CString field, errorData errStruct);	// �������� ���� �� ������
	errorSignalData getErrSignal(list<signalData>::iterator it, list <CString> error);	// �������� ������ ������ �������
	
	// ��������������
	bool syntaxNumWord(errorSheetData& sheet, list<signalData>::iterator& it);		// �������� ������ �����
	bool syntaxTitleParam(errorSheetData& sheet, list<signalData>::iterator& it);	// �������� ������������ �������
	bool syntaxMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it);	// �������� ������������, ������������� � ���
	bool syntaxBits(errorSheetData& sheet, list<signalData>::iterator& it);			// �������� ������������ ��������
	bool syntaxComment(errorSheetData& sheet, list<signalData>::iterator& it, bool begin);		// �������� �����������

	// �������������
	bool simanticNumWord(errorSheetData& sheet, list<signalData>::iterator& it, bool wRep[]);		// �������� ������ �����
	bool simanticTitleParam(errorSheetData& sheet, list<signalData>::iterator& it);		// �������� ������������ �������
	bool simanticMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it);		// �������� ������������, ������������� � ���
	bool simanticBits(errorSheetData& sheet, list<signalData>::iterator& it);			// �������� ������������ ��������

	bool findRepiteInBook();
	bool findRepiteInSheet();
	void translateNumWord(list<signalData>::iterator& it);	// ������� � ����� �� ������ � �����
	void translateBits(list<signalData>::iterator& it);		// ������� ������������ �������� �� ������ � �����
	vector <int> stepTranslateBits(CString bits);			// �������������� ������� ��� �������� ��������
	void translateBitSign(list<signalData>::iterator& it);	// ������� �������� ����� �� ����������

	string convertString(CString cStr);	// ��������� CString � string 
};

