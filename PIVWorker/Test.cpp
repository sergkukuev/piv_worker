#include "stdafx.h"
#include "Test.h"

// �����������
CTest::CTest()
{
	errorRegular temp;

	temp = getRegular("[[:d:]]+", _T("���������"), false);
	errNumWord.push_back(temp);
	temp = getRegular("[[:d:]]+(,)[[:d:]]+", _T("���������"), false);
	errNumWord.push_back(temp);

	temp = getRegular("", _T(""), false);
	errTitle.push_back(temp);
}

// ����������
CTest::~CTest()
{
	errNumWord.clear();
	errTitle.clear();
	errMinMaxCSR.clear();
	errBits.clear();
	errComent.clear();
}

errorRegular CTest::getRegular(string regular, CString msg, bool bError)
{
	errorRegular error;
	regex reg(regular);

	error.regular = reg;
	error.description = msg;
	error.bError = bError;

	return error;
}

// �������� �� ��� ������
errorBookData CTest::Start(bookData book) 
{
	errorBookData errBook;
	
	errBook.sheets.resize(book.sheets.size());
	errBook.name = book.nameBook;
	Syntax(errBook, book);
	Simantic(errBook, book);
	
	return errBook;
}

// �������� �� �������������� ������
void CTest::Syntax(errorBookData& errBook, bookData book)
{
	for (size_t cSheet = 0; cSheet < errBook.sheets.size(); cSheet++)
	{
		errBook.sheets[cSheet].name = book.sheets[cSheet].nameSheet;

		if (book.sheets[cSheet].bErrorSheet)
		{
			for (list <signalData>::iterator it = book.sheets[cSheet].signals.begin(); it != book.sheets[cSheet].signals.end(); it++)
			{
				SyntaxNumWord(errBook, cSheet, it);
				SyntaxTitleParam(errBook, cSheet, it);
				SyntaxMinMaxCSR(errBook, cSheet, it);
				SyntaxBits(errBook, cSheet, it);
				SyntaxComment(errBook, cSheet, it);
			}
		}
	}
}

// �������� �� ������������� ������
void CTest::Simantic(errorBookData& errBook, bookData book)
{
	for (size_t cSheet = 0; cSheet < errBook.sheets.size(); cSheet++)
	{
		errBook.sheets[cSheet].name = book.sheets[cSheet].nameSheet;

		if (book.sheets[cSheet].bErrorSheet)
		{
			for (list <signalData>::iterator it = book.sheets[cSheet].signals.begin(); it != book.sheets[cSheet].signals.end(); it++)
			{
				SimanticNumWord(errBook, cSheet, it);
				SimanticTitleParam(errBook, cSheet, it);
				SimanticMinMaxCSR(errBook, cSheet, it);
				SimanticBits(errBook, cSheet, it);
				SimanticComment(errBook, cSheet, it);
			}
		}
	}
}

///////////////////////////////////////////// �������������� /////////////////////////////////////////////

// �������� ������ �����
void CTest::SyntaxNumWord(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// �������� ������������ �������
void CTest::SyntaxTitleParam(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// �������� ������������, ������������� � ���
void CTest::SyntaxMinMaxCSR(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// �������� ������������ ��������
void CTest::SyntaxBits(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// �������� �����������
void CTest::SyntaxComment(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

///////////////////////////////////////////// ������������� /////////////////////////////////////////////

// �������� ������ �����
void CTest::SimanticNumWord(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// �������� ������������ �������
void CTest::SimanticTitleParam(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// �������� ������������, ������������� � ���
void CTest::SimanticMinMaxCSR(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// �������� ������������ ��������
void CTest::SimanticBits(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// �������� �����������
void CTest::SimanticComment(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}