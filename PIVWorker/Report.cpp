#include "stdafx.h"
#include "Report.h"

// �����������
CReport::CReport()	{	}

// ����������
CReport::~CReport()	{	}

#pragma region GenerateReport

// ��������� ������ �� �������
void CReport::getReport(list <bookData>& books, list<errorSet>& Db, const CString& pathToSave) {
	if (pathToSave.IsEmpty())
		throw EmptyPathException();

	path = pathToSave;

	// ������� ���������� ������� � ������
	setAmount(books);
	setAmountError(Db);

	makeReport(books, Db);
}

// ��������� �� ������� ������
void CReport::makeReport(list <bookData>& books, list <errorSet>& errorDB) {
	ofstream file;	// ����� ������ � ����
	CString tPath;	// ���� � �������� �����

	// �������� � �������� �����
	tPath.Format(_T("%s\\�����.html"), path);
	file.open(tPath);

	// ������ � ����
	file << "<html>\n"
		"  <head>"
		"     <meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n"
		"     <title>���������</title>\n"
		"  </head>\n"
		"  <body>\n"
		"      <h3>����������.</h3>\n"
		"      <table border=\"1\"cellspacing=\"0\">\n"
		"          <tr>\n"
		"              <th align=\"left\">������� ���������� �����</th>\n"
		"              <td align=\"center\">";	file << amount.all;	file << "</td>\n"
		"          </tr>\n"
		"          <tr>\n"
		"              <th align=\"left\">������� ���������� � ��������</th>\n"
		"              <td align=\"center\">";	file << amount.withError; file << "</td> \n"
		"          </tr>\n"
		"          <tr>\n"
		"              <th align=\"left\">������� ���������� ��� ������</th>\n"
		"              <td align=\"center\">";	file << amount.withoutError; file << "</td>\n"
		"          </tr>\n"
		"          <tr>\n"
		"              <th align=\"left\">����� ������</th>\n"
		"              <td align=\"center\">"; file << amount.error; file << "</td>\n"
		"          </tr>\n"
		"          <tr>\n"
		"              <th align=\"left\">����� ��������������</th>\n"
		"              <td align=\"center\">";	file << amount.warning;	file << "</td>\n"
		"          </tr>\n"
		"      </table>\n"
		"      <br/>\n";

	if (amount.error + amount.warning != 0)
		startWrite(file, errorDB);
	else
		file << "      <h2>������ � ��������� ���!</h2>\n";
	
	file << "  </body>\n"
		"</html>\n";

	file.close();	// �������� �����
}

void CReport::startWrite(ofstream& file, list <errorSet>& errorDB) {
	CString tPath;

	// �������� ���������� ��� ������
	tPath.Format(_T("%s\\Error"), path);
	CreateDirectory(tPath, NULL);
	tPath.Format(_T("%s\\Warning"), path);
	CreateDirectory(tPath, NULL);
	tPath.Format(_T("%s\\Error\\Syntax"), path);
	CreateDirectory(tPath, NULL);
	tPath.Format(_T("%s\\Error\\Simantic"), path);
	CreateDirectory(tPath, NULL);

	file << "      <h3>�����.</h3>\n";
		"      <table border=\"1\"cellspacing=\"0\">\n"
		"          <tr>\n"
		"              <th rowspan=\"2\">�������� �����</th>\n"
		"              <th rowspan=\"2\">�������� �����</th>\n"
		"              <th colspan=\"3\">������ � ���������</th>\n"
		"              <th rowspan=\"2\">��������������</th>\n"
		"          </tr>\n"
		"          <tr>\n"
		"              <th>��������������</th>\n"
		"              <th>�������������</th>\n"
		"          </tr>\n";

	for (list <errorSet>::iterator it = errorDB.begin; it != errorDB.end(); it++) {
		file << "          <tr>\n";
		"              <td>"; file << CT2A(it->book->name); file << "</td>\n"
			"              <td align=\"center\">\n"
			"                 <dl>\n";

		writeSheets(file, it->set);
		file << "          </tr>\n";
	}
	file << "      </table>\n";
}

// �������� �������
void CReport::writeSheets(ofstream& file, vector <errorSheet>& set) {
	for (size_t i = 0; i < set.size(); i++) {
		file << "                     <dt>";
		file << CT2A(set[i].sheet->name);
		file << "</dt>\n";
	}

	file << "                 </dl>\n"
		"              </td>\n";
	// ���������
	file << "              <td align=\"center\">\n"
		"                     <dl>";
	for (size_t i = 0; i < set.size(); i++) {
		file << writeError(set[i].sheet, set[i].syntax, SYNTAX_FOLDER);
	}
	file << "                 </dl>\n"
		"              </td>\n";
	// ���������
	file << "              <td align=\"center\">\n"
		"                     <dl>";
	for (size_t i = 0; i < set.size(); i++) {
		file << writeError(set[i].sheet, set[i].simantic, SIMANTIC_FOLDER);
	}
	file << "                 </dl>\n"
		"              </td>\n";
	// ��������������
	file << "              <td colspan=\"3\" align=\"center\">\n"
		"                     <dl>";
	for (size_t i = 0; i < set.size(); i++) {
		file << writeError(set[i].sheet, set[i].warning, WARNING_FOLDER);
	}
	file << "                 </dl>\n"
		"              </td>\n";
}

CString CReport::writeError(sheetData* sheet, const vector <errorSignal>& db, const CString& folder, const CString& bookName) {
	// ������� ���� � ��������
	// �������� ���� ������
	// ������� ����� � ������

	CString pathFile;
	pathFile.Format(_T("%s\\%s\\%s\\%s.html"), path, folder, bookName, sheet->name);
	CreateDirectory(pathFile, NULL);

	ofstream file;
	file.open(pathFile);

	// �������
	file << "<html>\n"
		"  <head>\n"
		"     <meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n"
		"     <title>��������� �� ����� \""; file << CT2A(bookName); file << "\"</title>\n"
		"  </head>\n"
		"  <body>\n"
		"      <table border=\"1\"cellspacing=\"0\">\n"
		"          <tr>\n"
		"              <th rowspan=\"2\">� ���������</th>\n"
		"              <th colspan=\"9\">���������. ����� \""; file << CT2A(bookName);
	file << "\". ���� \""; file << CT2A(sheet->name); file << "\".</th>\n"
		"          </tr>\n"
		"          <tr>\n"
		"              <th>������</th>\n"
		"              <th>������������ �������</th>\n"
		"              <th>�������� ����������� ��������� / �������</th>\n"
		"              <th>������� ���������</th>\n"
		"              <th>����������� ��������</th>\n"
		"              <th>������������ ��������</th>\n"
		"              <th>���� �������� �������</th>\n"
		"              <th>������������ �������</th>\n"
		"              <th>����������</th>\n"
		"          </tr>\n";

	int iRow = 1;
	for (size_t i = 0; i < db.size(); i++) {
		file << "          <tr>\n"
			"              <th rowspan=\"2\"> &nbsp "; file << iRow; file << "</th>\n";
	}
}

/*// �������� ������� ������
void CReport::writeError(ofstream& file, const vector <errorSignal>& error, const CString& folder) {
	bool pathDirectory = true;
	CString colspan;

	(folder.Compare(_T("Warning")) == 0) ? colspan = "colspan=\"3\"" : colspan = "";

	for (size_t iSheets = 0; iSheets < error.sheets.size(); iSheets++) { // ����� �� ������
		if (error.sheets[iSheets].signals.size() > 0) {
			// �������� ���������� ��� �����, ���� �� ���
			if (pathDirectory) {
				pathDirectory = false;

				CString tPath = _T("");
				tPath.Format(_T("%s\\%s\\%s"), path, folder, error.name);
				CreateDirectory(tPath, NULL);

				file << "              <td "; file << CT2A(colspan); file << " align=\"center\">\n"
					"                 <dl>\n";
			}

			// ��������� ���������� � ������� �������� ������
			CString fullPath = _T("");	// ������ ���� �������� ������
			CString pathHref = _T("");
			CString buffer = _T("");
			pathHref.Format(_T("%s\\%s\\%s.html"), folder, error.name, error.sheets[iSheets].name);
			buffer.Format(_T("                     <dt><a href=\"%s\">%d</a></dt>\n"), pathHref, error.sheets[iSheets].signals.size());
			file << CT2A(buffer);

			fullPath.Format(_T("%s\\%s"), path, pathHref);

			// �������� �����
			ofstream tempFile;
			tempFile.open(fullPath);

			// ������ 
			tempFile << "<html>\n"
				"  <head>\n"
				"     <meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n"
				"     <title>��������� �� ����� \""; tempFile << CT2A(error.name); tempFile << "\"</title>\n"
				"  </head>\n"
				"  <body>\n"
				"      <table border=\"1\"cellspacing=\"0\">\n"
				"          <tr>\n"
				"              <th rowspan=\"2\">� ���������</th>\n" 
				"              <th colspan=\"9\">���������. ����� \""; tempFile << CT2A(error.name);
			tempFile << "\". ���� \""; tempFile << CT2A(error.sheets[iSheets].name); tempFile << "\".</th>\n"
				"          </tr>\n"
				"          <tr>\n"
				"              <th>������</th>\n"
				"              <th>������������ �������</th>\n"
				"              <th>�������� ����������� ��������� / �������</th>\n"
				"              <th>������� ���������</th>\n"
				"              <th>����������� ��������</th>\n"
				"              <th>������������ ��������</th>\n"
				"              <th>���� �������� �������</th>\n"
				"              <th>������������ �������</th>\n"
				"              <th>����������</th>\n"
				"          </tr>\n";

			// ����� �� �������
			int iRow = 1;
			for (list<errorSignalData>::iterator it = error.sheets[iSheets].signals.begin(); it != error.sheets[iSheets].signals.end(); it++, iRow++) {
				tempFile << "          <tr>\n"
					"              <th rowspan=\"2\"> &nbsp "; tempFile << iRow; tempFile << "</th>\n";
				
				int indxErr = getIndexErrorField(it);
				
				hightlightError(tempFile, indxErr, 0, it->sNumWordField);	// ������������ ����� �����

				buffer.Empty();
				buffer.Format(_T("              <td >  &nbsp  %s</td>\n"), it->sTitleParamField[0]);
				tempFile << CT2A(buffer);

				hightlightError(tempFile, indxErr, 1, it->sTitleParamField[1]);	// ������������ �������������

				buffer.Empty();
				buffer.Format(_T("              <td align=\"center\"> &nbsp %s</td>\n"), it->sDimensionField);
				tempFile << CT2A(buffer);

				// ������������
				hightlightError(tempFile, indxErr, 2, it->sMinMaxCsrValField[0]);	// ���
				hightlightError(tempFile, indxErr, 3, it->sMinMaxCsrValField[1]);	// ����
				hightlightError(tempFile, indxErr, 4, it->sMinMaxCsrValField[2]);	//���
				hightlightError(tempFile, indxErr, 5, it->sBitField);				// ����
				hightlightError(tempFile, indxErr, 6, it->sCommentField);			// �������

				tempFile << "          </tr>\n"
					"          <tr>\n";

				// ������ �������� ������
				list<CString>::iterator itP = it->sErrorField.begin();
				buffer.Empty();
				buffer.Format(_T("              <td colspan=\"9\" bgcolor = \"#FDFCD0\"> &nbsp %s\n"), *itP);
				tempFile << CT2A(buffer);

				if (it->sErrorField.size() > 1) {
					itP++;
					tempFile << "                  <ul>\n";
					for (; itP != it->sErrorField.end(); itP++) {
						buffer.Empty();
						buffer.Format(_T("                  <li>%s</li>\n"), *itP);
						tempFile << CT2A(buffer);
					}
					tempFile << "                  </ul>\n";
				}
				tempFile << "              </td>\n"
					"          </tr>\n"
					"          <tr>\n"
					"              <td colspan=\"10\"> &nbsp </td>\n"
					"          </tr>\n";
			}
			tempFile << "      </table>\n"
				"  </body>\n"
				"</html>\n";

			tempFile.close();	// ��������
		}
		else {
			if (pathDirectory) {
				pathDirectory = false;

				CString tPath = _T("");
				tPath.Format(_T("%s\\%s\\%s"), path, folder, error.name);
				CreateDirectory(tPath, NULL);

				file << "              <td "; file << CT2A(colspan); file << " align=\"center\">\n"
					"                 <dl>\n";
			}
			file << "                     <dt>-</dt>\n";
		}
	}
	file << "                 </dl>\n"
		"              </td>\n";
}
*/
//��������� ���������� ���������
void CReport::hightlightError(ofstream& file, int tIndx, int Indx, CString field) {
	CString buffer = _T("");
	CString color = _T("bgcolor = \"#FDFCD0\"");
	buffer.Format(_T("              <td align=\"center\" %s> &nbsp  %s</td>\n"), (tIndx == Indx) ? color : _T(""), field);
	file << CT2A(buffer);
}

// ����� ������� ���������� ���������
int CReport::getIndexErrorField(list<errorSignalData>::iterator it) {
	int result = -1;
	CString tab = *it->sErrorField.begin();

	for (int i = 0; i < 7; i++)
		if (tab.Compare(errRemarks[i]) == 0)
			result = i;

	return result;
}

// ��������� ���������� ������ ������ (�����, � ��������, ���)
void CReport::setAmount(list <bookData>& books) {
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++) {
		amount.all += it->sheets.size();
		for (size_t j = 0; j < it->sheets.size(); j++)
			if (it->sheets[j].error)
				amount.withError++;
	}
	amount.withoutError = amount.all - amount.withError;
}

void CReport::setAmountError(list <errorSet>& db) {
	for (list <errorSet>::iterator it = db.begin(); it != db.end; it++)
		for (size_t i = 0; i < it->set.size(); i++) {
			it->set[i].simantic
		}
}

#pragma endregion

#pragma region GenerateTxt

// ������ ��������� txt ������
void CReport::getTxt(list <bookData>& books, const CString& pathToSave, const bool& bNumPK) {
	// �������� ����������
	path = pathToSave;
	path.Format(_T("%s\\Text"), pathToSave);
	CreateDirectory(path, NULL);

	CString filePath = path;
	filePath.Format(_T("%s\\_ProtocolMain.txt"), pathToSave);

	// �������� �������� ����� ��� ������
	ofstream mainFile;
	mainFile.open(filePath);

	// ����� �� ������
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++) {
		// ����� �� ���������
		for (size_t i = 0; i < it->sheets.size(); i++) {
			if (!it->sheets[i].error) { // ���� ��� �� �������� ������
				ofstream tmpFile;
				CString name;
				sheetInfo info;

				info.np = it->sheets[i].np;
				info.pk = it->sheets[i].pk;
				info.bPK = bNumPK;

				// �������� txt ����� ��� ������
				name.Format(_T("NP_%d_%s.txt"), info.np, it->sheets[i].name);
				filePath.Format(_T("%s\\%s"), path, name);
				tmpFile.open(filePath);
				mainFile << "#include \""; mainFile << CT2A(name); mainFile << "\"\n";

				for (size_t j = 0; j < it->sheets[i].signals.size(); j++) 
					writeTxtParam(tmpFile, it->sheets[i].signals[j], info);	// ������ ���������

				tmpFile.close();
			}
			else continue;
		}
	}
	mainFile.close();
}

// ������ ������� � txt ����
void CReport::writeTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info) {
	CString buffer = signal.title[0];

	if (buffer.Find(RESERVE_SIGNAL) == -1) {
		buffer.Format(_T("PAR=%s\n"), signal.title[1]);	// ������ ����������� �������
		file << CT2A(buffer);

		buffer.Replace(_T("\""), _T("\\\""));
		buffer.Format(_T("\t NAME=\"%s\"\n"), signal.title[0]); // ������������ �������
		file << CT2A(buffer);

		if (!signal.dimension.IsEmpty()) {
			buffer.Format(_T("\t UNIT=\"%s\"\n"), signal.dimension);	// �����������
			file << CT2A(buffer);
		}

		// ��������� ������
		if (info.pk != 0 && info.bPK)
			buffer.Format(_T("\t SET=%d:%d\n"), info.pk, info.np);
		else
			buffer.Format(_T("\t SET=%d\n"), info.np);
		file << CT2A(buffer);

		int max = (signal.bit[1] == 0 ? signal.bit[0] : signal.bit[1]);

		// ������ ������ ����� � �����
		if (signal.numWord.size() == 2) { 
			file << "\t MERGE\n";
			buffer.Format(_T("\t\t WDADDR = %d,%d,%d\n"), signal.numWord[0], signal.bit[0], max);
			file << CT2A(buffer);
			buffer.Format(_T("\t\t WDADDR = %d,%d,%d\n"), signal.numWord[1], signal.bit[0], max);
			file << CT2A(buffer);
			file << "\t END_MERGE\n";
		}
		else {
			buffer.Format(_T("\t\t WDADDR = %d,%d,%d\n"), signal.numWord[0], signal.bit[0], max);
			file << CT2A(buffer);
		}

		// ���, ����, ���
		if (signal.max != signal.csr != DBL_MIN) { 
			file << "\t VALDESCR\n";
			file << (signal.bitSign ? "\t\t SIGNED\n" : "\t\t UNSIGNED\n");

			buffer.Format(_T("\t\t MIN = %lf\n"), signal.min);
			file << CT2A(buffer);

			buffer.Format(_T("\t\t MAX = %lf\n"), signal.max);
			file << CT2A(buffer);

			buffer.Format(_T("\t\t MSB = %lf\n"), signal.csr);
			file << CT2A(buffer);

			file << "\t END_VALDESCR\n";
		}

		// ������ ������������
		if (!signal.comment.IsEmpty()) {	
			file << "\t COMMENT\n";

			buffer = signal.comment;
			buffer.Replace(_T("\""), _T("\\\""));
			buffer.Replace(_T("\n"), _T("\"\n\t\t\""));
			buffer.Format(_T("\t\t\"%s\"\n"), buffer);

			file << CT2A(buffer);
			file << "\t END_COMMENT\n";
		}
		file << "END_PAR \n\n";
	}
}

#pragma endregion
