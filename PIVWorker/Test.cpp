#include "stdafx.h"
#include "Test.h"

// �����������
CTest::CTest()
{
}

// ����������
CTest::~CTest()
{
}

// �������� �� ��� ������
errorBookData CTest::Start(bookData book) 
{
	errorBookData errBook;
	
	errBook.sheets.resize(book.sheets.size());
	errBook.name = book.nameBook;
	testAll(errBook, book);
	
	return errBook;
}

// �������� �� �������������� ������
void CTest::testAll(errorBookData& errBook, bookData book)
{
	for (size_t cSheet = 0; cSheet < errBook.sheets.size(); cSheet++)
	{
		errBook.sheets[cSheet].name = book.sheets[cSheet].nameSheet;

		if (book.sheets[cSheet].bErrorSheet)
		{
			for (list <signalData>::iterator it = book.sheets[cSheet].signals.begin(); it != book.sheets[cSheet].signals.end(); it++)
			{
				testNumWord(errBook.sheets[cSheet], it);
				testTitleParam(errBook.sheets[cSheet], it);
				testMinMaxCSR(errBook.sheets[cSheet], it);
				testBits(errBook.sheets[cSheet], it);
			}
		}
	}
}

// �������� ������ �����
void CTest::testNumWord(errorSheetData& sheet, list<signalData>::iterator it)
{
	list <CString> error = testField(it->sNumWordField, ErrorBase.getNumWord());

	if (error.empty())
		return;
	
	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
}

// �������� ������������ �������
void CTest::testTitleParam(errorSheetData& sheet, list<signalData>::iterator it)
{
	list <CString> error = testField(it->sTitleParamField[1], ErrorBase.getTitleParam());

	if (error.empty())
		return;

	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
}

// �������� ������������, ������������� � ���
void CTest::testMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator it)
{
	list <CString> errMin = testField(it->sMinMaxCsrValField[0], ErrorBase.getMinMaxCSR());
	list <CString> errMax = testField(it->sMinMaxCsrValField[1], ErrorBase.getMinMaxCSR());
	list <CString> errCSR = testField(it->sMinMaxCsrValField[2], ErrorBase.getMinMaxCSR());

	if (!errMin.empty())
	{
		errorSignalData signal = getErrSignal(it, errMin);
		sheet.signals.push_back(signal);
	}

	if (!errMax.empty())
	{
		errorSignalData signal = getErrSignal(it, errMax);
		sheet.signals.push_back(signal);
	}

	if (!errCSR.empty())
	{
		errorSignalData signal = getErrSignal(it, errCSR);
		sheet.signals.push_back(signal);
	}
}

// �������� ������������ ��������
void CTest::testBits(errorSheetData& sheet, list<signalData>::iterator it)
{
	list <CString> error = testField(it->sBitField, ErrorBase.getBits());

	if (error.empty())
		return;

	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
}

// �������� ���� �� ������
list <CString> CTest::testField(CString field, errorData errStruct)
{
	string temp = convertString(field);
	list <CString> error;	// ����� ��������� ������ � ����
	bool result = false;

	result = regex_match(temp, errStruct.correct);	// �������� �� ���������� ���������

	if (result)	return error;

	for (size_t i = 0; i < errStruct.error.size(); i++)	// ������� ���� ������
		if (regex_match(temp, errStruct.error[i]))
		{
			error.push_back(errStruct.description[i]);
			result = true;
		}

	if (!result) throw UndefinedError();	// ����������, ���� � ���� ������ ����� ������ �� �������������

	return error;
}

// �������� ������ ������ �������
errorSignalData CTest::getErrSignal(list<signalData>::iterator it, list <CString> error)
{
	errorSignalData signal;

	signal.sNumWordField = it->sNumWordField;
	signal.sTitleParamField[0] = it->sTitleParamField[0];
	signal.sTitleParamField[1] = it->sTitleParamField[1];
	signal.sDimensionField = it->sDimensionField;
	signal.sMinMaxCsrValField[0] = it->sMinMaxCsrValField[0];
	signal.sMinMaxCsrValField[1] = it->sMinMaxCsrValField[1];
	signal.sMinMaxCsrValField[2] = it->sMinMaxCsrValField[2];
	signal.sBitField = it->sBitField;
	signal.sCommentField = it->sCommentField;
	signal.sErrorField = error;
	
	return signal;
}

// ������� �������������� CString � string
string CTest::convertString(CString cStr)
{
	int bytes = WideCharToMultiByte(CP_ACP, 0, cStr.GetBuffer(), -1, 0, 0, NULL, NULL);
	char *buffer = new char[bytes];

	WideCharToMultiByte(CP_ACP, 0, cStr.GetBuffer(), -1, buffer, bytes, NULL, NULL);

	string result(buffer);

	delete buffer;
	buffer = nullptr;

	return result;
}