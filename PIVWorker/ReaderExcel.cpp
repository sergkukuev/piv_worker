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
		long column;
		long row = i;
		int merge = 0;

		// ������ ����
		column = header.adress[header.iNumWord];
		merge = work.getMerge(row, column);
		getNumWord(work.cellValue(row, column), signal.numWord);
		
		// ������������ � �������������
		column = header.adress[header.iName];
		row = i;
		merge = work.getMerge(row, column);
		signal.title[0] = work.cellValue(row, column);

		column = header.adress[header.iSignal];
		signal.title[1] = work.cellValue(row, column);

		// ������ �����������
		column = header.adress[header.iDimension];
		signal.dimension = work.cellValue(row, column);

		// min, max, csr 
		column = header.adress[header.iMin];
		getDouble(work.cellValue(row, column), signal.min);

		column = header.adress[header.iMax];
		getDouble(work.cellValue(row, column), signal.max);
		
		column = header.adress[header.iCSR];
		getDouble(work.cellValue(row, column), signal.csr);
		
		// ������������ ��������
		column = header.adress[header.iBits];
		getBits(work.cellValue(row, column), signal.bit);

		// ������ ������������
		signal.comment = getComment(work, row, merge);

		bool bEmpty = isEmpty(work, row);
		bool bRemark = isRemark(work, row);

		// ���������� �������
		if (!bEmpty && !bRemark)
			signals.push_back(signal);

		i += merge - 1;
	}
}

// �������� ������ ���� �� ������ � �����
void CReaderExcel::getNumWord(CString numeric, vector <int>& result) {
	int posDot = numeric.Find(_T(','));
	if (posDot == -1) {
		numeric.Trim();
		result.push_back(_wtoi(numeric));
	}
	else {
		CString numeric2 = numeric;	
		numeric.Delete(posDot, numeric.GetLength());	// ������ �����
		numeric.Trim();
		result.push_back(_wtoi(numeric));

		numeric2.Delete(0, posDot + 1);					// ������ �����
		numeric2.Trim();
		result.push_back(_wtoi(numeric2));
	}
}

// ������� �� ������������ �������� �� ������ � �����
void CReaderExcel::getBits(CString bits, vector <int>& result) {
	int posDot = bits.Find(_T(','));
	vector <int> tmp;

	// ��� ������ ����������
	if (posDot == -1)
		tmp = stepGetBits(bits);
	else { // ��� ���� �����������
		CString bits2 = bits;
		bits2.Delete(0, posDot + 1);
		bits.Delete(posDot, bits.GetLength());
		bits.Trim();	bits2.Trim();

		tmp = stepGetBits(bits);
		tmp.resize(4);
		tmp[1] = -1;
		if (tmp.size() != 2)
			tmp.resize(4);

		vector <int> tmp2 = stepGetBits(bits2);
		for (size_t i = 0; i < tmp2.size(); i++)
			tmp[i + 2] = tmp2[i];
	}

	for (size_t i = 0; i < tmp.size(); i++)
		result[i] = tmp[i];
}

// �������������� ������� ��� �������� ��������
vector <int> CReaderExcel::stepGetBits(CString bits) {
	vector <int> result;
	bits.Trim();

	// ����� ������� ������������
	int indxDel = bits.Find(_T('.'));
	if (indxDel == -1)
		indxDel = bits.Find(_T('�'));

	if (indxDel == -1)	// ������������ ���, ������������ ���� ������
		result.push_back(_wtoi(bits));
	else {
		CString num = bits;
		CString num2 = num;	// ������ �����
		int posDot = bits.ReverseFind(_T('.'));

		if (posDot != -1)
			indxDel = posDot;

		num.Delete(indxDel, num.GetLength());	// ������ �����
		num.Trim();
		result.push_back(_wtoi(num));

		num2.Delete(0, indxDel + 1);
		num2.Trim();
		result.push_back(_wtoi(num2));
	}

	return result;
}

// ��������� ���� ��������
void CReaderExcel::getDouble(CString field, double& result) {
	if (!field.IsEmpty())
		result = _wtof(field);
}

// ������ ����������
CString CReaderExcel::getComment(CWorkExcel& work, long row, int size) {
	long column = header.adress[header.iComment];
	CString result = work.cellValue(row, column);

	for (int i = 1; i < size; i++) {
		CString tmp = work.cellValue(row + i, column);
		if (!tmp.IsEmpty())
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