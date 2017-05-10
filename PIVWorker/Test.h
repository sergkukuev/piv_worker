#pragma once

#include "StructPIV.h"
#include "MyException.h"

class CTest
{
public:
	CTest();	// �����������
	~CTest();	// ����������

	errorBookData Start(bookData book);	// �������� �� ��� ������

private:

	list <errorRegular> errNumWord;		// ����� ��������� ��� ������ �����
	list <errorRegular> errTitle;		// ����� ��������� ��� ������������ �������
	list <errorRegular> errMinMaxCSR;	// ����� ��������� ��� ������������, ������������� � ���
	list <errorRegular> errBits;		// ����� ��������� ��� ������������ ��������
	list <errorRegular> errComent;		// ����� ��������� ��� �����������

	void Syntax(errorBookData& errBook, bookData book);			// �������� �� �������������� ������
	void Simantic(errorBookData& errBook, bookData book);		// �������� �� ������������� ������
	errorRegular getRegular(string regular, CString msg, bool bError = true);	// ��������� ����������� ���������

	// �������������� ������
	void SyntaxNumWord(errorBookData& book, size_t cSheet, list<signalData>::iterator it);		// �������� ������ �����
	void SyntaxTitleParam(errorBookData& book, size_t cSheet, list<signalData>::iterator it);	// �������� ������������ �������
	void SyntaxMinMaxCSR(errorBookData& book, size_t cSheet, list<signalData>::iterator it);	// �������� ������������, ������������� � ���
	void SyntaxBits(errorBookData& book, size_t cSheet, list<signalData>::iterator it);			// �������� ������������ ��������
	void SyntaxComment(errorBookData& book, size_t cSheet, list<signalData>::iterator it);		// �������� �����������

	// ������������� ������
	void SimanticNumWord(errorBookData& book, size_t cSheet, list<signalData>::iterator it);	// �������� ������ �����
	void SimanticTitleParam(errorBookData& book, size_t cSheet, list<signalData>::iterator it);	// �������� ������������ �������
	void SimanticMinMaxCSR(errorBookData& book, size_t cSheet, list<signalData>::iterator it);	// �������� ������������, ������������� � ���
	void SimanticBits(errorBookData& book, size_t cSheet, list<signalData>::iterator it);		// �������� ������������ ��������
	void SimanticComment(errorBookData& book, size_t cSheet, list<signalData>::iterator it);	// �������� �����������
};

