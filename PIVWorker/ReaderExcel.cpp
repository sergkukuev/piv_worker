#include "stdafx.h"
#include "ReaderExcel.h"

// �����������
CReaderExcel::CReaderExcel() {
	// ��������� ������ ����������
	header.list.push_back(vector<CString>{ _T("� ����"), _T("� �����"), _T("� �/�"), _T("� ����� � ���������") });			// ��� ������ �����
	header.list.push_back(vector<CString>{ _T("������������ ���������"), _T("������������ �������") });						// ��� ������������ ���������
	header.list.push_back(vector<CString>{ _T("����������� �������"), _T("�������� ����������� ��������� / �������") });	// ��� �������
	header.list.push_back(vector<CString>{ _T("�����������"), _T("������� ���������") });									// ��� ������� �����������
	header.list.push_back(vector<CString>{ _T("����������� ��������"), _T("�����. ��������"), _T("������� ��������") });	// ����������� ��������
	header.list.push_back(vector<CString>{ _T("������������ ��������"), _T("������. ��������"), _T("�������� ��������") });	// ������������ ��������
	header.list.push_back(vector<CString>{ _T("���� �������� �������") });													// ��� ���
	header.list.push_back(vector<CString>{ _T("������������ �������"), _T("���������-��� �������") });						// ��� ������������ ��������
	header.list.push_back(vector<CString>{ _T("����������") });																// ����������� (���������)

	// ���������� ���� ����� ���������� excel ������
	extension.push_back(_T("xlsx"));	// ������� �������� ������
	extension.push_back(_T("xls"));		// �������� ������ �� 2003 ����
	/*extension.push_back(_T("xlsm"));
	extension.push_back(_T("xlsb"));
	extension.push_back(_T("xltx"));
	extension.push_back(_T("xltm"));
	extension.push_back(_T("xml"));
	extension.push_back(_T("xlam"));
	extension.push_back(_T("xla"));
	extension.push_back(_T("xlw"));
	extension.push_back(_T("xlr"));*/
}

// ����������
CReaderExcel::~CReaderExcel() {
	header.list.clear();
	header.list.shrink_to_fit();
	extension.clear();
	extension.shrink_to_fit();
}

// ������ ����� �����
bookData CReaderExcel::getBook(CString pathToExcel) {
	bookData book;

	try {
		if (!checkExtension(pathToExcel))
			throw BadTypeException();

		CWorkExcel work;
		work.openBook(pathToExcel);
		book.name = work.bookName();
		book.sheets = getSheets(work);
	}
	catch (COleDispatchException& exc) {
		TCHAR msg[255];
		CString msgFormated;

		exc.GetErrorMessage(msg, 255);
		msgFormated += msg;

		AccessExcelException myExc;
		myExc.setOleError(msgFormated);
		throw myExc;
	}
	
	return book;
}

// ������ ������
vector <sheetData> CReaderExcel::getSheets(CWorkExcel& work) {
	vector <sheetData> sheets;
	sheets.resize(work.countSheets());

	try {
		for (long i = 1; i < work.countSheets(); i++) {
			work.openSheet((long)i);
			sheets[i - 1].name = work.sheetName();
			sheets[i - 1].iFieldNP = -1;
			sheets[i - 1].iNumPK = -1;

			// ����� ���������� �� �����
			if (!work.findHeader(header)) {
				NotAllHeaderException exc;
				exc.setParam(sheets[i - 1].name, work.bookName());
				throw exc;
			}

			sheets[i - 1].bError = false;

			sheets[i - 1].iNumPK = getNumPK(work);

			header.adress[header.iRow]++;

			sheets[i - 1].signals = getSignals(work);
		}
	}
	catch (COleException& exc) {
		TCHAR msg[255];
		CString msgFormated;

		exc.GetErrorMessage(msg, 255);
		msgFormated += msg;

		ReadBookException myExc;
		myExc.setParam(msgFormated, work.bookName());
		throw myExc;
	}

	return sheets;
}

// ����� ������ ����� (� ��������� ������ ����� ����� -1)
int CReaderExcel::getNumPK(CWorkExcel& work) {
	// ������� � ������� ����������
	long row = header.adress[header.iRow] - 1;
	long column = header.adress[header.iComment];

	int iNumPK = -1;	// ������ ��������

	CString sNumPK = work.cellValue(row, column);
	if (!sNumPK.IsEmpty()) {
		int i = 0;

		while (i != -1) {
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
list <signalData> CReaderExcel::getSignals(CWorkExcel& work) {
	list <signalData> signals;

	for (long i = header.adress[header.iRow]; i < work.countRows() + 1; i++) {
		signalData signal;
		long row, column;

		row = i;
		
		// ������ ����������:
		// ������������ ��������� � ����������� �������
		column = header.adress[header.iName];
		signal.sTitleParamField[0] = work.cellValue(row, column);

		column = header.adress[header.iSignal];
		signal.sTitleParamField[1] = work.cellValue(row, column);
		signal.bTitleParamField = false;

		// ������ ������ �����
		column = header.adress[header.iNumWord];
		signal.sNumWordField = work.cellValue(row, column);
		
		signal.b2NumWordField = false;
		signal.bNumWordField = false;

		// ������ �����������, min, max � csr
		column = header.adress[header.iDimension];
		signal.sDimensionField = work.cellValue(row, column);

		column = header.adress[header.iMin];
		signal.sMinMaxCsrValField[0] = work.cellValue(row, column);
		signal.bMinValField = false;

		column = header.adress[header.iMax];
		signal.sMinMaxCsrValField[1] = work.cellValue(row, column);
		signal.bMaxValField = false;

		column = header.adress[header.iCSR];
		signal.sMinMaxCsrValField[2] = work.cellValue(row, column);
		signal.bCsrValField = false;

		// ������ ��������
		column = header.adress[header.iBits];
		signal.sBitField = work.cellValue(row, column);
		signal.bBitField = false;
		signal.b2BitField = false;
		signal.iBitSigns = 0;
		signal.bBitSigns = false;

		// ������ ������������
		column = header.adress[header.iComment];
		signal.sCommentField = work.cellValue(row, column);
		signal.bCommentField = false;

		bool bEmpty = isEmpty(work, row);
		bool bRemark = isRemark(work, row);

		// ���������� �������
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
	}

	return signals;
}

// �������� ������ �� �������
bool CReaderExcel::isEmpty(CWorkExcel& work, long row) {
	bool result = true;

	for (long i = 1; i < header.size; i++) {
		long column = header.adress[i];
		CString tmp = work.cellValue(row, column);
		if (!tmp.IsEmpty())
			result = false;
	}

	return result;
}

// �������� �� ������ �����������
bool CReaderExcel::isRemark(CWorkExcel& work, long row) {
	bool result = false;

	for (long i = 1; i < header.size; i++) {
		long column = static_cast<long> (header.adress[i]);
		CString tmp = work.cellValue(row, column);
		result = (tmp.Find(_T("����������:")) > -1 ||
			tmp.Find(_T("����������:")) > -1) ? true : result;
	}

	return result;
}

// �������� ����������
bool CReaderExcel::checkExtension(CString path) {
	bool result = false;
	int posDot = path.Find(_T("."), path.GetLength() - 5);	// ����� �������
	CString pathExtension = path.Mid(posDot + 1);	// ��������� ���������� �� ����

	for (size_t i = 0; i < extension.size(); i++)
		if (pathExtension.CompareNoCase(extension[i]) == 0)
			result = true;

	return result;
}