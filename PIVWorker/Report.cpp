#include "stdafx.h"
#include "Report.h"

// �����������
CReport::CReport()	{	}

// ����������
CReport::~CReport()	{	}

#pragma region GenerateReport

// ��������� ������ �� �������
void CReport::getReport(pivData& data, const CString& pathToSave) {
	if (pathToSave.IsEmpty())
		throw EmptyPathException();

	path = pathToSave;
	// ������� ���������� ������� � ������
	setAmount(data.books);
	setAmountError(data.db);

	makeReport(data.db);
}

// ��������� ������
void CReport::makeReport(list <errorSet>& db) {
	ofstream file;	// ����� ������ � ����
	CString tPath;	// ���� � �������� �����

	// �������� � �������� �����
	tPath.Format(_T("%s\\�����.html"), path);
	file.open(tPath);

	errorTable(file);

	if (amount.error != 0)
		startWrite(file, db);
	else
		file << "\t\t<h2>������ � ��������� ���!</h2>\n";
	
	file << "\t</body>\n"
		"</html>\n";

	file.close();	// �������� �����
}

// ������ ��������� ������ � ����������
void CReport::startWrite(ofstream& file, list <errorSet>& Db) {
	// �������� ���������� ��� ������
	CString tPath;
	tPath.Format(_T("%s\\Error"), path);
	CreateDirectory(tPath, NULL);
	tPath.Format(_T("%s%s"), path, SYNTAX_FOLDER);
	CreateDirectory(tPath, NULL);
	tPath.Format(_T("%s%s"), path, SIMANTIC_FOLDER);
	CreateDirectory(tPath, NULL);
	tPath.Format(_T("%s%s"), path, WARNING_FOLDER);
	CreateDirectory(tPath, NULL);

	// ��������� �����
	file << "\t\t<h3>�����.</h3>\n"
		"\t\t<table border=\"1\"cellspacing=\"0\">\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th rowspan=\"2\">�������� �����</th>\n"
		"\t\t\t\t<th rowspan=\"2\">�������� �����</th>\n"
		"\t\t\t\t<th colspan=\"3\">������ � ���������</th>\n"
		"\t\t\t\t<th rowspan=\"2\">��������������</th>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th>��������������</th>\n"
		"\t\t\t\t<th>�������������</th>\n"
		"\t\t\t</tr>\n";

	// ����� �� ������
	for (list <errorSet>::iterator it = Db.begin(); it != Db.end(); it++) {
		file << "\t\t\t<tr>\n"
			"\t\t\t\t<td>"; file << CT2A(it->book->name); file << "</td>\n"
			"\t\t\t\t<td align=\"center\">\n"
			"\t\t\t\t\t<dl>\n";

		writeBook(file, it);	// ������ ������
		file << "\t\t\t</tr>\n";
	}
	file << "\t\t</table>\n";
}

// ������ ���� ������ �� �����
void CReport::writeBook(ofstream& file, list <errorSet>::iterator& it) {
	// ������������ ����� �������
	for (size_t i = 0; i < it->set.size(); i++) {
		file << "\t\t\t\t\t\t<dt>";file << CT2A(it->set[i].sheet->name); file << "</dt>\n";
	}
	file << "\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n";
	
	writeSheets(file, it);
}

// ������ ���� ������ � ������
void CReport::writeSheets(ofstream& file, list <errorSet>::iterator& it) {
	// ������ �������������� ������
	file << "\t\t\t\t<td align=\"center\">\n"
		"\t\t\t\t\t<dl>\n";
	for (size_t i = 0; i < it->set.size(); i++) {
		file << CT2A(writeErrors(it->set[i].sheet, it->set[i].syntax, SYNTAX_FOLDER, it->book->name));
	}
	file << "\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n";
	// ������ ������������� ������
	file << "\t\t\t\t<td align=\"center\">\n"
		"\t\t\t\t\t<dl>\n";
	for (size_t i = 0; i < it->set.size(); i++) {
		file << CT2A(writeErrors(it->set[i].sheet, it->set[i].simantic, SIMANTIC_FOLDER, it->book->name));
	}
	file << "\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n";
	// ������ ���������
	file << "\t\t\t\t<td colspan=\"3\" align=\"center\">\n"
		"\t\t\t\t\t<dl>\n";
	for (size_t i = 0; i < it->set.size(); i++) {
		file << CT2A(writeErrors(it->set[i].sheet, it->set[i].warning, WARNING_FOLDER, it->book->name));
	}
	file << "\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n";
}

// ������ ������ � ������ �����
CString CReport::writeErrors(sheetData* sheet, const vector <errorSignal>& db, const CString& folder, const CString& bookName) {
	CString pathFile;
	// �������� ���������� ��� �����
	pathFile.Format(L"%s%s\\%s", path, folder, bookName);
	CreateDirectory(pathFile, NULL);
	
	CString result;	// �������������� ������ ��� ������ ������ � ������� ����
	int count = countError(db);

	if (count > 0)
	{
		pathFile.Format(L"%s\\%s.html", pathFile, sheet->name);
		result.Format(L"\t\t\t\t\t\t<dt><a href=\"%s\">%d</a></dt>\n", pathFile, count);	// ������������ �������������� ������
		
		ofstream file;
		file.open(pathFile);
		// �������
		file << "<html>\n"
			"\t<head>\n"
			"\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n"
			"\t\t<title>��������� �� ����� \""; file << CT2A(bookName); file << "\"</title>\n"
			"\t</head>\n"
			"\t<body>\n"
			"\t\t<table border=\"1\"cellspacing=\"0\">\n"
			"\t\t\t<tr>\n"
			"\t\t\t\t<th rowspan=\"2\">� ���������</th>\n"
			"\t\t\t\t<th colspan=\"9\">���������. ����� \""; file << CT2A(bookName); file << "\". ���� \""; file << CT2A(sheet->name); file << "\".</th>\n"
			"\t\t\t</tr>\n"
			"\t\t\t<tr>\n"
			"\t\t\t\t<th>������</th>\n"
			"\t\t\t\t<th>������������ �������</th>\n"
			"\t\t\t\t<th>�������� ����������� ��������� / �������</th>\n"
			"\t\t\t\t<th>������� ���������</th>\n"
			"\t\t\t\t<th>����������� ��������</th>\n"
			"\t\t\t\t<th>������������ ��������</th>\n"
			"\t\t\t\t<th>���� �������� �������</th>\n"
			"\t\t\t\t<th>������������ �������</th>\n"
			"\t\t\t\t<th>����������</th>\n"
			"\t\t\t</tr>\n";

		for (size_t i = 0; i < db.size(); i++) {
			file << "\t\t\t<tr>\n"
				"\t\t\t\t<th rowspan=\"2\"> &nbsp "; file << i + 1; file << "</th>\n";
			writeSignal(file, db[i]);
		}

		file << "\t\t</table>\n"
			"\t</body>\n"
			"</html>\n";

		file.close();
	}
	else
		result.Format(L"\t\t\t\t\t\t<dt>-</dt>\n"); // ������������ �������������� ������

	return result;
}

// ������ �������
void CReport::writeSignal(ofstream& file, const errorSignal& set) {
	CString buffer;
	file << CT2A(writeParam(set.signal->numWord.field, findRemark(set.error, errRemarks[0])));

	buffer.Format(L"\t\t\t\t<td align=\"center\"> &nbsp %s</td>\n", set.signal->title[0]);
	file << CT2A(buffer);

	file << CT2A(writeParam(set.signal->title[1], findRemark(set.error, errRemarks[1])));
	
	buffer.Format(L"\t\t\t\t<td align=\"center\"> &nbsp %s</td>\n", set.signal->dimension);
	file << CT2A(buffer);

	file << CT2A(writeParam(set.signal->min.field, findRemark(set.error, errRemarks[2])));
	file << CT2A(writeParam(set.signal->max.field, findRemark(set.error, errRemarks[3])));
	file << CT2A(writeParam(set.signal->csr.field, findRemark(set.error, errRemarks[4])));

	file << CT2A(writeParam(set.signal->bit.field, findRemark(set.error, errRemarks[5])));
	file << CT2A(writeParam(set.signal->comment, findRemark(set.error, errRemarks[6])));

	file << "\t\t\t</tr>\n"
		"\t\t\t<tr>\n";

	// ������ ���� ������
	for (size_t j = 0; j < set.error.size(); j++) {
		if (IsRemark(set.error[j])) {
			if (j != 0) {
				file << "\t\t\t\t\t</ul>\n";
				buffer.Format(L"\t\t\t\t&nbsp %s\n", set.error[j]);
				file << CT2A(buffer);
			}
			else {
				buffer.Format(L"\t\t\t\t<td colspan=\"9\" bgcolor = \"#FDFCD0\"> &nbsp %s\n", set.error[j]);
				file << CT2A(buffer);
			}
			file << "\t\t\t\t\t<ul>\n";
		}
		else {
			buffer.Format(L"\t\t\t\t\t<li>%s</li>\n", set.error[j]);
			file << CT2A(buffer);
		}
	}

	file << "\t\t\t\t\t</ul>\n";
		"\t\t\t\t</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<td colspan=\"10\"> &nbsp </td>\n"
		"\t\t\t</tr>\n";
}

// ������ ��������� �������
CString CReport::writeParam(const CString& field, const bool& color) {
	CString result;
	color ? result = L"bgcolor = \"#FDFCD0\"" : result = L"";
	result.Format(L"\t\t\t\t<td align=\"center\" %s> &nbsp %s</td>\n", result, field);
	return result;
}

// ������� � ����� ����������� � ���������� ������
void CReport::errorTable(ofstream& file) {
	file << "<html>\n"
		"\t<head>\n"
		"\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n"
		"\t\t<title>���������</title>\n"
		"\t</head>\n"
		"\t<body>\n"
		"\t\t<h3>����������.</h3>\n"
		"\t\t<table border=\"1\"cellspacing=\"0\">\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">������� ���������� �����</th>\n"
		"\t\t\t\t<td align=\"center\">";	file << amount.all;	file << "</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">������� ���������� � ��������</th>\n"
		"\t\t\t\t\<td align=\"center\">";	file << amount.withError; file << "</td> \n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">������� ���������� ��� ������</th>\n"
		"\t\t\t\t<td align=\"center\">";	file << amount.withoutError; file << "</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">����� ������</th>\n"
		"\t\t\t\t<td align=\"center\">"; file << amount.error; file << "</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">����� ��������������</th>\n"
		"\t\t\t\t<td align=\"center\">";	file << amount.warning;	file << "</td>\n"
		"\t\t\t</tr>\n"
		"\t\t</table>\n"
		"\t\t<br/>\n";
}

// ����� ������ ��������� � �������
bool CReport::findRemark(const vector <CString>& error, const CString& remark) {
	bool result = false;
	for (size_t i = 0; i < error.size(); i++)
		error[i].CompareNoCase(remark) == 0 ? result = true : result = result;
	return result;
}

// �������� �� ������ ������� ��� ����������
bool CReport::IsRemark(const CString& field) {
	bool result = false;
	for (size_t i = 0; i < REMARKS_SIZE; i++)
		errRemarks[i].CompareNoCase(field) == 0 ? result = true : result = result;
	return result;
}

// ������� ���������� ������ �� �����
int CReport::countError(const vector<errorSignal>& set) {
	int result = 0;
	for (size_t i = 0; i < set.size(); i++)
		for (size_t j = 0; j < set[i].error.size(); j++)
			if (!IsRemark(set[i].error[j]))
				result++;
	return result;
}

// ��������� ���������� ������ ������ (�����, � ��������, ���)
void CReport::setAmount(list <bookData>& books) {
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++) {
		amount.all += (int)it->sheets.size();
		for (size_t j = 0; j < it->sheets.size(); j++)
			if (it->sheets[j].error)
				amount.withError++;
	}
	amount.withoutError = amount.all - amount.withError;
}

// ��������� ���������� ������ � ���������
void CReport::setAmountError(list <errorSet>& db) {
	for (list<errorSet>::iterator it = db.begin(); it != db.end(); it++)
		for (size_t i = 0; i < it->set.size(); i++) {
			amount.error += countError(it->set[i].syntax) + countError(it->set[i].simantic);
			amount.warning += countError(it->set[i].warning);
		}
}

#pragma endregion

#pragma region GenerateTxt

// ������ ��������� txt ��� ������ ���������
void CReport::getTxt(const bookData& book, const CString& pathToSave, const bool& bNumPK) {
	if (pathToSave.IsEmpty())
		throw EmptyPathException();
	// �������� ����������
	path = pathToSave;
	path.Format(L"%s\\Text", pathToSave);
	CreateDirectory(path, NULL);

	Generate(book, bNumPK);
}

// ������ ��������� txt ������
void CReport::getTxt(list <bookData>& books, const CString& pathToSave, const bool& bNumPK) {
	if (pathToSave.IsEmpty())
		throw EmptyPathException();
	// �������� ����������
	path = pathToSave;
	path.Format(L"%s\\Text", pathToSave);
	CreateDirectory(path, NULL);

	// ����� �� ������
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++)
		Generate(*it, bNumPK);
}

// ��������� txt ��� �����
void CReport::Generate(const bookData& book, const bool& bNumPK) {
	CString tPath = path;
	tPath.Format(L"%s\\%s", path, book.name);
	CreateDirectory(tPath, NULL);

	CString filePath = path;
	filePath.Format(L"%s\\_ProtocolMain.txt", tPath);

	// �������� �������� ����� ��� ������
	ofstream mainFile;
	mainFile.open(filePath);

	// ����� �� ���������
	for (size_t i = 0; i < book.sheets.size(); i++) {
		if (!book.sheets[i].error) { // ���� ��� �� �������� ������
			ofstream tmpFile;
			CString name;
			sheetInfo info;

			info.np = book.sheets[i].np;
			info.pk = book.sheets[i].pk;
			info.bPK = bNumPK;

			// �������� txt ����� ��� ������
			name.Format(L"NP_%d_%s.txt", info.np, book.sheets[i].name);
			filePath.Format(L"%s\\%s", tPath, name);
			tmpFile.open(filePath);
			mainFile << "#include \""; mainFile << CT2A(name); mainFile << "\"\n";

			for (size_t j = 0; j < book.sheets[i].signals.size(); j++)
				writeTxtParam(tmpFile, book.sheets[i].signals[j], info);	// ������ ���������

			tmpFile.close();
		}
		else continue;
	}
	mainFile.close();
}

// ������ ������� � txt ����
void CReport::writeTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info) {
	CString buffer = signal.title[0];

	if (buffer.Find(RESERVE_SIGNAL) == -1) {
		buffer.Format(L"PAR=%s\n", signal.title[1]);	// ������ ����������� �������
		file << CT2A(buffer);

		buffer.Replace(L"\"", L"\\\"");
		buffer.Format(L"\tNAME=\"%s\"\n", signal.title[0]); // ������������ �������
		file << CT2A(buffer);

		if (!signal.dimension.IsEmpty()) {
			buffer.Format(L"\tUNIT=\"%s\"\n", signal.dimension);	// �����������
			file << CT2A(buffer);
		}

		// ��������� ������
		if (info.pk != 0 && info.bPK)
			buffer.Format(L"\tSET=%d:%d\n", info.pk, info.np);
		else
			buffer.Format(L"\tSET=%d\n", info.np);
		file << CT2A(buffer);

		int max = (signal.bit.value[1] == -1 ? signal.bit.value[0] : signal.bit.value[1]);

		// ������ ������ ����� � �����
		if (signal.numWord.value.size() == 2) { 
			file << "\tMERGE\n";
			buffer.Format(L"\t\tWDADDR = %d,%d,%d\n", signal.numWord.value[0], signal.bit.value[0], max);
			file << CT2A(buffer);
			max = (signal.bit.value[3] == -1) ? signal.bit.value[2] : signal.bit.value[3];
			buffer.Format(L"\t\tWDADDR = %d,%d,%d\n", signal.numWord.value[1], signal.bit.value[2], max);
			file << CT2A(buffer);
			file << "\tEND_MERGE\n";
		}
		else {
			buffer.Format(L"\tWDADDR = %d,%d,%d\n", signal.numWord.value[0], signal.bit.value[0], max);
			file << CT2A(buffer);
		}

		// ���, ����, ���
		if (signal.max.value != DBL_MIN && signal.csr.value != DBL_MIN) { 
			file << "\tVALDESCR\n";
			file << (signal.bitSign ? "\t\tSIGNED\n" : "\t\tUNSIGNED\n");

			buffer.Format(L"\t\tMIN = %lf\n", signal.min.value);
			file << CT2A(buffer);

			buffer.Format(L"\t\tMAX = %lf\n", signal.max.value);
			file << CT2A(buffer);

			buffer.Format(L"\t\tMSB = %lf\n", signal.csr.value);
			file << CT2A(buffer);

			file << "\tEND_VALDESCR\n";
		}

		// ������ ������������
		if (!signal.comment.IsEmpty()) {	
			file << "\tCOMMENT\n";

			buffer = signal.comment;
			buffer.Replace(L"\"", L"\\\"");
			buffer.Replace(L"\n", L"\"\n\t\t\"");
			buffer.Format(L"\t\t\"%s\"\n", buffer);

			file << CT2A(buffer);
			file << "\tEND_COMMENT\n";
		}
		file << "END_PAR\n\n";
	}
}

#pragma endregion
