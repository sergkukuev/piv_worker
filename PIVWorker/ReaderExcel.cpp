#include "stdafx.h"
#include "ReaderExcel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// �����������
CReaderExcel::CReaderExcel()
{
	// ��������� ������ ����������
	header.list.push_back(vector<CString>{ L"� ����", L"� �����", L"� �/�", L"� ����� � ���������" });				// ��� ������ �����
	header.list.push_back(vector<CString>{ L"������������ ���������", L"������������ �������" });					// ��� ������������ ���������
	header.list.push_back(vector<CString>{ L"����������� �������", L"�������� ����������� ��������� / �������" });	// ��� �������
	header.list.push_back(vector<CString>{ L"�����������", L"������� ���������" });									// ��� ������� �����������
	header.list.push_back(vector<CString>{ L"����������� ��������", L"�����. ��������", L"������� ��������" });		// ����������� ��������
	header.list.push_back(vector<CString>{ L"������������ ��������", L"������. ��������", L"�������� ��������" });	// ������������ ��������
	header.list.push_back(vector<CString>{ L"���� �������� �������" });												// ��� ���
	header.list.push_back(vector<CString>{ L"������������ �������", L"���������-��� �������" });					// ��� ������������ ��������
	header.list.push_back(vector<CString>{ L"����������" });														// ����������� (���������)
	header.list.push_back(vector<CString>{ L"�����"});																// ����� (��� ARINC)

	// �������������� ���������� Excel ������
	extension.push_back(L"xlsx");	// ������� �������� ������
	extension.push_back(L"xls");	// �������� ������ �� 2003 ����
}

// ����������
CReaderExcel::~CReaderExcel() 
{
	header.list.clear();
	header.list.shrink_to_fit();
	extension.clear();
	extension.shrink_to_fit();
}

// ������ ���������
bookData CReaderExcel::GetBook(const CString& pathToExcel, bool bProject) 
{
	bookData book;
	CWorkExcel work;

	bProj = bProject;

	if (!CheckExtension(pathToExcel))
		throw BadTypeException(work.BookName(pathToExcel));

	if (!work.OpenBook(pathToExcel))
		throw ReadBookException(work.BookName(pathToExcel));

	book.name = work.BookName();
	GetSheets(book.sheets, work);
	
	return book;
}

// ������ ������ ����������
void CReaderExcel::GetSheets(vector <sheetData>& sheets, CWorkExcel& work) 
{
	sheets.resize(work.CountSheets());
	for (long i = 1; i < work.CountSheets() + 1; i++) 
	{
		work.OpenSheet((long)i);
		sheets[i - 1].name = work.SheetName();

		if (!work.FindHeader(header)) // ����� ���������� �������
			throw NotAllHeaderException(work.BookName(), sheets[i - 1].name);

		sheets[i - 1].arinc = work.IsArinc();
		sheets[i - 1].pk = work.PkValue(header);

		header.adress[header.iRow]++;

		GetSignals(sheets[i - 1].signals, work, sheets[i - 1].arinc);
		sheets[i - 1].arinc ? sheets[i - 1].np = -1 : sheets[i - 1].np = work.NpValue(sheets[i - 1].signals[0].comment/*header*/);
	}
}

// ������ ���������� �� �����
void CReaderExcel::GetSignals(vector <signalData>& signals, CWorkExcel& work, const bool& isArinc) 
{
	arincData arinc;
	for (long merge = 1, row = header.adress[header.iRow], column; row < work.CountRows() + 1; row += merge, merge = 1) 
	{
		signalData signal;
		// ���������� ��� arinc
		if (isArinc) 
		{
			if (IsTitle(work, row))
				continue;
			//	����� ����������
			CString remark = work.CellValue(row - 1, 1);
			if (remark.Find(ARINC_REMARK) != -1 && arinc.flag && arinc.startRow != row)	// ������, ���������
				GetArinc(remark, row, arinc);
			else if ((remark.Find(ARINC_REMARK) != -1 || IsTitle(work, row - 1)) && !arinc.flag) 
			{
				if (arinc.current == arinc.amount)
					GetArinc(remark, row, arinc);
				else
				{
					row = arinc.startRow;
					arinc.current++;
				}
			}
		}

		// ������ ���������� ������ �������
		// ����� (ARINC) ������������ ������ ������ �����
		if (!isArinc) 
		{
			column = header.adress[header.iNumWord];
			int range = work.CPrevEmpty(row, column);
			signal.numWord = GetNumWord(work.CellValue(row - range, column));
		}
		else 
		{
			column = header.adress[header.iAdress];
			int range = work.CPrevEmpty(row, column);
			signal.numWord = GetAdress(work.CellValue(row - range, column), arinc.current);
		}

		column = header.adress[header.iName];
		int range = work.CPrevEmpty(row, column);
		signal.title[0] = work.CellValue(row - range, column);
		range = work.CNextEmpty(row, column);
		
		column = header.adress[header.iSignal];
		merge = work.CNextEmpty(row, column);
		signal.title[1] = work.CellValue(row, column);

		if (range < merge) 
			merge = range;
		
		// ������ �����, ���� ������� ����������
		if (!arinc.flag) 
		{
			CString tmp;
			// ������� ������������ ��������� � ��������� (������� ����� ������� ���������)
			tmp.Format(L"%d", arinc.current);
			int pos = signal.title[1].Find(arinc.symbol);
			signal.title[1].Replace(arinc.symbol, tmp);
		}

		column = header.adress[header.iDimension];
		signal.dimension = work.CellValue(row, column);
		GetMinMaxCsr(signal, work, row);	
		column = header.adress[header.iBits];
		signal.bit = GetBits(work.CellValue(row, column), (int)signal.numWord.value.size());
		signal.comment = GetComment(work, row, merge, signal.bitSign);

		bool bEmpty = IsEmpty(work, row);	
		bool bRemark = IsRemark(work, row);
		
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
		
		// TODO: ����������� ������� (����� ���������� � ������ ����� �������)
		if (work.CountRows() == row + merge - 1 && !arinc.flag && arinc.amount != arinc.current)
			row = arinc.startRow - merge;
	}
	// TODO: ���� ��� �����35
	if (!bProj)
		ConcatDW(signals);
}

// ������ ������ ���������� � ARINC ��������� (���������� ����� � �����)
void CReaderExcel::GetArinc(const CString& field, const long& row, arincData& arinc) 
{
	CString numeric = field;
	int posEqual = numeric.Find(L'=');

	if (posEqual != -1) 
	{
		arinc.symbol = field[posEqual - 1];
		arinc.startRow = row;
		arinc.flag = false;

		// ��������� ������� ����� ���������
		numeric.Delete(0, posEqual + 1);
		int posDel = numeric.ReverseFind(L'�') == -1 ? numeric.ReverseFind(L'.') : numeric.ReverseFind(L'�');
		CString numeric1 = numeric.Mid(posDel + 1, numeric.GetLength() - posDel);
		
		// ��������� ������� ����� ���������
		posDel = numeric.Find(L'�') == -1 ? numeric.Find(L'.') : numeric.Find(L'�');
		numeric.Delete(posDel, numeric.GetLength() - posDel);
		numeric.Trim();	numeric1.Trim();
		
		arinc.current = GetInt(numeric, arinc.flag);
		arinc.amount = GetInt(numeric1, arinc.flag);
	}
	else
	{
		// ����� ��������� ����������
		arinc.flag = true;
		arinc.symbol = L"";
		arinc.amount = 0;
		arinc.current = 0;
	}
}

// ����������� ������� ����
void CReaderExcel::ConcatDW(vector <signalData>& signals)
{
	CString LOW_PART = L"(��.�)";
	CString HIGHT_PART = L"(��.�)";

	for (size_t i = 0; i < signals.size(); i++)
	{
		if (signals[i].title[0].Find(LOW_PART) != -1) 
		{
			int numeric = ParsePart(signals[i].comment);
			if (numeric != -1)
			{
				CString temp = L"��.�. � ��." + signals[i].numWord.value[0];
				int index = findSignalByNum(signals, i, numeric, HIGHT_PART, temp);
				if (index != -1) 
				{
					signals[i].title[0].Replace(LOW_PART, L"");
					temp.Format(L"��.�. � ��.%d", numeric);
					signals[i].comment.Replace(temp, L"");
					signals[i].numWord.value.push_back(signals[index].numWord.value[0]);
					signals[i].bit.value.push_back(signals[index].bit.value[0]);
					signals[i].bit.value.push_back(signals[index].bit.value[1]);

					signals.erase(signals.begin() + index);
				}
			}
		}
		else if (signals[i].title[0].Find(HIGHT_PART) != -1) 
		{
			int numeric = ParsePart(signals[i].comment);
			if (numeric != -1)
			{
				CString temp = L"��.�. � ��.";
				temp.Format(L"%s%d",temp, signals[i].numWord.value[0]);
				int index = findSignalByNum(signals, i, numeric, LOW_PART, temp);
				if (index != -1)
				{
					signals[i].title[0].Replace(HIGHT_PART, L"");
					temp.Format(L"��.�. � ��.%d", numeric);
					signals[i].comment.Replace(temp, L"");
					signals[i].numWord.value.push_back(signals[index].numWord.value[0]);
					signals[i].bit.value.push_back(signals[index].bit.value[0]);
					signals[i].bit.value.push_back(signals[index].bit.value[1]);

					signals.erase(signals.begin() + index);
				}
			}
		}
	}
}

// ������� ������ � ������������� ������ ����� �����
int CReaderExcel::ParsePart(const CString& part)
{
	// ��.�. � ��. // ��.�. � ��.
	CString temp = L".�. � ��.";
	int result = -1;
	int posPart = part.Find(temp);
	
	if (posPart != -1)
	{
		bool error = false;
		CString num = part.Mid(posPart + temp.GetLength());
		result = GetInt(num, error);
		if (error)
			result = -1;
	}
	return result;
}

int CReaderExcel::findSignalByNum(vector <signalData>& signals, size_t start, int numeric, CString& partNum, CString& partComment)
{
	int result = -1;
	for (size_t i = start; i < signals.size(); i++)
	{
		if (signals[i].numWord.value[0] == numeric)
			if (signals[i].title[0].Find(partNum) != -1 && signals[i].comment.Find(partComment) != -1)
			{
				result = i;
				break;
			}
	}
	return result;
}

// ��������� ������ ��������� ���������� �������
#pragma region Parameters
// ������ (ARINC)
intData CReaderExcel::GetAdress(const CString& field, const int& num) 
{
	intData result;
	CString numeric = field;
	int adr = -1;
	int posDel, current = num;
	bool flag = true;

	do 
	{
		posDel = numeric.Find(L',') == -1 ? numeric.GetLength() : numeric.Find(L',');
		CString str = numeric.Mid(0, posDel);
		numeric.Delete(0, posDel + 1);
		str.Replace(L'\n', L' ');
		vector<int> temp = StepAdress(str, result.flag);	// ������ - �����, �������� - ������� ���������
		
		if (temp.size() == 4) 
		{
			if (current > temp[2] - temp[0] + 1)
				current -= temp[2] - temp[0] + 1;
			else 
			{
				adr = temp[0] + current - 1;
				result.sys = temp[1];
				flag = false;
			}
		}
		else if (temp.size() == 2 && current == 0) 
		{
			adr = temp[0];
			result.sys = temp[1];
			flag = false;
		}
		else
			result.flag = true;
	} 
	while (numeric.GetLength() != 0 && flag);

	result.value.push_back(adr);
	result.field = field;
	return result;
}

// ��� ������� ��� ������
vector <int> CReaderExcel::StepAdress(const CString& adress, bool& flag) 
{
	vector <int> result = {-1, -1};
	CString num = adress;
	int indxDel = num.Find(L'.') == -1 ? num.Find(L'�') : num.Find(L'.');	// ����� ������� ������������

	// ������������ ���, ������������ ���� ������
	if (indxDel == -1) 
	{ 
		result[1] = GetSubIndex(num);
		result[0] = GetInt(num, flag);
	}
	else 
	{
		result.push_back(-1);
		result.push_back(-1);
		int posDot = num.ReverseFind(L'.');
		if (posDot != -1)
			num.Delete(indxDel, posDot - indxDel);

		CString num2 = num;
		num.Delete(indxDel, num.GetLength());	// ������ �����
		num2.Delete(0, indxDel + 1);			// ������ �����
		num.Trim();	num2.Trim();
		
		result[1] = GetSubIndex(num);
		result[3] = GetSubIndex(num2);
		result[0] = GetInt(num, flag);
		result[2] = GetInt(num2, flag);
	}
	return result;
}

// ��������� ������������ ������� � �������� ��� �� ������
// ����������� ������ � �����(8320 - 8329)
// TODO: ����������� ������� (� ������ ����� ������� ���������, �������� �� 8-��, ����� ��������� ��������) 
int CReaderExcel::GetSubIndex(CString& numeric) 
{
	CString sub;	
	// ����� ������� ������������ �������
	for (int i = 0; i < numeric.GetLength(); i++) 
	{
		if ((int)numeric[i] > 8319)
		{
			sub = numeric.Mid(i, numeric.GetLength() - i);
			numeric.Delete(i, numeric.GetLength() - i);
			break;
		}
	}

	int l = sub.GetLength();
	for (int i = 0; i < l; i++)
		sub += (wchar_t)((int)sub[i] - 8272); // �������� ������������ � ������� �����
	sub.Delete(0, l);

	bool flag = false;
	int result = GetInt(sub, flag);
	flag ? result = -1 : result = result;
	return result;
}

// ����� �����
intData CReaderExcel::GetNumWord(const CString& field) 
{
	intData result;
	vector <int> vec = { -1 };
	CString numeric = field;
	int posDot = numeric.Find(L',');
	numeric.Trim();

	if (posDot == -1)	// ���� �����
		vec[0] = GetInt(numeric, result.flag);
	else 
	{
		vec.push_back(-1);
		CString numeric2 = numeric;	
		numeric.Delete(posDot, numeric.GetLength());	// ������ �����
		numeric2.Delete(0, posDot + 1);					// ������ �����
		numeric.Trim();	numeric2.Trim();

		vec[0] = GetInt(numeric, result.flag);
		vec[1] = GetInt(numeric2, result.flag);
	}
	result.value = vec;
	result.field = field;
	return result;
}

// �����������, ������������ � ���
void CReaderExcel::GetMinMaxCsr(signalData& signal, CWorkExcel& work, const long& row) 
{
	long column = header.adress[header.iMin];
	signal.min.field = work.CellValue(row, column);
	signal.min.value = GetDouble(signal.min.field, signal.min.flag);

	column = header.adress[header.iMax];
	signal.max.field = work.CellValue(row, column);
	signal.max.value = GetDouble(signal.max.field, signal.max.flag);

	column = header.adress[header.iCSR];
	signal.csr.field = work.CellValue(row, column);
	signal.csr.value = GetDouble(signal.csr.field, signal.csr.flag);

	// ����� ������, ���� ������������ ��� ��������� �����
	if (signal.min.value == DBL_MIN && signal.max.value == DBL_MIN && signal.csr.value == DBL_MIN) 
	{
		signal.min.flag = false;
		signal.max.flag = false;
		signal.csr.flag = false;
	}
}

// ������������ �������
intData CReaderExcel::GetBits(const CString& field, const int& size)
{
	intData result;
	CString bits = field;
	int posDot = bits.Find(L',');
	vector <int> vec;

	// ��� ������ ����������
	if (posDot == -1)
		vec = StepBits(bits, result.flag);
	else // ��� ���� �����������
	{ 
		CString bits2 = bits;
		bits2.Delete(0, posDot + 1);
		bits.Delete(posDot, bits.GetLength());
		bits.Trim();	bits2.Trim();

		vec = StepBits(bits, result.flag);		// ������ ����������
		vector <int> vec2 = StepBits(bits2, result.flag);	// ������ ����������

		if (size == 2) 
		{
			vec.resize(4);
			for (size_t i = 0; i < vec2.size(); i++)
				vec[i + 2] = vec2[i];
		}
		else
			vec[1] = vec2[0];
	}
	result.field = field;
	result.value = vec;
	return result;
}

// �������������� ������� ��� �������� ��������
vector <int> CReaderExcel::StepBits(const CString& bits, bool& flag) 
{
	vector <int> result = { -1, -1 };
	CString num = bits;
	num.Trim();

	int indxDel = num.Find(L'.') == -1 ? num.Find(L'�') : num.Find(L'.');	// ����� ������� ������������

	if (indxDel == -1)	// ������������ ���, ������������ ���� ������
		result[0] = GetInt(num, flag);
	else
	{
		int posDot = num.ReverseFind(L'.');
		if (posDot != -1)
			num.Delete(indxDel, posDot - indxDel);

		CString num2 = num;
		num.Delete(indxDel, num.GetLength());	// ������ �����
		num2.Delete(0, indxDel + 1);	// ������ �����
		num2.Trim();	num.Trim();

		result[0] = GetInt(num, flag);
		result[1] = GetInt(num2, flag);
	}
	return result;
}

// ������ ����������
CString CReaderExcel::GetComment(CWorkExcel& work, const long& row, const int& size, bool& flag) 
{
	long column = header.adress[header.iComment];
	long tmpRow = row;
	int merge = work.GetMerge(tmpRow, column);
	CString result;

	size > merge ? merge = size : merge = merge;
	for (int i = 0; i < merge; i++) 
	{
		CString tmp = work.CellValue(tmpRow + i, column);
		if (!tmp.IsEmpty()) 
		{
			(tmp.Find(SIGN_FIELD) != -1) ? flag = true : flag = flag;
			result.IsEmpty() ? result = tmp : result += L'\n' + tmp;
		}
	}
	return result;
}
#pragma endregion

#pragma region Converters
// ��������� double ��������
double CReaderExcel::GetDouble(const CString& field, bool& flag) 
{
	double result = DBL_MIN;
	CString temp = field;
	temp.Trim();
	temp.Replace(L',', L'.');
	CStringA tmp(temp);
	char* str = new char[tmp.GetLength() + 1];
	strcpy_s(str, tmp.GetLength() + 1, tmp);

	if (!temp.IsEmpty()) 
	{
		char* end;
		errno = 0;
		result = strtod(str, &end);
		(*end != '\0' || errno != 0) ? flag = true : flag = flag;
	}
	else
		flag = true;

	delete[] str;
	return result;
}

// ��������� int ��������
int CReaderExcel::GetInt(const CString& field, bool& flag) 
{
	int result = -1;
	CString temp = field;
	temp.Trim();
	CStringA tmp(temp);
	char* str = new char[tmp.GetLength() + 1];
	strcpy_s(str, tmp.GetLength() + 1, tmp);

	if (!field.IsEmpty()) 
	{
		char* end;
		errno = 0;
		result = strtol(str, &end, 10);
		(*end != '\0' || errno != 0) ? flag = true : flag = flag;
	}
	else
		flag = true;

	delete[] str;
	return result;
}
#pragma endregion

// ��������� ������ ��������
#pragma region SUB_FUNCTION
// �������� ���������
bool CReaderExcel::IsTitle(CWorkExcel& work, const long& row)
{
	bool result = false;
	CString numeric = work.CellValue(row, 1);
	if (!numeric.IsEmpty())
		GetInt(numeric, result);
	return result;
}

// �������� �������
bool CReaderExcel::IsEmpty(CWorkExcel& work, const long& row) 
{
	bool result = true;
	for (long i = 1; i < header.size; i++)
	{
		long column = header.adress[i];
		if (header.adress[i] != -1) 
		{
			CString tmp = work.CellValue(row, column);
			if (!tmp.IsEmpty())
				result = false;
		}
	}
	return result;
}

// �������� ����������
bool CReaderExcel::IsRemark(CWorkExcel& work, const long& row)
{
	bool result = false;
	for (long i = 1; i < header.size; i++) 
	{
		long column = header.adress[i];
		if (header.adress[i] != -1)
		{
			CString tmp = work.CellValue(row, column);
			result = (tmp.Find(REMARK1) > -1 ||
				tmp.Find(REMARK2) > -1) ? true : result;
		}
	}
	return result;
}

// �������� ����������
bool CReaderExcel::CheckExtension(const CString& path)
{
	bool result = false;
	int dotPos = path.ReverseFind(L'.');	// ����� �������
	CString ext = path.Mid(dotPos + 1);		// ��������� ���������� �� ����

	for (size_t i = 0; i < extension.size(); i++)
		if (ext.CompareNoCase(extension[i]) == 0)
			result = true;

	return result;
}
#pragma endregion