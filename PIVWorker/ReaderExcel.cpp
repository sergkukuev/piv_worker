#include "stdafx.h"
#include "ReaderExcel.h"

// �����������
CReaderExcel::CReaderExcel()
{
	// ��������� ������ ����������
	HeaderTable.push_back(list<CString>{ _T("� ����"), _T("� �/�"), _T("� ����� � ���������") });
	HeaderTable.push_back(list<CString>{ _T("������������ ���������"), _T("������������ �������") });
	HeaderTable.push_back(list<CString>{ _T("����������� �������"), _T("�������� ����������� ��������� / �������") });
	HeaderTable.push_back(list<CString>{ _T("�����������"), _T("������� ���������") });
	HeaderTable.push_back(list<CString>{ _T("����������� ��������"), _T("�����. ��������"), _T("������� ��������") });
	HeaderTable.push_back(list<CString>{ _T("������������ ��������"), _T("������. ��������"), _T("�������� ��������") });
	HeaderTable.push_back(list<CString>{ _T("���� �������� �������") });
	HeaderTable.push_back(list<CString>{ _T("������������ �������"), _T("���������-��� �������") });
	HeaderTable.push_back(list<CString>{ _T("����������") });
}

// ����������
CReaderExcel::~CReaderExcel()
{
}

// ����������� � ����������
CReaderExcel::CReaderExcel(vector <CString> pathToExcel)
{
	vector <CString> path = pathToExcel;

	// �������� ����
	for (size_t i = 0; i < path.size(); i++)
		work.openWorkBook(path[i]);

	// ��������� ������ ����������
	HeaderTable.push_back(list<CString>{ _T("� ����"), _T("� �/�"), _T("� ����� � ���������") });
	HeaderTable.push_back(list<CString>{ _T("������������ ���������"), _T("������������ �������") });
	HeaderTable.push_back(list<CString>{ _T("����������� �������"), _T("�������� ����������� ��������� / �������") });
	HeaderTable.push_back(list<CString>{ _T("�����������"), _T("������� ���������") });
	HeaderTable.push_back(list<CString>{ _T("����������� ��������"), _T("�����. ��������"), _T("������� ��������") });
	HeaderTable.push_back(list<CString>{ _T("������������ ��������"), _T("������. ��������"), _T("�������� ��������") });
	HeaderTable.push_back(list<CString>{ _T("���� �������� �������") });
	HeaderTable.push_back(list<CString>{ _T("������������ �������"), _T("���������-��� �������") });
	HeaderTable.push_back(list<CString>{ _T("����������") });
}

// �������� ���������� ����
int CReaderExcel::getSize()
{
	return work.getCountBooks();
}

// ������ ����� �����
bool CReaderExcel::getBook(CString pathToExcel, bookData& book)
{
	work.openWorkBook(pathToExcel);

	if (work.getCountBooks() != 1)
		return false;

	long nBook = 1;

	work.setActivBook(nBook);
	book.nameBook = work.getNameBook();
	book.sheets.resize(work.getCountSheets());

	book.sheets = getSheets();

	return true;
}

// ������ ����
bool CReaderExcel::getBooks(vector <bookData>& books)
{
	if (work.getCountBooks() == 0)
		return false;

	books.resize(work.getCountBooks());

	for (int i = 1; i < books.size() + 1; i++)
	{
		long clBooks = static_cast<long>(i);

		work.setActivBook(clBooks);
		books[i - 1].nameBook = work.getNameBook();
		books[i - 1].sheets.resize(work.getCountSheets());

		books[i - 1].sheets = getSheets();
	}

	return true;
}

// ������ ������
vector <sheetData> CReaderExcel::getSheets()
{
	vector <sheetData> sheets;
	sheets.resize(work.getCountSheets());

	for (int i = 1; i < work.getCountSheets() + 1; i++)
	{
		long iSheet = static_cast<long> (i);

		work.setActivSheet(iSheet);
		sheets[i - 1].nameSheet = work.getNameSheet();
		sheets[i - 1].iCommentFieldNP = -1;
		sheets[i - 1].iNumPodKadra = -1;

		if (bHeader)	// �������� ������� ���������� �� �����
		{
			sheets[i - 1].bErrorExcelSheet = true;
			sheets[i - 1].bErrorSheet = true;
		}
		else
		{
			sheets[i - 1].bErrorExcelSheet = false;
			sheets[i - 1].bErrorSheet = false;
		}

		bHeader = findHeader();

		sheets[i - 1].iNumPodKadra = getNumPK();

		iHeader.iRows++;

		sheets[i - 1].signals = getSignals();
	}

	return sheets;
}

// ����� ������ ����� (� ��������� ������ ����� ����� -1)
int CReaderExcel::getNumPK()
{
	long lRows = static_cast<long> (iHeader.iRows - 1);
	long lColumns = static_cast<long> (iHeader.iComments);
	int iNumPK = -1;

	if (work.getCellValue(lRows, lColumns) != "")
	{
		CString sNumPK = work.getCellValue(lRows, lColumns);
		int i = 0;

		while (i != -1)
		{
			i = sNumPK.Find(_T("�"), i == 0 ? i : i + 1);
			i = i == sNumPK.GetLength() ? -1 : i;
			iNumPK = i == -1 ? iNumPK : i;
		}

		if (iNumPK != -1)
			sNumPK.Delete(0, iNumPK + 1);

		sNumPK = sNumPK.Trim();

		regex regStr("[[:digit:]]+");
		string stdStr = convertString(sNumPK);

		if (regex_match(stdStr, regStr))
			iNumPK = atoi(stdStr.c_str());
	}

	return iNumPK;
}

// ������ ���������� �� �����
list <signalData> CReaderExcel::getSignals()
{
	list <signalData> signals;
	bool bRemark = false;	// ����������
	int cEmpty = 0;			// ���������� ������ �����

	do
	{
		signalData signal;
		long lRow = static_cast<long> (iHeader.iRows);

		// ������ ����������
		long lColumn = static_cast<long> (iHeader.iNumber);
		signal.sNumWordField = work.getCellValue(lRow, lColumn);
		signal.bNumWordField = true;
		signal.b2NumWordField = false;
		signal.bRepitNumWordField = false;

		lColumn = static_cast<long> (iHeader.iName);
		signal.sTitleParamField[0] = work.getCellValue(lRow, lColumn);

		lColumn = static_cast<long> (iHeader.iNameSignal);
		signal.sTitleParamField[1] = work.getCellValue(lRow, lColumn);
		signal.bTitleParamField = true;

		lColumn = static_cast<long> (iHeader.iDimension);
		signal.sDimensionField = work.getCellValue(lRow, lColumn);

		lColumn = static_cast<long> (iHeader.iMin);
		signal.sMinMaxCsrValField[0] = work.getCellValue(lRow, lColumn);

		lColumn = static_cast<long> (iHeader.iMax);
		signal.sMinMaxCsrValField[1] = work.getCellValue(lRow, lColumn);

		lColumn = static_cast<long> (iHeader.iCSR);
		signal.sMinMaxCsrValField[2] = work.getCellValue(lRow, lColumn);
		signal.bMinValField = true;
		signal.bMaxValField = true;
		signal.bCsrValField = true;

		lColumn = static_cast<long> (iHeader.iBits);
		signal.sBitField = work.getCellValue(lRow, lColumn);
		signal.bBitField = true;
		signal.b2BitField = false;

		signal.iBitSigns = 0;
		signal.bBitSigns = true;

		// ������ ������������
		lColumn = static_cast<long> (iHeader.iComments);
		signal.bCommentField = false;

		if (work.getMergeCount(lRow, lColumn) != 1)	// ���� ����������� �� ��� ����� �������
		{
			list <signalData>::iterator itS = signals.end();
			itS--;
			signal.sCommentField.push_back(*itS->sCommentField.begin());
		}
		else
			signal.sCommentField.push_back(work.getCellValue(lRow, lColumn));

		bool bEmpty = true;		// ������ ������
		bool bOnlyOne = false;	// ���� �� ���� �������� ������
		for (int j = 1; j < 9; j++)
		{
			lColumn = static_cast <long>(j);
			if (work.getCellValue(lRow, lColumn) == "")
				bOnlyOne = true;
			else
				bEmpty = false;
		}

		if (bEmpty)
		{
			list <CString>::iterator itC = signal.sCommentField.begin();

			if (*itC != "")	// �������� �� ������ ������ ������������
			{
				list <signalData>::iterator itS = signals.end();
				itS--;
				itS->sCommentField.push_back(*itC);
			}
			else
				cEmpty++;
		}
		else
		{
			cEmpty = 0;

			// �������� �� ������ ����������� ��� ����������
			for (int j = 1; j < 9; j++)
			{
				lColumn = static_cast <long> (j);
				bRemark = (work.getCellValue(lRow, lColumn).Find(_T("����������:")) > -1 ||
					work.getCellValue(lRow, lColumn).Find(_T("����������:")) > -1) ? true : false;
			}
		}

		// ���������� �������
		if (!bEmpty && !bRemark)
			signals.push_back(signal);

		iHeader.iRows++;

	} while (cEmpty < 5 && !bRemark);

	return signals;
}

// ������� �������������� CString � string
string CReaderExcel::convertString(CString cStr)
{
	int bytes = WideCharToMultiByte(CP_ACP, 0, cStr.GetBuffer(), -1, 0, 0, NULL, NULL);
	char *buffer = new char[bytes];

	WideCharToMultiByte(CP_ACP, 0, cStr.GetBuffer(), -1, buffer, bytes, NULL, NULL);

	string result(buffer);

	delete buffer;
	buffer = nullptr;

	return result;
}

// ����� �������� ����������
bool CReaderExcel::findHeader()
{
	vector<list<CString>>::iterator itV = HeaderTable.begin();
	adrCell cell;		// ����� ������ ���������
	bool result = true;	// ������� �� ���������

	for (int i = 1; itV != HeaderTable.end(); itV++, i++)
	{
		list<CString>::iterator itL = (*itV).begin();
		bool bEnd = true;

		while (itL != (*itV).end() || bEnd)
		{
			work.findOneDateCells(*itL, cell.row, cell.column);	// ����� ������ ������ � ������ ���������

			if (cell.row == -1)
			{
				if (itL == (*itV).end())
					result = bEnd = false;
			}
			else
			{
				setHeader(i, cell);
				bEnd = false;
			}
			itL++;
		}
	}

	return result;
}

// ��������� ���������
void CReaderExcel::setHeader(int index, adrCell cell)
{
	if (index == INDEX_NUM)
	{
		iHeader.iNumber = cell.column;
		iHeader.iRows = cell.row;
	}
	else if (index == INDEX_NAME)
		iHeader.iName = cell.column;
	else if (index == INDEX_SIGNAL)
		iHeader.iNameSignal = cell.column;
	else if (index == INDEX_DIMENSION)
		iHeader.iDimension = cell.column;
	else if (index == INDEX_MIN)
		iHeader.iMin = cell.column;
	else if (index == INDEX_MAX)
		iHeader.iMax = cell.column;
	else if (index == INDEX_CSR)
		iHeader.iCSR = cell.column;
	else if (index == INDEX_BITS)
		iHeader.iBits = cell.column;
	else if (index = INDEX_COMMENTS)
		iHeader.iComments = cell.column;
}
