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

		column = header.adress[header.iNumWord];
		int step = 0;
		Cell cell;
		cell.row = row; cell.column = column;
		signal.numWord = getNumWord(work, cell, step);
		
		cell.column = header.adress[header.iSignal];
		long rowID = 0; int size = 0;
		signal.title = getTitle(work, cell, rowID, size);

		// ������ �����������, min, max � csr
		column = header.adress[header.iDimension];
		signal.dimension = work.cellValue(rowID, column);

		column = header.adress[header.iMin];
		signal.min = work.cellValue(rowID, column);

		column = header.adress[header.iMax];
		signal.max= work.cellValue(rowID, column);
		
		column = header.adress[header.iCSR];
		signal.csr = work.cellValue(rowID, column);
		
		// ������ ��������
		column = header.adress[header.iBits];
		signal.bit = work.cellValue(rowID, column);

		// ������ ������������
		cell.column = header.adress[header.iComment];
		cell.row = rowID;
		signal.comment = getComment(work, cell, size);

		bool bEmpty = isEmpty(work, row);
		bool bRemark = isRemark(work, row);

		// ���������� �������
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
	}
}

// �������� ������ ����
vector <int> CReaderExcel::getNumWord(CWorkExcel& work, Cell cell, int& step) {
	CString field = work.cellValue(cell);
	step = 0;

	while (field.IsEmpty()) {
		cell.row--;
		field = work.cellValue(cell);
	}

	vector <int> result;
	translateNumWord(field, result);

	do {
		cell.row++;	step++;
		field = work.cellValue(cell);
	} while (field.IsEmpty() && cell.row < work.countRows());
	if (cell.row < work.countRows())
		step++;
	return result;
}

// ������� ������� ���� �� ������ � �����
void CReaderExcel::translateNumWord(CString numeric, vector <int>& numWord) {
	int posDot = numeric.Find(_T(','));

	if (posDot == -1) {
		numeric.Trim();	// �������� ��������
		numWord.push_back(_wtoi(numeric));
	}
	else {
		CString numeric2 = numeric;	
		numeric.Delete(posDot, numeric.GetLength());	// ������ �����
		numeric.Trim();
		numWord.push_back(_wtoi(numeric));

		numeric2.Delete(0, posDot + 1); // ������ �����
		numeric2.Trim();
		numWord.push_back(_wtoi(numeric2));
	}
}

vector <CString> CReaderExcel::getTitle(CWorkExcel& work, Cell cell, long& size, int& step) {
	CString field = work.cellValue(cell);
	size = 0;

	while (field.IsEmpty()) {
		cell.row--;
		field = work.cellValue(cell);
	}
	size = cell.row;

	vector <CString> result = { _T(""), _T("") };
	result[0] = work.cellValue(cell.row, header.adress[header.iName]);
	result[1] = work.cellValue(cell.row, header.adress[header.iSignal]);
	do {
		cell.row++;	step++;
		field = work.cellValue(cell);
	} while (field.IsEmpty() && cell.row < work.countRows());
	if (cell.row < work.countRows())
		step++;
	return result;
}

CString CReaderExcel::getComment(CWorkExcel& work, Cell cell, int size) {
	CString result = work.cellValue(cell);
	for (int i = 1; i < size; i++) {
		CString tmp = work.cellValue(cell.row + i, cell.column);
		result += _T("\n") + tmp;
	}
		
	return result;
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