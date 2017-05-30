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
errorOneSet CTest::Start(bookData& book) 
{	
	// �������������� ������
	error.syntax.sheets.resize(book.sheets.size());
	error.syntax.name = book.nameBook;
	Syntax(error.syntax, book);

	// ������������� ������
	/*error.simantic.sheets.resize(book.sheets.size());
	error.simantic.name = book.nameBook;
	error.warning.sheets.resize(book.sheets.size());
	error.warning.name = book.nameBook;
	Simantic(error.simantic, book);
	*/
	return error;
}

#pragma region Syntax

// �������� �� �������������� ������
void CTest::Syntax(errorBookData& errBook, bookData& book)
{
	for (size_t cSheet = 0; cSheet < errBook.sheets.size(); cSheet++)
	{
		errBook.sheets[cSheet].name = book.sheets[cSheet].nameSheet;

		for (list <signalData>::iterator it = book.sheets[cSheet].signals.begin(); it != book.sheets[cSheet].signals.end(); it++)
		{
			try 
			{
				bool begin = true;
				if (it != book.sheets[cSheet].signals.begin())	begin = false;
				bool result = syntaxNumWord(errBook.sheets[cSheet], it);
				(!syntaxTitleParam(errBook.sheets[cSheet], it))? result = false : result = result;
				(!syntaxMinMaxCSR(errBook.sheets[cSheet], it)) ? result = false : result = result;
				(!syntaxBits(errBook.sheets[cSheet], it))? result = false : result = result;
				(!syntaxComment(errBook.sheets[cSheet], it, begin)) ? result = false : result = result;

				if (!result)	// ��������� �����, ��� �� ����� ���� ������
					book.sheets[cSheet].bErrorSheet = false;
			}
			catch (UndefinedError& exc)
			{
				exc.SetName(errBook.sheets[cSheet].name);
				throw exc;
			}
		}
	}
}

// �������� ������ �����
bool CTest::syntaxNumWord(errorSheetData& sheet, list<signalData>::iterator& it)
{
	list <CString> error = testField(it->sNumWordField, ErrorBase.getNumWord());

	if (error.empty())
	{
		translateNumWord(it);	// �������������� ���� � �����
		return true;
	}
	
	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
	it->bNumWordField = false;
	return false;
}

// �������� ������������ �������
bool CTest::syntaxTitleParam(errorSheetData& sheet, list<signalData>::iterator& it)
{
	list <CString> error = testField(it->sTitleParamField[1], ErrorBase.getTitleParam());

	if (error.empty())
		return true;

	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
	it->bTitleParamField = false;
	return false;
}

// �������� ������������, ������������� � ���
bool CTest::syntaxMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it)
{
	list <CString> errMin = testField(it->sMinMaxCsrValField[0], ErrorBase.getMinMaxCSR());
	list <CString> errMax = testField(it->sMinMaxCsrValField[1], ErrorBase.getMinMaxCSR());
	list <CString> errCSR = testField(it->sMinMaxCsrValField[2], ErrorBase.getMinMaxCSR());
	bool result = true;

	if (!errMin.empty())
	{
		errorSignalData signal = getErrSignal(it, errMin);
		sheet.signals.push_back(signal);
		it->bMinValField = false;
		result = false;
	}
	else
	{
		CString buffer = it->sMinMaxCsrValField[0];
		it->iMinMaxCsrVal[0] = _wtoi(buffer);
	}

	if (!errMax.empty())
	{
		errorSignalData signal = getErrSignal(it, errMax);
		sheet.signals.push_back(signal);
		it->bMaxValField = false;
		result = false;
	}
	else
	{
		CString buffer = it->sMinMaxCsrValField[1];
		it->iMinMaxCsrVal[1] = _wtoi(buffer);
	}

	if (!errCSR.empty())
	{
		errorSignalData signal = getErrSignal(it, errCSR);
		sheet.signals.push_back(signal);
		it->bCsrValField = false;
		result = false;
	}
	else
	{
		CString buffer = it->sMinMaxCsrValField[2];
		it->iMinMaxCsrVal[2] = _wtoi(buffer);
	}

	return result;
}

// �������� ������������ ��������
bool CTest::syntaxBits(errorSheetData& sheet, list<signalData>::iterator& it)
{
	list <CString> error = testField(it->sBitField, ErrorBase.getBits());

	if (error.empty())
	{
		translateBits(it);
		return true;
	}

	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
	it->bBitField = false;

	return false;
}

// �������� ������������
bool CTest::syntaxComment(errorSheetData& sheet, list<signalData>::iterator& it, bool begin)
{
	list <CString> error;
	string field = convertString(it->sCommentField);
	bool bNP = false;
	bool zn = regex_search(field, ErrorBase.getComment().error[1]);	// �������� �� ����

	if (begin)
	{
		bNP = regex_search(field, ErrorBase.getComment().error[0]);	// �������� �� ����� ����������

		if (bNP)
			error.push_back(ErrorBase.getComment().description[0]);
	}
	else
		if (zn)
			error.push_back(ErrorBase.getComment().description[1]);

	if (error.empty())
	{
		translateBitSign(it);	// ������� �������� ��������� ����
		return true;
	}
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
		if (regex_search(temp, errStruct.error[i]))
		{
			error.push_back(errStruct.description[i]);
			result = true;
		}

	if (!result)	// ����������, ���� � ���� ������ ����� ������ �� �������������
	{
		UndefinedError exc;
		exc.SetParam(field);
		throw exc;
	}

	return error;
}

#pragma endregion 

#pragma once region Simantic

// �������� �� ������������� ������
void CTest::Simantic(errorBookData& errBook, bookData& book)
{
	for (size_t cSheet = 0; cSheet < errBook.sheets.size(); cSheet++)
	{
		errBook.sheets[cSheet].name = book.sheets[cSheet].nameSheet;

		for (list <signalData>::iterator it = book.sheets[cSheet].signals.begin(); it != book.sheets[cSheet].signals.end(); it++)
		{
			bool wRepite[32];
			bool result = simanticNumWord(errBook.sheets[cSheet], it, wRepite);
			(!simanticTitleParam(errBook.sheets[cSheet], it)) ? result = false : result = result;
			(!simanticMinMaxCSR(errBook.sheets[cSheet], it)) ? result = false : result = result;
			(!simanticBits(errBook.sheets[cSheet], it)) ? result = false : result = result;
			
			if (!result)	// ��������� �����, ��� �� ����� ���� ������
				book.sheets[cSheet].bErrorSheet = false;
		}
	}
}

// �������� ������ �����
bool CTest::simanticNumWord(errorSheetData& sheet, list<signalData>::iterator& it, bool wRep[])
{
	list <CString> error;

	if (it->bNumWordField)
	{
		if (it->iNumWord[0] > 32 || it->iNumWord[1] > 32)	// ����� ������ ���� �� ������ 32
			error.push_back(ErrorBase.sim.NumMore32);

		if (it->iNumWord[1] > 0)
			if (!wRep[it->iNumWord[1] - 1])	// ����� ���������� � ���� ��� ������� �����
				error.push_back(ErrorBase.sim.NumRepite);
		
		if (!wRep[it->iNumWord[0] - 1])	// ��� �������
			error.push_back(ErrorBase.sim.NumRepite);

		// ������� � ���, ��� ��� ����� �� ���� �����
		wRep[it->iNumWord[0] - 1] = false;
		if (it->iNumWord[1] > 0)
			wRep[it->iNumWord[1] - 1] = false;
	}

	if (error.empty())
		return true;

	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
	it->bNumWordField = false;

	return false;
}

bool CTest::simanticTitleParam(errorSheetData& errSheet, list<signalData>::iterator& it)	// ��������� ����� � ������ �����
{
	list <CString> error;

	if (it->bTitleParamField)
	{
		if (findRepiteInSheet())
			error.push_back(ErrorBase.sim.TitleRepSheet);
		else if (findRepiteInBook())
			error.push_back(ErrorBase.sim.TitleRepBook);	// ���������, � �� ������ 
	}

	if (error.empty())
		return true;

	errorSignalData signal = getErrSignal(it, error);
	errSheet.signals.push_back(signal);
	it->bTitleParamField = false;

	return false;
}

// �������� ������������, ������������� � ���
bool CTest::simanticMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it)
{

}

// �������� ������������ ��������
bool CTest::simanticBits(errorSheetData& sheet, list<signalData>::iterator& it)
{

}

// ������� ������� ���� �� ������ � �����
void CTest::translateNumWord(list<signalData>::iterator& it)
{
	CString num = it->sNumWordField;
	int indxDot = num.Find(_T(','));

	if (indxDot == -1)
	{
		num.Trim();	// �������� ��������
		it->iNumWord[0] = _wtoi(num);
	}
	else
	{
		CString num2 = num;	// ������ �����
		num2.Delete(indxDot, num.GetLength() - indxDot);
		num2.Trim();
		it->iNumWord[1] = _wtoi(num2);

		num.Delete(0, indxDot);	// ������ �����
		num.Trim();
		it->iNumWord[0] = _wtoi(num);

		it->b2NumWordField = false; // ��������� ����� ����������� ���� ����
	}
}

// ������� �� ������������ �������� �� ������ � �����
void CTest::translateBits(list<signalData>::iterator& it)
{
	CString bits = it->sBitField;
	int indxDot = bits.Find(_T(','));

	if (indxDot == -1)	// ��� ������ ����������
	{
		vector <int> tmp = stepTranslateBits(bits);

		it->iBit[0] = tmp[0];
		it->iBit[1] = tmp[1];
	}
	else  // ��� ���� �����������
	{
		CString bits2 = bits;
		bits2.Delete(indxDot, bits.GetLength() - indxDot);
		bits.Delete(0, indxDot);

		vector <int> tmp = stepTranslateBits(bits);
		it->iBit[0] = tmp[0];
		it->iBit[1] = tmp[1];
		
		tmp.clear();
		tmp = stepTranslateBits(bits2);
		it->iBit[2] = tmp[0];
		it->iBit[3] = tmp[1];

		it->b2BitField = false; // ��������� ����� ��� ���� �����������
	}
}

// �������������� ������� ��� �������� ��������
vector <int> CTest::stepTranslateBits(CString bits)
{
	vector <int> result = { 0, 0 };
	bits.Trim();
	
	// ����� ������� ������������
	int indxDel = bits.Find(_T('.'));
	if (indxDel == -1)
		indxDel = bits.Find(_T('�'));
	else
		bits.Delete(indxDel, 2);	// ������ ��� ����������� �����
	
	if (indxDel == -1)	// ������������ ���, ������������ ���� ������
		result[0] = _wtoi(bits);
	else
	{
		CString num = bits;
		num.Delete(0, indxDel);	// ������ �����
		num.Trim();
		result[0] = _wtoi(num);

		CString num2 = num;	// ������ �����
		num2.Delete(indxDel, num.GetLength() - indxDel);
		num2.Trim();
		result[1] = _wtoi(num2);
	}

	return result;
}

// ������� ��������� ����, ���� �� ����
void CTest::translateBitSign(list<signalData>::iterator& it)
{
	int posZn = it->sCommentField.Find(_T("��-"));

	if (posZn != -1)
	{
		CString tmp = it->sCommentField;
		tmp.Delete(0, posZn + 3);
		int iSpace = tmp.Find(_T(' '));
		if (iSpace == -1)
			iSpace = tmp.Find(_T('0x1c'));

		tmp.Delete(0, iSpace);

		it->iBitSigns = _wtoi(tmp);

		it->bBitSigns = false;	// ��������� ����� ����������� ����� � ����������
	}
}

#pragma endregion

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