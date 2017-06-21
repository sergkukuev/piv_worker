#include "stdafx.h"
#include "ReaderExcel.h"
#include <ctime>

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
	unsigned int s = clock();
	if (!checkExtension(pathToExcel))
		throw BadTypeException();

	CWorkExcel work;
	bookData book;

	work.openBook(pathToExcel);
	book.name = work.bookName();

	book.sheets = getSheets(work);
	
	unsigned int e = clock();
	unsigned int res = e - s;
	
	return book;
}

// ������ ������
vector <sheetData> CReaderExcel::getSheets(CWorkExcel& work)
{
	vector <sheetData> sheets;
	sheets.resize(work.countSheet());

	for (long i = 1; i < work.countSheet(); i++)
	{
		long iSheet = static_cast<long> (i);

		work.setActiveSheet(iSheet);
		sheets[i - 1].name = work.sheetName();
		sheets[i - 1].iFieldNP = -1;
		sheets[i - 1].iNumPK = -1;

		//if (!findHeader(work))		// ����� ���������� �� �����
			//throw NotAllHeaderException();
		header.adress[header.iRow] = 8;
		header.adress[1] = 1;
		header.adress[2] = 2;
		header.adress[3] = 3;
		header.adress[4] = 4;
		header.adress[5] = 5;
		header.adress[6] = 6;
		header.adress[7] = 7;
		header.adress[8] = 8;
		header.adress[9] = 9;

		sheets[i - 1].bError = false;

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
	
	long row = static_cast<long> (header.adress[header.iRow] - 1);
	long column = static_cast<long> (header.adress[header.iComment]);

	int iNumPK = -1;	// ������ ��������

	CString res = work.cellValue(row, column);
	if (res != "")
	{
		CString sNumPK = work.cellValue(row, column);
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
		iNumPK = _wtoi(sNumPK);
	}

	return iNumPK;
}

// ������ ���������� �� �����
list <signalData> CReaderExcel::getSignals(CWorkExcel& work)
{
	list <signalData> signals;
	bool bRemark = false;	// �������� �� ������ �����������
	int cEmpty = 0;			// ������� ������ �����

	for (int i = header.adress[header.iRow]; i < work.countSignal() + 1; i++)
	{
		signalData signal;
		long row, column;

		row = static_cast<long>(i);
		
		// ������ ����������:
		// ������������ ��������� � ����������� �������
		column = static_cast<long> (header.adress[header.iName]);
		signal.sTitleParamField[0] = work.cellValue(row, column);

		column = static_cast<long> (header.adress[header.iSignal]);
		signal.sTitleParamField[1] = work.cellValue(row, column);
		signal.bTitleParamField = false;

		// ������ ������ �����
		column = static_cast<long> (header.adress[header.iNumWord]);
		signal.sNumWordField = work.cellValue(row, column);
		
		signal.b2NumWordField = false;
		signal.bNumWordField = false;

		// ������ �����������, min, max � csr
		column = static_cast<long> (header.adress[header.iDimension]);
		signal.sDimensionField = work.cellValue(row, column);

		column = static_cast<long> (header.adress[header.iMin]);
		signal.sMinMaxCsrValField[0] = work.cellValue(row, column);
		signal.bMinValField = false;

		column = static_cast<long> (header.adress[header.iMax]);
		signal.sMinMaxCsrValField[1] = work.cellValue(row, column);
		signal.bMaxValField = false;

		column = static_cast<long> (header.adress[header.iCSR]);
		signal.sMinMaxCsrValField[2] = work.cellValue(row, column);
		signal.bCsrValField = false;

		// ������ ��������
		column = static_cast<long> (header.adress[header.iBits]);
		signal.sBitField = work.cellValue(row, column);
		signal.bBitField = false;
		signal.b2BitField = false;
		signal.iBitSigns = 0;
		signal.bBitSigns = false;

		// ������ ������������
		column = static_cast<long> (header.adress[header.iComment]);
		signal.sCommentField = work.cellValue(row, column);
		signal.bCommentField = false;

		bool bEmpty = IsEmpty(work, row);
		bRemark = IsRemark(work, row);

		if (bEmpty)
			cEmpty++;	// ������������� �������� ������ �����
		else
			cEmpty = 0;	// ����� ��������

		// ���������� �������
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
	}

	return signals;
}

// �������� ������ �� �������
bool CReaderExcel::IsEmpty(CWorkExcel& work, long row)
{
	bool result = true;

	for (long i = 1; i < header.size; i++)
	{
		long column = static_cast<long> (header.adress[i]);
		CString res = work.cellValue(row, column);
		if (res != "")
			result = false;
	}

	return result;
}

// �������� �� ������ �����������
bool CReaderExcel::IsRemark(CWorkExcel& work, long row)
{
	bool result = false;

	for (long i = 1; i < header.size; i++)
	{
		long column = static_cast<long> (header.adress[i]);
		CString res = work.cellValue(row, column);
		result = (res.Find(_T("����������:")) > -1 ||
			res.Find(_T("����������:")) > -1) ? true : result;
	}

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
/*bool CReaderExcel::findHeader(CWorkExcel& work)
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

			bFind = work.findHeader(*it, cell);
		}
		header.adress[header.iRow] = cell.row;
		header.adress[i + 1] = cell.column;
	}

	return true;
}*/
