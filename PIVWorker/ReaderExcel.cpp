#include "stdafx.h"
#include "ReaderExcel.h"

// �����������
CReaderExcel::CReaderExcel() {
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
	header.list.push_back(vector<CString>{ L"�����"});																// ����� (arinc)

	// ���������� ���� ����� ���������� excel ������
	extension.push_back(L"xlsx");	// ������� �������� ������
	extension.push_back(L"xls");	// �������� ������ �� 2003 ����
}

// ����������
CReaderExcel::~CReaderExcel() {
	header.list.clear();
	header.list.shrink_to_fit();
	extension.clear();
	extension.shrink_to_fit();
}

// ������ ����� �����
bookData CReaderExcel::getBook(const CString& pathToExcel) {
	bookData book;
	CWorkExcel work;

	if (!checkExtension(pathToExcel)) {
		BadTypeException exc;
		exc.setName(work.bookName(pathToExcel));
		throw exc;
	}

	if (!work.openBook(pathToExcel)) {
		ReadBookException exc;
		exc.setParam(work.bookName(pathToExcel));
		throw exc;
	}

	book.name = work.bookName();
	getSheets(book.sheets, work);
	
	return book;
}

// ������ ������
void CReaderExcel::getSheets(vector <sheetData>& sheets, CWorkExcel& work) {
	sheets.resize(work.countSheets());
	for (long i = 1; i < work.countSheets() + 1; i++) {
		work.openSheet((long)i);
		sheets[i - 1].name = work.sheetName();

		// ����� ���������� �� �����
		if (!work.findHeader(header)) {
			NotAllHeaderException exc;
			exc.setParam(sheets[i - 1].name, work.bookName());
			throw exc;
		}

		sheets[i - 1].line = work.lineValue();
		sheets[i - 1].np = work.npValue(header);
		sheets[i - 1].pk = work.pkValue(header);

		bool arinc;
		sheets[i - 1].line.Find(ARINC) != -1 ? arinc = true : arinc = false;
		header.adress[header.iRow]++;

		getSignals(sheets[i - 1].signals, work, arinc);
	}
}

// ������ ���������� �� �����
void CReaderExcel::getSignals(vector <signalData>& signals, CWorkExcel& work, const bool& isArinc) {
	arincData arinc;
	for (long merge, i = header.adress[header.iRow]; i < work.countRows() + 1; i += merge) {
		signalData signal;
		long column, row = i;
		merge = 1;

		// ���������� ��� arinc
		if (isArinc) {
			if (isTitle(work, row))	// ������� ������ ���������
				continue;
			//	����� ����������
			CString remark = work.cellValue(row - 1, 1);
			if (remark.Find(ARINC_REMARK) != -1 && !arinc.flag && arinc.startRow != row)	// ��������� ����������
				getArinc(remark, row, arinc);
			else if ((remark.Find(ARINC_REMARK) != -1 || isTitle(work, row - 1)) && arinc.flag) {
				if (arinc.current == arinc.amount)
					getArinc(remark, row, arinc);
				else {
					i = row = arinc.startRow;
					arinc.current++;
				}
			}
		}

		// ������ ���������� ������ �������
		// ����� ��� ����� �����, � ����������� �� ����� ��������
		if (!isArinc) {
			column = header.adress[header.iNumWord];
			merge = work.cPrevEmpty(row, column);
			signal.numWord = getNumWord(work.cellValue(row, column));
		}
		else {
			column = header.adress[header.iAdress];
			merge = work.cPrevEmpty(row, column);
			signal.numWord = getAdress(work.cellValue(row, column), arinc.current);
		}
		
		column = header.adress[header.iName];	row = i;
		merge = work.cNextEmpty(row, column) + work.cPrevEmpty(row, column);
		signal.title[0] = work.cellValue(row, column);

		column = header.adress[header.iSignal];
		signal.title[1] = work.cellValue(row, column);
		// ������ �����, ���� ���������� � ����� ������������ ������
		if (arinc.flag) {
			CString tmp;
			// ������� ������������ ��������� � ��������� (������� ����� ������� ���������)
			tmp.Format(L"%d", arinc.current);
			int pos = signal.title[1].Find(arinc.symbol);
			signal.title[1].Replace(arinc.symbol, tmp);
		}

		column = header.adress[header.iDimension];
		signal.dimension = work.cellValue(row, column);
		getMinMaxCsr(signal, work, row);	
		column = header.adress[header.iBits];
		signal.bit = getBits(work.cellValue(row, column));
		signal.comment = getComment(work, row, merge, signal.bitSign);

		bool bEmpty = isEmpty(work, row);		// �������� �� ������ ������
		bool bRemark = isRemark(work, row);		// �������� �� ����������

		if (work.countRows() == row + merge - 1 && arinc.flag && arinc.amount != arinc.current) // ���� ��� ������ ���������� � ������ ����� �����
			i = arinc.startRow - merge;

		// ���������� �������
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
	}
}

// ������ ������ arinc (���������� ����� � �����)
void CReaderExcel::getArinc(const CString& field, const long& row, arincData& arinc) {
	CString numeric = field;
	int posEqual = numeric.Find(L'=');

	if (posEqual != -1) {
		arinc.symbol = field[posEqual - 1];
		arinc.startRow = row;

		bool flag;
		int posDel = numeric.ReverseFind(L'�');
		
		if (posDel == -1)
			posDel = numeric.ReverseFind(L'.');

		CString numeric1 = numeric.Mid(posDel + 1, numeric.GetLength() - posEqual);
		numeric.Delete(0, posEqual + 1);

		posDel = numeric.ReverseFind(L'�');
		if (posDel == -1)
			posDel = numeric.Find(L'.');

		numeric.Delete(posDel, numeric.GetLength() - posDel);
		numeric.Trim();	numeric1.Trim();
		
		arinc.current = getInt(numeric, arinc.flag);
		arinc.amount = getInt(numeric1, flag);
		flag ? arinc.flag = arinc.flag : arinc.flag = flag;
	}
	else {
		// ����� ��������� ����������
		arinc.flag = false;
		arinc.symbol = L"";
		arinc.amount = 0;
		arinc.current = 0;
	}
}

// �������� ����� �� ������ � �����
intData CReaderExcel::getAdress(const CString& field, const int& num) {
	intData result;
	CString numeric = field;
	vector <int> vec = { -1, -1 };
	int posDel, current = num;
	bool flag = false;

	do {
		posDel = numeric.Find(L',');
		if (posDel == -1)
			posDel = numeric.GetLength();

		CString str = numeric.Mid(0, posDel);
		numeric.Delete(0, posDel + 1);
		str.Replace(L'\n', L' ');
		vector<int> temp = stepGetAdress(str, result.flag);	// ������ - �����, �������� - ������� ���������
		
		if (temp.size() == 4) {
			current > temp[2] - temp[0] + 1 ? current -= temp[2] - temp[0] + 1 :
				vec[0] = temp[0] + current - 1;
			if (vec[0] != -1) {
				vec[1] = temp[1];
				flag = true;
			}
		}
		else if (temp.size() == 2 && current == 0) {
			vec = temp;
			flag = true;
		}
		else {
			result.flag = false;
		}

	} while (numeric.GetLength() != 0 && !flag);

	result.value = vec;
	result.field = field;
	return result;
}

// ��� ������� ��� ������
vector <int> CReaderExcel::stepGetAdress(const CString& adress, bool& flag) {
	vector <int> result = {-1, -1};
	CString num = adress;
	num.Trim();

	// ����� ������� ������������
	int indxDel = num.Find(L'.');
	if (indxDel == -1)
		indxDel = num.Find(L'�');

	if (indxDel == -1) { // ������������ ���, ������������ ���� ������
		result[1] = getSubIndex(num);
		result[0] = getInt(num, flag);
	}
	else {
		result.push_back(-1);
		result.push_back(-1);
		int posDot = num.ReverseFind(L'.');
		if (posDot != -1)
			num.Delete(indxDel, posDot - indxDel);

		CString num2 = num;
		num.Delete(indxDel, num.GetLength());	// ������ �����
		num.Trim();
		result[1] = getSubIndex(num);
		result[0] = getInt(num, flag);

		num2.Delete(0, indxDel + 1);	// ������ �����
		num2.Trim();
		result[3] = getSubIndex(num2);
		result[2] = getInt(num2, flag);
	}

	return result;
}

// ��������� ������������ ������� � �������� ��� �� ������
int CReaderExcel::getSubIndex(CString& numeric) {
	// ����������� ������ � �����(8320 - 8329)
	CString sub;
	for (int i = 0; i < numeric.GetLength(); i++) {
		if ((int)numeric[i] > 8319) {
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
	int result = getInt(sub, flag);

	if (!flag)
		result = -1;

	return result;
}

// �������� ������ ���� �� ������ � �����
intData CReaderExcel::getNumWord(const CString& field) {
	intData result;
	CString numeric = field;
	int posDot = numeric.Find(_T(','));
	vector <int> vec;
	vec.push_back(-1);

	if (posDot == -1) {
		numeric.Trim();
		vec[0] = getInt(numeric, result.flag);
	}
	else {
		vec.push_back(-1);
		CString numeric2 = numeric;	
		numeric.Delete(posDot, numeric.GetLength());	// ������ �����
		numeric.Trim();
		vec[0] = getInt(numeric, result.flag);

		numeric2.Delete(0, posDot + 1);					// ������ �����
		numeric2.Trim();
		vec[1] = getInt(numeric2, result.flag);
	}

	result.value = vec;
	result.field = field;
	return result;
}

// �������� �������� ���, ���� � ���
void CReaderExcel::getMinMaxCsr(signalData& signal, CWorkExcel& work, const long& row) {
	long column = header.adress[header.iMin];
	signal.min.field = work.cellValue(row, column);
	signal.min.value = getDouble(signal.min.field, signal.min.flag);

	column = header.adress[header.iMax];
	signal.max.field = work.cellValue(row, column);
	signal.max.value = getDouble(signal.max.field, signal.max.flag);

	column = header.adress[header.iCSR];
	signal.csr.field = work.cellValue(row, column);
	signal.csr.value = getDouble(signal.csr.field, signal.csr.flag);

	// ����� ������, ���� ������������ ��� ��������� �����
	if (signal.min.value == DBL_MIN && signal.max.value == DBL_MIN && signal.csr.value == DBL_MIN) {
		signal.min.flag = false;
		signal.max.flag = false;
		signal.csr.flag = false;
	}
}

// ������� �� ������������ �������� �� ������ � �����
intData CReaderExcel::getBits(const CString& field) {
	intData result;
	CString bits = field;
	int posDot = bits.Find(_T(','));
	vector <int> vec;

	// ��� ������ ����������
	if (posDot == -1)
		vec = stepGetBits(bits, result.flag);
	else { // ��� ���� �����������
		CString bits2 = bits;
		bits2.Delete(0, posDot + 1);
		bits.Delete(posDot, bits.GetLength());
		bits.Trim();	bits2.Trim();

		vec = stepGetBits(bits, result.flag);
		vec[1] = -1;
		vec.resize(4);

		vector <int> vec2 = stepGetBits(bits2, result.flag);
		for (size_t i = 0; i < vec2.size(); i++)
			vec[i + 2] = vec2[i];
	}

	result.field = field;
	result.value = vec;
	return result;
}

// �������������� ������� ��� �������� ��������
vector <int> CReaderExcel::stepGetBits(const CString& bits, bool& flag) {
	vector <int> result;
	CString num = bits;
	num.Trim();

	result.push_back(-1);
	result.push_back(-1);

	// ����� ������� ������������
	int indxDel = num.Find(L'.');
	if (indxDel == -1)
		indxDel = num.Find(L'�');

	if (indxDel == -1)	// ������������ ���, ������������ ���� ������
		result[0] = getInt(num, flag);
	else {
		int posDot = num.ReverseFind(L'.');
		if (posDot != -1)
			num.Delete(indxDel, posDot - indxDel);

		CString num2 = num;
		num.Delete(indxDel, num.GetLength());	// ������ �����
		num.Trim();
		result[0] = getInt(num, flag);

		num2.Delete(0, indxDel + 1);	// ������ �����
		num2.Trim();
		result[1] = getInt(num2, flag);
	}

	return result;
}

// ��������� double ��������
double CReaderExcel::getDouble(const CString& field, bool& flag) {
	double result = DBL_MIN;
	CString temp = field;
	temp.Trim();
	temp.Replace(L',', L'.');
	CStringA tmp(temp);
	char* str = new char[tmp.GetLength() + 1];
	strcpy_s(str, tmp.GetLength() + 1, tmp);

	if (!temp.IsEmpty()) {
		char* end;
		errno = 0;
		result = strtod(str, &end);
		(*end != '\0' || errno != 0) ? flag = false : flag = true;
	}
	else
		flag = true;

	delete[] str;
	return result;
}

// ��������� int ��������
int CReaderExcel::getInt(const CString& field, bool& flag) {
	int result = -1;
	CString temp = field;
	temp.Trim();
	CStringA tmp(temp);
	char* str = new char[tmp.GetLength() + 1];
	strcpy_s(str, tmp.GetLength() + 1, tmp);

	if (!field.IsEmpty()) {
		char* end;
		errno = 0;
		result = strtol(str, &end, 10);
		(*end != '\0' || errno != 0) ? flag = false : flag = true;
	}
	else
		flag = true;

	return result;
}

// ������ ����������
CString CReaderExcel::getComment(CWorkExcel& work, const long& row, const int& size, bool& flag) {
	long column = header.adress[header.iComment];
	long tmpRow = row;
	int merge = work.getMerge(tmpRow, column);
	CString result;

	(size > merge) ? merge = size : merge = merge;
	for (int i = 0; i < merge; i++) {
		CString tmp = work.cellValue(tmpRow + i, column);
		tmp.Trim();	
		if (!tmp.IsEmpty()) {
			(tmp.Find(SIGN_FIELD) != -1) ? flag = true : flag = flag;
			result.IsEmpty() ? result = tmp : result += L'\n' + tmp;
		}
	}
	return result;
}

// �������� ������ �� ���������
bool CReaderExcel::isTitle(CWorkExcel& work, const long& row) {
	bool result = true;
	CString numeric = work.cellValue(row, 1);
	numeric.Trim();
	getInt(numeric, result);
	return !result;
}

// �������� ������ �� �������
bool CReaderExcel::isEmpty(CWorkExcel& work, const long& row) {
	bool result = true;

	for (long i = 1; i < header.size; i++) {
		long column = header.adress[i];
		if (header.adress[i] != -1 && header.adress[i] != 1) {
			CString tmp = work.cellValue(row, column);
			if (!tmp.IsEmpty())
				result = false;
		}
	}

	return result;
}

// �������� �� ������ �����������
bool CReaderExcel::isRemark(CWorkExcel& work, const long& row) {
	bool result = false;

	for (long i = 1; i < header.size; i++) {
		long column = header.adress[i];
		if (header.adress[i] != -1) {
			CString tmp = work.cellValue(row, column);
			result = (tmp.Find(REMARK1) > -1 ||
				tmp.Find(REMARK2) > -1) ? true : result;
		}
	}

	return result;
}

// �������� ����������
bool CReaderExcel::checkExtension(const CString& path) {
	bool result = false;
	int dotPos = path.ReverseFind(_T('.'));	// ����� �������
	CString ext = path.Mid(dotPos + 1);		// ��������� ���������� �� ����

	for (size_t i = 0; i < extension.size(); i++)
		if (ext.CompareNoCase(extension[i]) == 0)
			result = true;

	return result;
}