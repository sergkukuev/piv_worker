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
		getSheets(book.sheets, work);
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
void CReaderExcel::getSheets(vector <sheetData>& sheets, CWorkExcel& work) {
	sheets.resize(work.countSheets());
	try {
		for (long i = 1; i < work.countSheets(); i++) {
			work.openSheet((long)i);
			// ����� ���������� �� �����
			if (!work.findHeader(header)) {
				NotAllHeaderException exc;
				exc.setParam(sheets[i - 1].name, work.bookName());
				throw exc;
			}

			sheets[i - 1].name = work.sheetName();
			sheets[i - 1].line = work.lineValue();
			sheets[i - 1].np = work.npValue(header);
			sheets[i - 1].pk = work.pkValue(header);

			header.adress[header.iRow]++;

			getSignals(sheets[i - 1].signals, work);
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
}

// ������ ���������� �� �����
void CReaderExcel::getSignals(vector <signalData>& signals, CWorkExcel& work) {

	for (long i = header.adress[header.iRow]; i < work.countRows() + 1; i++) {
		signalData signal;
		long row, column;

		row = i;
		// ������ ����������
		// ������������ ��������� � ����������� �������
		column = header.adress[header.iName];
		signal.title[0] = work.cellValue(row, column);

		column = header.adress[header.iSignal];
		signal.title[1] = work.cellValue(row, column);

		// ������ ������ �����
		column = header.adress[header.iNumWord];
		signal.numWord = work.cellValue(row, column);
	
		// ������ �����������, min, max � csr
		column = header.adress[header.iDimension];
		signal.dimension = work.cellValue(row, column);

		column = header.adress[header.iMin];
		signal.min = work.cellValue(row, column);

		column = header.adress[header.iMax];
		signal.max= work.cellValue(row, column);
		
		column = header.adress[header.iCSR];
		signal.csr = work.cellValue(row, column);

		// ������ ��������
		column = header.adress[header.iBits];
		signal.bit = work.cellValue(row, column);

		// ������ ������������
		column = header.adress[header.iComment];
		signal.comment = work.cellValue(row, column);

		bool bEmpty = isEmpty(work, row);
		bool bRemark = isRemark(work, row);

		// ���������� �������
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
	}
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
		long column = header.adress[i];
		CString tmp = work.cellValue(row, column);
		result = (tmp.Find(_T("����������:")) > -1 ||
			tmp.Find(_T("����������:")) > -1) ? true : result;
	}

	return result;
}

// �������� ����������
bool CReaderExcel::checkExtension(CString path) {
	bool result = false;
	int dotPos = path.ReverseFind(_T('.'));	// ����� �������
	path = path.Mid(dotPos + 1);			// ��������� ���������� �� ����

	for (size_t i = 0; i < extension.size(); i++)
		if (path.CompareNoCase(extension[i]) == 0)
			result = true;

	return result;
}