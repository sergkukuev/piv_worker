#include "stdafx.h"
#include "ReaderExcel.h"

// �����������
CReaderExcel::CReaderExcel()
{
	// ��������� ������ ����������
	header.list.push_back(list<CString>{ _T("� ����"), _T("� �/�"), _T("� ����� � ���������") });						// ��� ������ �����
	header.list.push_back(list<CString>{ _T("������������ ���������"), _T("������������ �������") });					// ��� ������������ ���������
	header.list.push_back(list<CString>{ _T("����������� �������"), _T("�������� ����������� ��������� / �������") });	// ��� �������
	header.list.push_back(list<CString>{ _T("�����������"), _T("������� ���������") });									// ��� ������� �����������
	header.list.push_back(list<CString>{ _T("����������� ��������"), _T("�����. ��������"), _T("������� ��������") });	// ����������� ��������
	header.list.push_back(list<CString>{ _T("������������ ��������"), _T("������. ��������"), _T("�������� ��������") });// ������������ ��������
	header.list.push_back(list<CString>{ _T("���� �������� �������") });												// ��� ���
	header.list.push_back(list<CString>{ _T("������������ �������"), _T("���������-��� �������") });					// ��� ������������ ��������
	header.list.push_back(list<CString>{ _T("����������") });															// ����������� (���������)

	// ���������� ���� ����� ���������� excel ������
	extension.push_back(_T("xlsx"));	// ������� �������� ������
	extension.push_back(_T("xlsm"));
	extension.push_back(_T("xlsb"));
	extension.push_back(_T("xltx"));
	extension.push_back(_T("xltm"));
	extension.push_back(_T("xls"));		// �������� ������ �� 2003 ����
	extension.push_back(_T("xml"));
	extension.push_back(_T("xlam"));
	extension.push_back(_T("xla"));
	extension.push_back(_T("xlw"));
	extension.push_back(_T("xlr"));
}

// ����������
CReaderExcel::~CReaderExcel()
{
	header.list.clear();
	extension.clear();
}

// ������ ����� �����
bookData CReaderExcel::getBook(CString pathToExcel)
{
	if (!checkExtension(pathToExcel))
		throw BadTypeException();

	CWorkExcel work;
	bookData book;

	work.openWorkBook(pathToExcel);	// �������� �����

	if (work.getCountBooks() == 0)
		throw NoBooksException();

	long clBook = (long)1;
	work.setActivBook(clBook);

	book.name = work.getNameBook();
	book.sheets.resize(work.getCountSheets());

	book.sheets = getSheets(work);

	work.closeWorkBooks();	// �������� �����

	return book;
}

// ������ ����
vector <bookData> CReaderExcel::getBooks(vector <CString> pathToExcel)
{
	for (size_t i = 0; i < pathToExcel.size(); i++)
		if (!checkExtension(pathToExcel[i]))
			throw BadTypeException();

	CWorkExcel work;
	vector <bookData> books;

	// �������� ����
	for (size_t i = 0; i < pathToExcel.size(); i++)
		work.openWorkBook(pathToExcel[i]);

	if (work.getCountBooks() == 0)
		throw NoBooksException();

	books.resize(work.getCountBooks());

	for (int i = 1; i < books.size() + 1; i++)
	{
		long clBooks = static_cast<long>(i);

		work.setActivBook(clBooks);
		books[i - 1].name = work.getNameBook();
		books[i - 1].sheets.resize(work.getCountSheets());

		books[i - 1].sheets = getSheets(work);
	}

	work.closeWorkBooks();	// �������� ����

	return books;
}

// ������ ������
vector <sheetData> CReaderExcel::getSheets(CWorkExcel& work)
{
	vector <sheetData> sheets;
	sheets.resize(work.getCountSheets());

	for (int i = 1; i < work.getCountSheets() + 1; i++)
	{
		long iSheet = static_cast<long> (i);

		work.setActivSheet(iSheet);
		sheets[i - 1].name = work.getNameSheet();
		sheets[i - 1].iFieldNP = -1;
		sheets[i - 1].iNumPK = -1;

		if (!findHeader(work))		// ����� ���������� �� �����
			throw NotAllHeaderException();

		sheets[i - 1].bError = true;

		sheets[i - 1].iNumPK = getNumPK(work);

		header.adress[header.iRow]++;

		sheets[i - 1].signals = getSignals(work);
	}

	return sheets;
}

// ����� ������ ����� (� ��������� ������ ����� ����� -1)
int CReaderExcel::getNumPK(CWorkExcel& work)
{
	// ������� � ������� ����������
	Cell cell;
	cell.row = static_cast<long> (header.adress[header.iRow] - 1);
	cell.column = static_cast<long> (header.adress[header.iComment]);

	int iNumPK = -1;	// ������ ��������

	if (work.getCellValue(cell) != "")
	{
		CString sNumPK = work.getCellValue(cell);
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
list <signalData> CReaderExcel::getSignals(CWorkExcel& work)
{
	list <signalData> signals;
	bool bRemark = false;	// �������� �� ������ �����������
	int cEmpty = 0;			// ������� ������ �����

	do
	{
		signalData signal;
		Cell cell;

		cell.row = static_cast<long> (header.adress[header.iRow]);
		
		// ������ ����������:
		// ������������ �������� � ����������� �������
		cell.column = static_cast<long> (header.adress[header.iName]);
		signal.sTitleParamField[0] = getCell(work, cell);
		long cMergeName = work.getMergeCount(cell);

		cell.column = static_cast<long> (header.adress[header.iSignal]);
		signal.sTitleParamField[1] = getCell(work, cell);

		signal.bTitleParamField = true;

		// ������ ������ �����
		cell.column = static_cast<long> (header.adress[header.iNumWord]);
		signal.sNumWordField = getCell(work, cell, cMergeName);
		
		signal.b2NumWordField = true;
		signal.bNumWordField = true;

		// ������ �����������, min, max � csr
		cell.column = static_cast<long> (header.adress[header.iDimension]);
		signal.sDimensionField = getCell(work, cell, cMergeName);

		cell.column = static_cast<long> (header.adress[header.iMin]);
		signal.sMinMaxCsrValField[0] = getCell(work, cell, cMergeName);
		signal.bMinValField = true;

		cell.column = static_cast<long> (header.adress[header.iMax]);
		signal.sMinMaxCsrValField[1] = getCell(work, cell, cMergeName);
		signal.bMaxValField = true;

		cell.column = static_cast<long> (header.adress[header.iCSR]);
		signal.sMinMaxCsrValField[2] = getCell(work, cell, cMergeName);
		signal.bCsrValField = true;

		// ������ ��������
		cell.column = static_cast<long> (header.adress[header.iBits]);
		signal.sBitField = getCell(work, cell, cMergeName);
		signal.bBitField = true;
		signal.b2BitField = true;
		signal.iBitSigns = 0;
		signal.bBitSigns = true;

		// ������ ������������
		cell.column = static_cast<long> (header.adress[header.iComment]);
		signal.sCommentField = getCell(work, cell, cMergeName);
		signal.bCommentField = false;

		bool bEmpty = IsEmpty(work, cell.row);
		bRemark = IsRemark(work, cell.row);

		if (bEmpty)
			cEmpty++;	// ������������� �������� ������ �����
		else
			cEmpty = 0;	// ����� ��������

		// ���������� �������
		if (!bEmpty && !bRemark)
			signals.push_back(signal);

		header.adress[header.iRow] += cMergeName;

	} while (cEmpty < 5 && !bRemark);

	return signals;
}

// �������� ������ �� �������
bool CReaderExcel::IsEmpty(CWorkExcel& work, long row)
{
	bool result = true;
	Cell cell;

	cell.row = row;
	for (long i = 1; i < header.size; i++)
	{
		cell.column = static_cast<long> (header.adress[i]);
		if (work.getCellValue(cell) != "")
			result = false;
	}

	return result;
}

// �������� �� ������ �����������
bool CReaderExcel::IsRemark(CWorkExcel& work, long row)
{
	bool result = true;
	Cell cell;

	cell.row = row;
	for (long i = 1; i < header.size; i++)
	{
		cell.column = static_cast<long> (header.adress[i]);
		result = (work.getCellValue(cell).Find(_T("����������:")) > -1 ||
			work.getCellValue(cell).Find(_T("����������:")) > -1) ? true : false;
	}

	return result;
}

// ������ ������
CString CReaderExcel::getCell(CWorkExcel& work, Cell cell, long cName)
{
	CString result;
	long size = work.getMergeCount(cell);

	// ���� ������ ������ ������������ ������, ��� �������, ������ ������ ��� ������ � ���������� ������ ����� ������������
	if (cName > size)	
	{
		long start = work.getStartMerge(cell);
		for (long i = start; i < start + cName; i++)
		{
			cell.row = i;
			CString tmp = work.getCellValue(cell);

			if (i != (start + size - 1) || (!result.IsEmpty() && !tmp.IsEmpty()))
				result += _T(", ");

			if (!tmp.IsEmpty())
				result += work.getCellValue(cell);
		}
	}
	else
	{
		cell.row = work.getStartMerge(cell);
		result = work.getCellValue(cell);
	}

	return result;
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

// �������� ����������
bool CReaderExcel::checkExtension(CString path)
{
	int posDot = path.Find(_T("."), path.GetLength() - 5);	// ����� �������

	CString pathExtension = path.Mid(posDot + 1);	// ��������� ���������� �� ����

	for (size_t i = 0; i < extension.size(); i++)
		if (pathExtension.CompareNoCase(extension[i]) == 0)
			return true;

	return false;
}

// ����� �������� ����������
bool CReaderExcel::findHeader(CWorkExcel& work)
{
	for (size_t i = 0; i < header.list.size(); i++)
	{
		list<CString>::iterator it = header.list[i].begin();
		bool bFind = false;
		Cell cell;

		for (it; !bFind; it++)	// ����� ���� �� ������ �������� ��������� �� ������
		{
			if ((it == header.list[i].end()) && !bFind)	// ������ �� ����� � ������ ����������
				return false;

			bFind = work.findOneDateCells(*it, cell);
		}
		header.adress[header.iRow] = cell.row;
		header.adress[i + 1] = cell.column;
	}

	return true;
}
