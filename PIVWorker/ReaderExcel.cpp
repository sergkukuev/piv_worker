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
