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
	int NP = 0;

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
	bool simanticTitleParam(errorSheetData& sheet, list<signalData>::iterator& it);					// �������� ������������ �������
	bool simanticMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it, int currNP, bool begin);	// �������� ������������, ������������� � ���
	bool simanticBits(errorSheetData& sheet, list<signalData>::iterator& it, bool tRep[32][32]);			// �������� ������������ ��������

	bool findRepiteInBook();
	bool findRepiteInSheet();
	bool checkCrossBits(list<signalData>::iterator& it, bool repiter[32][32]);	// �������� �� ���������� �����
	void translateNumWord(list<signalData>::iterator& it);	// ������� � ����� �� ������ � �����
	void translateBits(list<signalData>::iterator& it);		// ������� ������������ �������� �� ������ � �����
	vector <int> stepTranslateBits(CString bits);			// �������������� ������� ��� �������� ��������
	void translateComment(list<signalData>::iterator& it);	// ������� �������� ����� ��� NP ������ �� ����������

	string convertString(CString cStr);	// ��������� CString � string 
};

