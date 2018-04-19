#include "stdafx.h"
#include "ReaderExcel.h"
#include "SubFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace readdll;
using namespace exceldll;

// �����������
CReaderExcel::CReaderExcel()
{
	Initialize();
}

// �������������
void CReaderExcel::Initialize()
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
	extension.push_back(L"xlsb");
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
bookData CReaderExcel::GetBook(const CString& pathToExcel)
{
	bookData book;

	if (!ExtensionChecker(pathToExcel))
		throw BadTypeException(work.BookName(pathToExcel));

	if (!work.OpenBook(pathToExcel))
		throw ReadBookException(work.BookName(pathToExcel));

	book.name = work.BookName();
	// UNDONE: ����������� ������ � ������ ���������
	// logger.Write(L"���������� ��������� \"" + book.name + L"\"...");
	logger >> L"���������� ��������� \"" + book.name + L"\"...";
	GetSheets(book.sheets);
	//logger >> L"���������� ��������� \"" + book.name + L"\" ���������";

	return book;
}

// ������ ������ ���������� (������)
void CReaderExcel::GetSheets(vector <sheetData>& sheets)
{
	sheets.resize(work.CountSheets());
	for (long i = 1; i < work.CountSheets() + 1; i++)
	{
		try
		{
			if (!work.OpenSheet(i))	// ���� ������
				throw ReadSheetException(work.SheetName());

			sheets[i - 1].name = work.SheetName();
			sheets[i - 1].arinc = IsArinc();

			if (!work.FindHeader(header, sheets[i - 1].arinc))	// ����� ���������� �������
				throw NotAllHeaderException(sheets[i - 1].name);

			header.adress[iRow]++;
			GetSignals(sheets[i - 1].signals, sheets[i - 1].arinc);
			settings.GetProject() == stgdll::project::kprno35 ? sheets[i - 1].pk = GetPuiPage(sheets[i - 1].signals) : sheets[i - 1].pk = GetPk();
			sheets[i - 1].arinc ? sheets[i - 1].np.value = stats::opt : GetNp(sheets[i - 1].signals, sheets[i - 1].np);

			if (sheets[i - 1].pk < 0)	// �������� ������ ���� -> ������ ������
				sheets[i - 1].pk == stats::empty ?
					logger >> L"����� �������� ����������� �� ����� \"" + sheets[i - 1].name + L"\"" :
					logger >> L"�� ������� ��������� ����� �������� �� ����� \"" + sheets[i - 1].name + L"\"";

			if (sheets[i - 1].np.value < 0)	// �������� ������ ���� -> ������ ������
				sheets[i - 1].np.value == stats::empty ?
					logger >> L"����� ������ ���������� ����������� �� ����� \"" + sheets[i - 1].name + L"\"" :
					logger >> L"�� ������� ��������� ����� ������ ���������� �� ����� \"" + sheets[i - 1].name + L"\"";
		}
		catch (MyException& exc)
		{
			logger >> exc.GetMsg();
		}
	}
}

// ������ ���������� �� �����
void CReaderExcel::GetSignals(vector <signalData>& signals, const bool& bArinc)
{
	arincData arinc;
	// TODO: ������������ �������� �������� 930�
	for (long merge = 1, row = header.adress[iRow], column; row <= work.CountRows(); row += merge, merge = 1)
	{
		signalData signal;
		// ���������� ��� arinc
		if (bArinc)
			if (!IsTitle(row))
				ArincChecker(arinc, row);
			else
				continue;

		// ������ ���������� ������ �������. ����� (��� ARINC) ������������ ������ ������ �����
		if (!bArinc)
		{
			column = header.adress[iNumWord];
			int range = work.CPrevEmpty(row, column);
			signal.numWord = GetNumWord(work.CellValue(row - range, column));
		}
		else
		{
			column = header.adress[iAdress];
			int range = work.CPrevEmpty(row, column);
			signal.numWord = GetAdress(work.CellValue(row - range, column), arinc.current);
		}

		column = header.adress[iName];
		int range = work.CPrevEmpty(row, column);
		signal.title[0] = work.CellValue(row - range, column);
		range = work.CNextEmpty(row, column);

		column = header.adress[iSignal];
		merge = work.CNextEmpty(row, column);
		signal.title[1] = work.CellValue(row, column);

		if (range < merge)
			merge = range;

		// ������ �����, ���� ������� ����������
		if (!arinc.flag)
		{
			CString tmp;
			tmp.Format(L"%d", arinc.current);
			myctl::Cyr2Lat(signal.title[1]);	// �������������� ��������� � ��������
			signal.title[1].Replace(arinc.symbol, tmp);
		}

		column = header.adress[iDimension];
		signal.dimension = work.CellValue(row, column);
		GetMinMaxCsr(signal, row);
		column = header.adress[iBits];
		signal.bit = GetBits(work.CellValue(row, column), (int)signal.numWord.value.size());
		signal.comment = GetComment(row, merge, signal.bitSign);

		bool bEmpty = IsEmpty(row);
		bool bRemark = IsRemark(row);
		// TODO: �������� ���������� ��� �������� ��� ���������� �����������
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
		else if (bRemark)
			break;

		// CRUTCH: ����� ���������� � ������ ����� ������� (arinc)
		if (work.CountRows() == row + merge - 1 && !arinc.flag && arinc.amount != arinc.current)
			row = arinc.startRow - merge;
	}

	// ����� ������� ����
	if (settings.GetProject() == stgdll::project::kprno35)
		ConcatDW(signals);
}

#pragma region DoubleWord
// ����������� ������� ���� (��������� ���������)
void CReaderExcel::ConcatDW(vector <signalData>& signals)
{
	for (size_t i = 0; i < signals.size(); i++)
	{
		for (size_t j = 0; j < dwPart::low.size(); j++)
		{
			ASSERT(dwPart::low.size() != dwPart::hight.size() != dwPart::hight2.size());
			if (signals[i].title[0].Find(dwPart::hight[j]) != -1)
				findDW(signals, i, dwPart::hight[j], dwPart::low);
			else if (signals[i].title[0].Find(dwPart::hight2[j]) != -1)
				findDW(signals, i, dwPart::hight2[j], dwPart::low);
			else if (signals[i].title[0].Find(dwPart::low[j]) != -1)
				findDW(signals, i, dwPart::low[j], dwPart::hight);
		}
	}
}

// ����� ������ ����� �������� �����
void CReaderExcel::findDW(vector <signalData>& signals, size_t start, CString old, vector <CString> revert)
{
	CString title = signals[start].title[0];
	title.Remove(L'\n');

	int index = -1, rev_index = 0;
	for (size_t i = start; i < signals.size(); i++)
	{
		CString tmp = signals[i].title[0];
		tmp.Remove(L' ');
		tmp.Remove(L'\n');
		for (size_t j = 0; j < revert.size(); j++)
		{
			CString rev_title = title;
			rev_title.Replace(old, revert[j]);
			rev_title.Remove(L' ');
			if (tmp.Compare(rev_title) == 0)
			{
				rev_index = (int)j;
				index = (int)i;
				break;
			}
		}
		if (index != -1)
		{
			signals[start].part = &signals[index];
			signals[index].part = &signals[start];
			break;
		}
	}
}
#pragma endregion

#pragma region Arinc
//  ����� ������������� ������
void CReaderExcel::ArincChecker(arincData& arinc, long& row)
{
	//	����� ����������
	CString field = work.CellValue(row - 1, 1);
	if (field.Find(arincRemark) != -1 && arinc.flag && arinc.startRow != row)	// ������, ���������
		GetArinc(arinc, row, field);
	else if ((field.Find(arincRemark) != -1 || IsTitle(row - 1)) && !arinc.flag)
	{
		if (arinc.current == arinc.amount)
			GetArinc(arinc, row, field);
		else
		{
			row = arinc.startRow;
			arinc.current++;
		}
	}
}

// ������ ������ ���������� � ARINC ��������� (���������� ����� � �����)
void CReaderExcel::GetArinc(arincData& arinc, const long& row, CString numeric)
{
	int posEqual = numeric.Find(L'=');
	if (posEqual != -1)
	{
		arinc.symbol = numeric[posEqual - 1];
		myctl::Cyr2Lat(arinc.symbol);	// ������� ��������� � ��������
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

// ������ ������
intData CReaderExcel::GetAdress(CString numeric, int current)
{
	intData result;
	int adr = -1;
	int posDel;
	bool flag = true;
	result.field = numeric;

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
	} while (numeric.GetLength() != 0 && flag);

	result.value.push_back(adr);
	return result;
}

// �������������� ������� ��� �������� ������
vector <int> CReaderExcel::StepAdress(CString numeric, bool& flag)
{
	vector <int> result = { -1, -1 };
	int indxDel = numeric.Find(L'.') == -1 ? numeric.Find(L'�') : numeric.Find(L'.');	// ����� ������� ������������

																						// ������������ ���, ������������ ���� ������
	if (indxDel == -1)
	{
		result[1] = GetSubIndex(numeric);
		result[0] = GetInt(numeric, flag);
	}
	else
	{
		result.push_back(-1);
		result.push_back(-1);
		int posDot = numeric.ReverseFind(L'.');
		if (posDot != -1)
			numeric.Delete(indxDel, posDot - indxDel);

		CString numeric2 = numeric;
		numeric.Delete(indxDel, numeric.GetLength());	// ������ �����
		numeric2.Delete(0, indxDel + 1);			// ������ �����
		numeric.Trim();	numeric2.Trim();

		result[1] = GetSubIndex(numeric);
		result[3] = GetSubIndex(numeric2);
		result[0] = GetInt(numeric, flag);
		result[2] = GetInt(numeric2, flag);
	}
	return result;
}

// ��������� ������������ ������� � �������� ��� �� ������
// ����������� ������ � �����(8320 - 8329)
// TODO: � ������ ��������� ������� ��������� (��), �������� �� 8-��, ����� �������� ������������ ���������
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
#pragma endregion

#pragma region SheetInfo
// ����� ������� �� ��������������
int CReaderExcel::FindSignalById(const vector<signalData>& signals, const CString& id)
{
	int index = -1;
	for (size_t i = 0; i < signals.size(); i++)
		if (signals[i].title[1].Find(id) == 0)
		{
			index = (int)i;
			break;
		}
	return index;
}

// ��������� �������� ������ ������
void CReaderExcel::GetNp(vector<signalData>& signals, npData& np) 
{
	int2 result = ParseValueById(signals, npId);
	np.value = result.value;
	np.index = result.index;
}

// ��������� �������� �������� ���
int CReaderExcel::GetPuiPage(const vector<signalData>& signals)
{
	return ParseValueById(signals, puiId).value;
}

// ��������� �������� �� ����������� �� �������������� (��� ��� � ��)
int2 CReaderExcel::ParseValueById(const vector<signalData>& signals, const vector<CString>& id)
{
	int2 res;
	int iId = -1;
	for (size_t i = 0; i < id.size(); i++)	// ������� ������ ��������������
	{
		res.index = FindSignalById(signals, id[i]);
		if (res.index != -1)
		{
			iId = (int)i;
			break;
		}
	}
	// ��������������� ������������� �� ������
	if (res.index == -1)
	{
		res.value = stats::empty;
		return res;
	}
	CString comment = signals[res.index].comment;
	if (!comment.IsEmpty())
	{
		int pos = comment.Find(id[iId]);
		if (pos == -1)	// ���� �� �������
		{
			res.value = stats::failed;
			return res;
		}
		comment.Remove(L' ');	// ������� ��� ������� ��� �������� ��������� ��������
		// TODO: ���������� �� ����������� ����� (, . ! \n \t � �.�.)
		bool result = true;
		res.value = GetInt(comment.Mid(pos + id[iId].GetLength() + 1), result);	// ����� �������������� + ������ "="
	}
	return res;
}

// ��������� �������� ������ ��������
int CReaderExcel::GetPk()
{
	// CRUTCH: ���� ��� ������� �� ������� ������, ����� ����� ����� ��������
	for (long i = header.adress[iComment]; i <= work.Boundary().column; i++)
	{
		CString item = work.CellValue(header.adress[iRow] - 1, i);
		if (!item.IsEmpty())
		{
			int pos = item.ReverseFind(PK_FIELD);
			if (pos == -1)
				continue;
			item.Delete(0, pos + 1);
			item.Trim();
			// ����� ������ ������
			pos = item.Find(L' ');
			if (pos != -1)
				item.Delete(pos, item.GetLength());
			// � ����� ������������ ����� ������� (��������� �� �������)
			pos = item.Find(L',');
			if (pos != -1)
				item.Delete(pos, item.GetLength());

			bool flag = false;
			return GetInt(item, flag);
		}
	}
	return stats::empty;
}
// UNDONE: ��������� ��� ������ � ����� ������ (����������� ������� ������� � ��� �������)
/*
#pragma region NP_BASE
// ������ ������� ������� �� ����� odb
void CWorkExcel::ReadNpBase()
{
	vector<CString> fileNames;	// ��������� ����� ������
								// ����� ���� ������ � ����� 
	WIN32_FIND_DATA findFiles;
	HANDLE h;
	CString folder = settings.GetDefaultPath() + L"\\odb\\*";
	h = FindFirstFile(folder, &findFiles);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			fileNames.push_back(findFiles.cFileName);
		} while (FindNextFile(h, &findFiles) != 0);
		FindClose(h);
	}
	folder.Delete(folder.GetLength() - 1);	// �������� ��������� �� ���� 
											// ������ ������ 
	for (size_t i = 0; i < fileNames.size(); i++)
	{
		// �������� ����������
		int posDot = fileNames[i].ReverseFind(L'.');
		CString temp = fileNames[i].Mid(posDot, fileNames[i].GetLength() - posDot);
		if (temp.Compare(L".np") != 0)
			continue;

		// ���������� ������ � ������
		temp.Format(L"%s\\%s", folder, fileNames[i]);
		CStdioFile file;
		BOOL bOper = file.Open(temp, CFile::modeRead | CFile::typeText);
		do
		{
			bOper = file.ReadString(temp);
			// ����� ��������� � 1252 -> 1251
			int nChar = ::WideCharToMultiByte(CP_UTF8, 0, temp, (int)temp.GetLength(), NULL, 0, NULL, NULL);
			string result;
			result.resize(nChar);
			::WideCharToMultiByte(1252, 0, temp, (int)temp.GetLength(), const_cast<char*>(result.c_str()), nChar, NULL, NULL);
			// �������
			np_s np;
			bool error = false;
			size_t pos = result.find_first_of("\t");
			np.number = ParseNp(result.substr(0, pos), error);
			if (!error)
			{
				result = result.substr(pos + 1, result.size());
				pos = result.find_first_of("\t");
				np.name = ParseFrameName(result.substr(0, pos));
				AddToNpBase(np);
			}
		} while (bOper);
		file.Close();
	}
}

// ���������� � ���� ������� ����������
void CWorkExcel::AddToNpBase(const np_s& np)
{
	bool find = false;
	for (size_t i = 0; i < npBase.size() && !find; i++)
		if (npBase[i].name.Compare(np.name) == 0)
			find = true;
	if (!find)
		npBase.push_back(np);
}

// ����� ������ ������ � ����  (-1 � ������ �� ��������� ������)
int CWorkExcel::CmpWithNpBase(const vector<CString>& sheetInfo)
{
	set <CString> prepos = { L"�", L"��", L"��", L"���", L"��", L"�����", L"��", L"���", L"��",  L"�",  L"�",  L"��",  L"�",  L"���" };	// ��������
	typedef struct {
		int numeric = 0;	// ���������� ����������� �������
		int all = 0;		// ���������� ���� �������
	} words;

	CString separate = L" ";
	int index = -1, result = -1, idxEmp = -1; // idxEmp - ������ ������� �������������� �����
	words res;

	for (size_t i = 0; i < npBase.size(); i++)
	{
		if (npBase[i].name.CompareNoCase(L"������"))
			idxEmp = (int)i;
		int pos = 0;
		words tmp;
		CString token = npBase[i].name.Tokenize(separate, pos);
		while (!token.IsEmpty() && prepos.find(token) == prepos.end())
		{
			tmp.all++;
			for (size_t j = 0; j < sheetInfo.size(); j++)
				if (sheetInfo[j].Find(token) != -1)
					tmp.numeric++;

			token = npBase[i].name.Tokenize(separate, pos);
		}
		if (res.numeric < tmp.numeric || (res.numeric == tmp.numeric && res.all > tmp.all))
		{
			res = tmp;
			index = (int)i;
		}
	}
	if (index != -1)
		result = npBase[index].number;

	return result;
}

// �������� ���������� ��� ������ ��� ������ ����� �������
vector<CString> CWorkExcel::GetSheetInfo(const Header& header)
{
	vector <CString> result;
	Cell res;
	FindCell(LINE_FIELD, res);
	if (res.row == -1 || res.column == -1)
		return result;

	for (long i = 1; i < header.adress[header.iRow] - 1; i++)
		for (long j = res.column + 2; j < last.column; j++)	// �������� �� ����������� ����� �� ��� ��������
			if (!CellValue(i, j).IsEmpty())
				result.push_back(CellValue(i, j));
	return result;
}

// ������� ������ ������
int CWorkExcel::ParseNp(string value, bool& flag)
{
	int result = -1;
	if (!value.empty())
	{
		char* end;
		errno = 0;
		result = strtol(value.c_str(), &end, 10);
		(*end != '\0' || errno != 0) ? flag = true : flag = flag;
	}
	else
		flag = true;
	return result;
}

// ������� ����� �����
CString CWorkExcel::ParseFrameName(string value)
{
	CString res(value.c_str());
	int pos1, pos2;
	// �������� �� �����������
	do
	{
		pos1 = res.Find(L"�� \"");
		pos2 = res.Find(L"��\"");
		if (pos1 != -1)
			res.Delete(pos1, 2);
		if (pos2 != -1)
			res.Delete(pos2, 2);
	} while (pos1 != -1 || pos2 != -1);

	// �������� ��������� ���� �������
	DeleteSymbol(res, L"\"");
	DeleteSymbol(res, L"�");
	DeleteSymbol(res, L"�");
	do // �������� ������ � �� ����������
	{
		pos1 = res.Find(L'(');
		pos2 = res.Find(L')');
		int a = res.GetLength();
		if (pos1 != -1 && pos2 != -1 && pos1 < pos2)
			res.Delete(pos1, pos2 - pos1 + 1);
	} while (pos1 < pos2);
	res.Trim();
	return res;
}

// �������� ������� �� ������
void CWorkExcel::DeleteSymbol(CString& field, const CString& sym)
{
	int pos;
	do
	{
		pos = field.Find(sym);
		if (pos != -1)
			field.Delete(pos);
	} while (pos != -1);
}
#pragma endregion 
*/
#pragma endregion

#pragma region ReadParameters
// ������ ������ �����
intData CReaderExcel::GetNumWord(CString numeric)
{
	intData result;
	vector <int> vec = { -1 };
	int posDot = numeric.Find(L',');
	result.field = numeric;

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
	return result;
}

// ������ ������������, ������������� � ���
void CReaderExcel::GetMinMaxCsr(signalData& signal, const long& row)
{
	long column = header.adress[iMin];
	signal.min.field = work.CellValue(row, column);
	signal.min.value = GetDouble(signal.min.field, signal.min.flag);

	column = header.adress[iMax];
	signal.max.field = work.CellValue(row, column);
	signal.max.value = GetDouble(signal.max.field, signal.max.flag);

	column = header.adress[iCSR];
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

// ������ ������������ ��������
intData CReaderExcel::GetBits(CString bits, const int& size)
{
	intData result;
	int posDot = bits.Find(L',');
	vector <int> vec;
	result.field = bits;

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
	result.value = vec;
	return result;
}

// �������������� ������� ��� �������� ��������
vector <int> CReaderExcel::StepBits(CString numeric, bool& flag)
{
	vector <int> result = { -1, -1 };
	int indxDel = numeric.Find(L'.') == -1 ? numeric.Find(L'�') : numeric.Find(L'.');	// ����� ������� ������������

	if (indxDel == -1)	// ������������ ���, ������������ ���� ������
		result[0] = GetInt(numeric, flag);
	else
	{
		int posDot = numeric.ReverseFind(L'.');
		if (posDot != -1)
			numeric.Delete(indxDel, posDot - indxDel);

		CString numeric2 = numeric;
		numeric.Delete(indxDel, numeric.GetLength());	// ������ �����
		numeric2.Delete(0, indxDel + 1);	// ������ �����
		numeric2.Trim();	numeric.Trim();

		result[0] = GetInt(numeric, flag);
		result[1] = GetInt(numeric2, flag);
	}
	return result;
}

// ������ ����������
CString CReaderExcel::GetComment(long row, const int& size, bool& flag)
{
	int merge = work.GetMerge(row, header.adress[iComment]);
	CString result;

	size > merge ? merge = size : merge = merge;
	for (int i = 0; i < merge; i++)
	{
		CString tmp = work.CellValue(row + i, header.adress[iComment]);
		if (!tmp.IsEmpty())
		{
			for (size_t j = 0; j < sign.size(); j++)
				tmp.Find(sign[j]) != -1 ? flag = true : flag = flag;
			result.IsEmpty() ? result = tmp : result += L'\n' + tmp;
		}
	}
	return result;
}
#pragma endregion

#pragma region Converters
// ��������� int ��������
int CReaderExcel::GetInt(CString field, bool& flag)
{
	int result = -1;
	CStringA tmp(field);
	char* buffer = new char[tmp.GetLength() + 1];
	strcpy_s(buffer, tmp.GetLength() + 1, tmp);
	result = GetInt(buffer, flag);
	delete[] buffer;
	return result;
}

// ���������� ��� string (� ������ �������, �������� �������� -1)
int CReaderExcel::GetInt(string value, bool& flag)
{
	int result = -1;
	if (!value.empty())
	{
		char* end;
		errno = 0;
		result = strtol(value.c_str(), &end, 10);
		(*end != '\0' || errno != 0) ? flag = true : flag = flag;
	}
	else
		flag = true;
	return result;
}

// ��������� double ��������
double CReaderExcel::GetDouble(CString field, bool& flag)
{
	double result = DBL_MIN;
	field.Replace(L',', L'.');
	CStringA tmp(field);
	char* buffer = new char[tmp.GetLength() + 1];
	strcpy_s(buffer, tmp.GetLength() + 1, tmp);

	if (!field.IsEmpty())
	{
		char* end;
		errno = 0;
		result = strtod(buffer, &end);
		(*end != '\0' || errno != 0) ? flag = true : flag = flag;
	}
	else
		flag = true;

	delete[] buffer;
	return result;
}
#pragma endregion

#pragma region Checkers
// �������� ������ �� ���������
bool CReaderExcel::IsTitle(const long& row)
{
	bool result = false;
	CString numeric = work.CellValue(row, 1);
	if (!numeric.IsEmpty())
		GetInt(numeric, result);
	return result;
}

// �������� ������ �� �������
bool CReaderExcel::IsEmpty(const long& row)
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

// �������� ������ �� ����������
bool CReaderExcel::IsRemark(const long& row)
{
	bool result = false;
	for (long i = 1; i < header.size; i++)
	{
		long column = header.adress[i];
		if (header.adress[i] != -1)
		{
			CString tmp = work.CellValue(row, column);
			for (size_t j = 0; j < remark.size(); j++)
				result = tmp.Find(remark[j]) > -1 ? true : result;
		}
	}
	return result;
}

// �������� ����� ��������
bool CReaderExcel::IsArinc()
{
	Cell cell;		// ������
	bool result = false;
	if (work.FindCell(line, cell))
		work.CellValue(cell.row, cell.column + 1).Find(ARINC) != -1 ? result = true : result = false;
	return result;
}

// �������� ���������� �����
bool CReaderExcel::ExtensionChecker(const CString& path)
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