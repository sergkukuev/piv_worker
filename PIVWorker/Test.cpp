#include "stdafx.h"
#include "Test.h"

// Конструктор
CTest::CTest()
{
	errorRegular temp;

	temp = getRegular("[[:d:]]+", _T("Допустимо"), false);
	errNumWord.push_back(temp);
	temp = getRegular("[[:d:]]+(,)[[:d:]]+", _T("Допустимо"), false);
	errNumWord.push_back(temp);

	temp = getRegular("", _T(""), false);
	errTitle.push_back(temp);
}

// Деструктор
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

// Проверка на все ошибки
errorBookData CTest::Start(bookData book) 
{
	errorBookData errBook;
	
	errBook.sheets.resize(book.sheets.size());
	errBook.name = book.nameBook;
	Syntax(errBook, book);
	Simantic(errBook, book);
	
	return errBook;
}

// Проверка на синтаксические ошибки
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

// Проверка на семантические ошибки
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

///////////////////////////////////////////// Синтаксические /////////////////////////////////////////////

// Проверка номера слова
void CTest::SyntaxNumWord(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// Проверка наименований сигнала
void CTest::SyntaxTitleParam(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// Проверка минимального, максимального и цср
void CTest::SyntaxMinMaxCSR(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// Проверка используемых разрядов
void CTest::SyntaxBits(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// Проверка коментариев
void CTest::SyntaxComment(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

///////////////////////////////////////////// Семантические /////////////////////////////////////////////

// Проверка номера слова
void CTest::SimanticNumWord(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// Проверка наименований сигнала
void CTest::SimanticTitleParam(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// Проверка минимального, максимального и цср
void CTest::SimanticMinMaxCSR(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// Проверка используемых разрядов
void CTest::SimanticBits(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}

// Проверка коментариев
void CTest::SimanticComment(errorBookData& book, size_t cSheet, list<signalData>::iterator it)
{

}