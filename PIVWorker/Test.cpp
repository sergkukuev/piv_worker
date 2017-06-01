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
	error.simantic.sheets.resize(book.sheets.size());
	error.simantic.name = book.nameBook;
	error.warning.sheets.resize(book.sheets.size());
	error.warning.name = book.nameBook;
	Simantic(error.simantic, book);
	
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
				bool begin = true, result = true;
				if (it != book.sheets[cSheet].signals.begin())	begin = false;
				if (it->sTitleParamField->Compare(_T("������")) != 0)
				{
					result = syntaxNumWord(errBook.sheets[cSheet], it);
					(!syntaxTitleParam(errBook.sheets[cSheet], it)) ? result = false : result = result;
					(!syntaxMinMaxCSR(errBook.sheets[cSheet], it)) ? result = false : result = result;
					(!syntaxBits(errBook.sheets[cSheet], it)) ? result = false : result = result;
					(!syntaxComment(errBook.sheets[cSheet], it, begin)) ? result = false : result = result;
				}
				
				if (NP != 0)
				{
					book.sheets[cSheet].iCommentFieldNP = NP;	// ��������� ������ ������ (translateComment)
					NP = 0;
				}

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
	
	error.push_front(errRemarks[0]);
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

	error.push_front(errRemarks[1]);
	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
	it->bTitleParamField = false;
	return false;
}

// �������� ������������, ������������� � ���
bool CTest::syntaxMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it)
{
	list <CString> errMin, errMax, errCSR;
	bool result = true;

	if (!it->sMinMaxCsrValField[0].IsEmpty() || !it->sMinMaxCsrValField[0].IsEmpty() || !it->sMinMaxCsrValField[0].IsEmpty())
	{
		errMin = testField(it->sMinMaxCsrValField[0], ErrorBase.getMinMaxCSR());
		errMax = testField(it->sMinMaxCsrValField[1], ErrorBase.getMinMaxCSR());
		errCSR = testField(it->sMinMaxCsrValField[2], ErrorBase.getMinMaxCSR());
	}

	if (!errMin.empty())
	{
		errMin.push_front(errRemarks[2]);
		errorSignalData signal = getErrSignal(it, errMin);
		sheet.signals.push_back(signal);
		it->bMinValField = false;
		result = false;
	}

	if (!errMax.empty())
	{
		errMax.push_front(errRemarks[3]);
		errorSignalData signal = getErrSignal(it, errMax);
		sheet.signals.push_back(signal);
		it->bMaxValField = false;
		result = false;
	}

	if (!errCSR.empty())
	{
		errCSR.push_front(errRemarks[4]);
		errorSignalData signal = getErrSignal(it, errCSR);
		sheet.signals.push_back(signal);
		it->bCsrValField = false;
		result = false;
	}

	if (result)
		translateMMC(it);

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

	error.push_front(errRemarks[5]);
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

	if (begin)
	{
		bool bNP = regex_search(field, ErrorBase.getComment().error[0]);	// �������� �� ����� ����������
		if (bNP)
			error.push_back(ErrorBase.getComment().description[0]);
	}
	else
	{
		bool zn = regex_search(field, ErrorBase.getComment().error[1]);	// �������� �� ����
		if (zn)
		{
			error.push_back(ErrorBase.getComment().description[1]);
			it->bBitSigns = false;
		}		
	}

	if (error.empty())
	{
		translateComment(it);	// ������� �������� ��������� ���� ��� NP ������, ���� ��� ����
		return true;
	}

	error.push_front(errRemarks[6]);
	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
	return false;
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

#pragma region Simantic

// �������� �� ������������� ������
void CTest::Simantic(errorBookData& errBook, bookData& book)
{
	for (size_t cSheet = 0; cSheet < errBook.sheets.size(); cSheet++)
	{
		errBook.sheets[cSheet].name = book.sheets[cSheet].nameSheet;
		bool wRepite[32];		// ��� ������������ ���������� ����
		bool tRepiter[32][32];	// ��� ������������ ���������� ���������������

		for (list <signalData>::iterator it = book.sheets[cSheet].signals.begin(); it != book.sheets[cSheet].signals.end(); it++)
		{
			bool begin = true, result = true;
			if (it != book.sheets[cSheet].signals.begin())	begin = false;
			if (it->sTitleParamField->Compare(_T("������")) != 0)
			{
				result = simanticNumWord(errBook.sheets[cSheet], it, wRepite);
				//(!simanticTitleParam(errBook.sheets[cSheet], it)) ? result = false : result = result;
				(!simanticMinMaxCSR(errBook.sheets[cSheet], it, book.sheets[cSheet].iCommentFieldNP, begin)) ? result = false : result = result;
				(!simanticBits(errBook.sheets[cSheet], it, tRepiter)) ? result = false : result = result;
			}
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
		else if (!wRep[it->iNumWord[0] - 1])	// ��� �������
			error.push_back(ErrorBase.sim.NumRepite);

		// ������� � ���, ��� ��� ����� �� ���� �����
		wRep[it->iNumWord[0] - 1] = false;
		if (it->iNumWord[1] > 0)
			wRep[it->iNumWord[1] - 1] = false;
	}

	if (error.empty())
		return true;

	error.push_front(errRemarks[0]);
	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);

	return false;
}

bool CTest::simanticTitleParam(errorSheetData& errSheet, list<signalData>::iterator& it)	// ��������� ����� � ������ �����
{
	/*list <CString> error;

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
	it->bTitleParamField = false;*/

	return false;
}

// �������� ������������, ������������� � ���
bool CTest::simanticMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it, int currNP, bool begin)
{
	list <CString> errMin, errMax;

	if (it->bMinValField && it->bMaxValField && it->bCsrValField && it->bBitField && it->bBitSigns)
	{
		if (begin)
		{
			if (it->iMinMaxCsrVal[0] != currNP)
				errMin.push_back(ErrorBase.sim.MMCNotNote);
			if (it->iMinMaxCsrVal[1] != currNP)
				errMax.push_back(ErrorBase.sim.MMCNotNote);
		}

		// ������� �����
		int nBit = 0;
		if (it->b2BitField)
			nBit = (it->iBit[1] - it->iBit[0]) + (it->iBit[3] - it->iBit[2]) + 2;
		else
			nBit = it->iBit[1] - it->iBit[0] + 1;

		if (it->bCommentField)	
			nBit--;

		// ������� �������� � ���������
		int nMin = it->iMinMaxCsrVal[2];	// ���
		for (int i = 1; i <= nBit; i++)
			nMin = nMin / 2;

		int nMax = (it->iMinMaxCsrVal[2] * 2) - nMin;	// ����

		if (it->iMinMaxCsrVal[1] - nMax > 2)
			errMax.push_back(ErrorBase.sim.MMCNotPkg);
		else if (( (abs(it->iMinMaxCsrVal[0]) > (nMax + nMin)) || (abs(it->iMinMaxCsrVal[0]) < nMin)) && it->iMinMaxCsrVal[0] != 0)
			errMin.push_back(ErrorBase.sim.MMCNotPkg);

		if ((it->iMinMaxCsrVal[0] < 0) && !it->bCommentField)
			errMin.push_back(ErrorBase.sim.MMCNotNegative);
		else if ((it->iMinMaxCsrVal[0] >= 0) && it->bCommentField)
			errMin.push_back(ErrorBase.sim.MMCNegative);
	}

	if (errMin.empty() && errMax.empty())
		return true;
	else if (!errMin.empty())
	{
		errMin.push_front(errRemarks[2]);
		errorSignalData signal = getErrSignal(it, errMin);
		sheet.signals.push_back(signal);
	}
	else
	{
		errMax.push_front(errRemarks[3]);
		errorSignalData signal = getErrSignal(it, errMax);
		sheet.signals.push_back(signal);
	}

	return false;
}

// �������� ������������ ��������
bool CTest::simanticBits(errorSheetData& sheet, list<signalData>::iterator& it, bool tRep[][32])
{
	list <CString> error;
	
	if (it->bNumWordField && it->bBitField)
	{
		if (it->b2NumWordField == it->b2BitField)	// ���-�� � ���� ������ ��������� � ���-���� ���������� ���. ��������
		{
			if (!checkCrossBits(it, tRep))	// �������� �� ����������
				error.push_back(ErrorBase.sim.BitsCross);	

			if (it->bCommentField && it->bBitSigns)
				if (it->iBitSigns != it->iBit[0])
					error.push_back(ErrorBase.sim.BitsNotSetSign);
		}
		else
		{
			if (it->b2NumWordField)
				error.push_back(ErrorBase.sim.BitsOneInter);
			if (it->b2BitField)
				error.push_back(ErrorBase.sim.BitsTwoInter);
		}
	}

	if (error.empty())
		return true;

	error.push_front(errRemarks[5]);
	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);

	return false;
}

// �������� �� ���������� �����
bool CTest::checkCrossBits(list<signalData>::iterator& it, bool repiter[][32])
{
	bool result = false;
	int nInterval;
	(it->b2BitField) ? nInterval = 1 : nInterval = 2;

	for (int i = 0; i < nInterval; i++)
	{
		int start, end;

		if (i == 0)
		{
			start = it->iBit[0];
			end = it->iBit[1];
		}
		else
		{
			start = it->iBit[2];
			end = it->iBit[3];
		}

		if (end == 0)
			end = start;

		// int param_povt_f = -1, param_povt_s = -1;
		// CString boot = _T("");

		for (; start <= end; start++)
		{
			if (repiter[it->iNumWord[i] - 1][start - 1])
				repiter[it->iNumWord[i] - 1][start - 1] = false;
			else
				result = true;
			/*{
				result = true;
				if (param_povt_f == -1)
				{
					param_povt_f = start_bit;
					boot.Format(_T("%d"), param_povt_f);
				}

				if (param_povt_s == -1)
					param_povt_s = start_bit;

				if ((start_bit - param_povt_s) > 1)
				{
					if (param_povt_f == param_povt_s)
						boot.Format(_T("%s,%d"), boot, param_povt_s);
					else
						boot.Format(_T("%s...%d,%d"), boot, param_povt_s, start_bit);
					param_povt_f = start;
					param_povt_s = start;
				}

				if ((start - param_povt_s) == 1)
					param_povt_s = start;
			}

			if (start == end)
				if (param_povt_f != param_povt_s)
					boot.Format(_T("%s...%d"), boot, param_povt_s);*/
		}
	}
	return result;
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
		num2.Delete(0, indxDot + 1);
		num2.Trim();
		it->iNumWord[1] = _wtoi(num2);

		num.Delete(indxDot, num.GetLength());	// ������ �����
		num.Trim();
		it->iNumWord[0] = _wtoi(num);

		it->b2NumWordField = false; // ��������� ����� ����������� ���� ����
	}
}

// ������� �� ���, ���� � ���
void CTest::translateMMC(list<signalData>::iterator& it)
{
	it->iMinMaxCsrVal[0] = stepTranslateMMC(it->sMinMaxCsrValField[0]);
	it->iMinMaxCsrVal[1] = stepTranslateMMC(it->sMinMaxCsrValField[1]);
	it->iMinMaxCsrVal[2] = stepTranslateMMC(it->sMinMaxCsrValField[2]);
}

// �������������� ������� �������� ���, ����, ���
double CTest::stepTranslateMMC(CString value)
{
	value.Replace(_T(','), _T('.'));
	return _wtof(value);
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
		bits2.Delete(0, indxDot + 1);
		bits.Delete(indxDot, bits.GetLength());
		bits.Trim();	bits2.Trim();

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
		CString num2 = num;	// ������ �����

		num.Delete(indxDel, num.GetLength());	// ������ �����
		num.Trim();
		result[0] = _wtoi(num);

		num2.Delete(0, indxDel + 1);
		num2.Trim();
		result[1] = _wtoi(num2);
	}

	return result;
}

// ������� ��������� ���� ��� NP ������, ���� �� ����
void CTest::translateComment(list<signalData>::iterator& it)
{
	int indx = it->sCommentField.Find(_T("��-"));
	CString tmp = it->sCommentField;

	if (indx != -1)	// ������ ��������� ����
	{
		tmp.Delete(0, indx + 3);
		
		int iSpace = tmp.Find(_T(' '));
		if (iSpace == -1)
			iSpace = tmp.Find(_T('\n'));
		if (iSpace == -1)
			iSpace = tmp.Find(_T('\0'));

		tmp.Delete(iSpace, tmp.GetLength());

		it->iBitSigns = _wtoi(tmp);

		it->bCommentField = false;	// ��������� ����� ����������� ����� � ����������
	}

	indx = tmp.Find(_T("NP="));

	if (indx != -1)	// ������ NP
	{
		tmp.Delete(0, indx + 3);

		int iSpace = tmp.Find(_T(' '));
		if (iSpace == -1)
			iSpace = tmp.Find(_T('\n'));
		if (iSpace == -1)
			iSpace = tmp.Find(_T('\0'));

		tmp.Delete(iSpace, tmp.GetLength());

		NP = _wtoi(tmp);
	}
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