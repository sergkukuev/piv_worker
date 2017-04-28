#include "stdafx.h"
#include "ReaderExcel.h"

// �����������
CReaderExcel::CReaderExcel()
{
}

// ����������
CReaderExcel::~CReaderExcel()
{
}

// ����������� � ����������
CReaderExcel::CReaderExcel(vector <CString>& pathToExcel)
{
	path = pathToExcel;

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

// ������ ����
vector <bookData> CReaderExcel::getBooks()
{
	vector <bookData> books;
	books.resize(work.getCountBooks());

	for (int i = 1; i < books.size() + 1; i++)
	{
		long clBooks = static_cast<long>(i);

		work.setActivBook(clBooks);
		books[i - 1].nameBook = work.getNameBook();
		books[i - 1].sheets.resize(work.getCountSheets());

		// ������� �����
	}

	return books;
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
void CReaderExcel::setHeader(int index, adrCell& cell)
{
	if (index == 1)
	{
		iHeader.iNumber = cell.column;
		iHeader.iRows = cell.row;
	}
	else if (index == 2)
		iHeader.iName = cell.column;
	else if (index == 3)
		iHeader.iNameSignal = cell.column;
	else if (index == 4)
		iHeader.iDimension = cell.column;
	else if (index == 5)
		iHeader.iMin = cell.column;
	else if (index == 6)
		iHeader.iMax = cell.column;
	else if (index == 7)
		iHeader.iCSR = cell.column;
	else if (index == 8)
		iHeader.iBits = cell.column;
	else if (index = 9)
		iHeader.iComments = cell.column;
}
