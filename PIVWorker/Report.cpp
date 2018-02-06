#include "stdafx.h"
#include "Report.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// �����������
CReport::CReport()	{	}

// ����������
CReport::~CReport()	{	}

#pragma region GENERATE_REPORT
// TODO: ��������� ���� ������ �� ���������� ������ ��������
// ��������� ������ �� �������
void CReport::GetReport(pivData& data, const CString& pathToSave, const bool& isProj) 
{
	if (pathToSave.IsEmpty())
		throw EmptyPathException();

	this->isProject = isProj;
	path = pathToSave;
	// ������� ���������� ������� � ������
	SetAmount(data.books);
	SetAmountError(data.db);

	CString tPath;	// ���� � �������� �����

					// �������� � �������� �����
	isProject ? tPath.Format(L"%s%s", path, PROJECT_FOLDER) : tPath.Format(L"%s%s", path, OTHER_FOLDER);
	CreateDirectory(tPath, NULL);
	tPath.Format(L"%s\\�����.html", tPath);
	CStdioFile file(tPath, CFile::modeCreate | CFile::modeWrite | CFile::typeUnicode);	// ����� ������ � ����

	ErrorTable(file);

	StartWrite(file, data.db);

	file.WriteString(L"\t</body>\n"
		"</html>\n");
	file.Close();
}

// ������ ������ ���������
void CReport::StartWrite(CStdioFile& file, list <errorData>& Db) 
{
	// �������� ���������� ��� ������
	CString tPath, folder;
	isProject ? folder.Format(L"%s%s", path, PROJECT_FOLDER) : folder.Format(L"%s%s", path, OTHER_FOLDER);
	tPath.Format(L"%s\\Error", folder);
	CreateDirectory(tPath, NULL);
	tPath.Format(L"%s%s", folder, SYNTAX_FOLDER);
	CreateDirectory(tPath, NULL);
	tPath.Format(L"%s%s", folder, SIMANTIC_FOLDER);
	CreateDirectory(tPath, NULL);
	tPath.Format(L"%s%s", folder, WARNING_FOLDER);
	CreateDirectory(tPath, NULL);

	// ��������� �����
	file.WriteString(L"\t\t<h3>�����.</h3>\n"
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
		"\t\t\t</tr>\n");

	// ����� �� ������
	for (list <errorData>::iterator it = Db.begin(); it != Db.end(); it++) 
	{
		file.WriteString(L"\t\t\t<tr>\n"
			"\t\t\t\t<td>"); 
		file.WriteString(it->book->name); 
		file.WriteString(L"</td>\n"
			"\t\t\t\t<td align=\"center\">\n"
			"\t\t\t\t\t<dl>\n");

		WriteBook(file, it);	// ������ ������
		file.WriteString(L"\t\t\t</tr>\n");
	}
	file.WriteString(L"\t\t</table>\n");
}

// ������ ������ �������� ���������
void CReport::WriteBook(CStdioFile& file, list <errorData>::iterator& it) 
{
	// ������������ ����� �������
	for (size_t i = 0; i < it->set.size(); i++)
	{
		file.WriteString(L"\t\t\t\t\t\t<dt>");
		file.WriteString(it->set[i].data->name);
		file.WriteString(L"</dt>\n");
	}
	file.WriteString(L"\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n");
	
	// ������ �������������� ������
	file.WriteString(L"\t\t\t\t<td align=\"center\">\n"
		"\t\t\t\t\t<dl>\n");
	for (size_t i = 0; i < it->set.size(); i++)
		file.WriteString(WriteSheets(it->set[i].data, it->set[i].syntax, SYNTAX_FOLDER, it->book->name));

	file.WriteString(L"\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n");
	// ������ ������������� ������
	file.WriteString(L"\t\t\t\t<td align=\"center\">\n"
		"\t\t\t\t\t<dl>\n");
	for (size_t i = 0; i < it->set.size(); i++)
		file.WriteString(WriteSheets(it->set[i].data, it->set[i].simantic, SIMANTIC_FOLDER, it->book->name));

	file.WriteString(L"\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n");
	// ������ ���������
	file.WriteString(L"\t\t\t\t<td colspan=\"3\" align=\"center\">\n"
		"\t\t\t\t\t<dl>\n");
	for (size_t i = 0; i < it->set.size(); i++)
		file.WriteString(WriteSheets(it->set[i].data, it->set[i].warning, WARNING_FOLDER, it->book->name));

	file.WriteString(L"\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n");
}

// ������ ������� � �����
CString CReport::WriteSheets(sheetData* sheet, const vector <errorSignal>& db, const CString& folder, const CString& bookName) 
{
	CString pathFile;
	isProject ? pathFile.Format(L"%s%s%s\\%s", path, PROJECT_FOLDER, folder, bookName) : pathFile.Format(L"%s%s%s\\%s", path, OTHER_FOLDER, folder, bookName);
	CreateDirectory(pathFile, NULL);
	
	CString result;	// �������������� ������ ��� ������ ������ � ������� ����
	int count = CountError(db);

	if (count > 0) 
	{
		pathFile.Format(L"%s\\%s.html", pathFile, sheet->name);
		CString relativePath = pathFile;
		relativePath.Delete(0, path.GetLength());
		isProject ? relativePath.Delete(0, PROJECT_SIZE) : relativePath.Delete(0, OTHER_SIZE);
		relativePath.Insert(0, L".");
		result.Format(L"\t\t\t\t\t\t<dt><a href=\"%s\">%d</a></dt>\n", relativePath, count);	// ������������ �������������� ������ (������)
		
		CStdioFile file(pathFile, CFile::modeCreate | CFile::modeWrite | CFile::typeUnicode);
		// �������
		file.WriteString(L"<html>\n"
			"\t<style>"
			"\t\t.fixed {\n\t\t\tposition: fixed; \n\t\t\tbackground: white; \n\t\t\tcolor: black; \n\t\t}\n"
			"\t</style>\n"
			"\t<head>\n"
			"\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n"
			"\t\t<title>��������� �� ����� \""); 
		file.WriteString(bookName);
		file.WriteString(L"\"</title>\n"
			"\t</head>\n"
			"\t<body style=\"margin: 0px; padding: 0px\">\n"
		);/* "\t\t<table style=\"word-break: break-all;\" border=\"1\"cellspacing=\"0\" class=\"fixed\">\n"
			"\t\t\t<tr>\n"
			"\t\t\t\t<th rowspan=\"2\" style=\"width: 5%\">� ���������</th>\n"
			"\t\t\t\t<th colspan=\"9\">���������. ����� \"");
		file.WriteString(bookName);
		file.WriteString(L"\". ���� \"");
		file.WriteString(sheet->name);
		file.WriteString(L"\".</th>\n"
			"\t\t\t</tr>\n"
			"\t\t\t<tr>\n");
		sheet->arinc ? file.WriteString(L"\t\t\t\t<th style=\"width: 5%\">�����</th>\n") : file.WriteString(L"\t\t\t\t<th style=\"width: 5%\">������</th>\n");
		file.WriteString(L"\t\t\t\t<th style=\"width: 15%\">������������ �������</th>\n"
			"\t\t\t\t<th style=\"width: 10%\">�������� ����������� ��������� / �������</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">������� ���������</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">����������� ��������</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">������������ ��������</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">���� �������� �������</th>\n"
			"\t\t\t\t<th style=\"width: 10%\">������������ �������</th>\n"
			"\t\t\t\t<th style=\"width: 30%\">����������</th>\n"
			"\t\t\t</tr>\n"
			"\t\t</table>\n"*/
		file.WriteString(L"\t\t<table style=\"word-break: break-all;\" border=\"1\"cellspacing=\"0\">\n"
			"\t\t\t<tr>\n"
			"\t\t\t\t<th rowspan=\"2\" style=\"width: 5%\">� ���������</th>\n"
			"\t\t\t\t<th colspan=\"9\">���������. ����� \""); 
		file.WriteString(bookName);
		file.WriteString(L"\". ���� \""); 
		file.WriteString(sheet->name);
		file.WriteString(L"\".</th>\n"
			"\t\t\t</tr>\n"
			"\t\t\t<tr>\n");
		sheet->arinc ? file.WriteString(L"\t\t\t\t<th style=\"width: 5%\">�����</th>\n") : file.WriteString(L"\t\t\t\t<th style=\"width: 5%\">������</th>\n");
		file.WriteString(L"\t\t\t\t<th style=\"width: 15%\">������������ �������</th>\n"
			"\t\t\t\t<th style=\"width: 10%\">�������� ����������� ��������� / �������</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">������� ���������</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">����������� ��������</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">������������ ��������</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">���� �������� �������</th>\n"
			"\t\t\t\t<th style=\"width: 10%\">������������ �������</th>\n"
			"\t\t\t\t<th style=\"width: 30%\">����������</th>\n"
			"\t\t\t</tr>\n");

		// ������ ������� ������
		for (size_t i = 0; i < db.size(); i++)
		{
			file.WriteString(L"\t\t\t<tr>\n"
				"\t\t\t\t<th rowspan=\"2\"> &nbsp "); 
			file.WriteString(IntToCString((int)i + 1));
			file.WriteString(L"</th>\n");
			WriteSignal(file, db[i]);
		}

		file.WriteString(L"\t\t</table>\n"
			"\t</body>\n"
			"</html>\n");
		file.Close();
	}
	else
		result.Format(L"\t\t\t\t\t\t<dt>-</dt>\n"); // ������������ �������������� ������
	return result;
}

// ������ ������ ������ ���������� �������
void CReport::WriteSignal(CStdioFile& file, const errorSignal& set) 
{
	CString buffer;
	file.WriteString(WriteParam(set.data->numWord.field, set.check[check::numword], 5));

	buffer.Format(L"\t\t\t\t<td align=\"center\" style=\"width: 15%%\"> &nbsp %s</td>\n", set.data->title[0]);
	file.WriteString(buffer);

	file.WriteString(WriteParam(set.data->title[1], set.check[check::title], 10));
	
	buffer.Format(L"\t\t\t\t<td align=\"center\" style=\"width: 5%%\"> &nbsp %s</td>\n", set.data->dimension);
	file.WriteString(buffer);

	file.WriteString(WriteParam(set.data->min.field, set.check[check::min], 5));
	file.WriteString(WriteParam(set.data->max.field, set.check[check::max], 5));
	file.WriteString(WriteParam(set.data->csr.field, set.check[check::csr], 5));

	file.WriteString(WriteParam(set.data->bit.field, set.check[check::bits], 10));
	file.WriteString(WriteParam(set.data->comment, set.check[check::comment], 30));

	file.WriteString(L"\t\t\t</tr>\n"
		"\t\t\t<tr>\n");

	// ������ ���� ������
	file.WriteString(L"\t\t\t\t<td style=\"padding-left: 20; padding-top: 0; padding-bottom: 15\" colspan=\"10\" bgcolor = \"#FDFCD0\">\n");
	for (size_t j = 0; j < set.error.size(); j++) 
	{
		buffer.Format(L"<br/>\t\t\t\t\t\t � %s\n", set.error[j]);
		file.WriteString(buffer);
	}

	file.WriteString(L"\t\t\t\t</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<td colspan=\"10\"> &nbsp </td>\n"
		"\t\t\t</tr>\n");
}

// ������ ������ ��������� �� ������
CString CReport::WriteParam(const CString& field, const bool& color, const int& width) 
{
	CString result;
	CString tmp = field;
	tmp.Replace(L"\n", L" <br> ");
	color ? result = L"bgcolor = \"#FDFCD0\"" : result = L"";
	result.Format(L"\t\t\t\t<td align=\"center\" %s style=\"width: %d%%\"> %s</td>\n", result, width, tmp);
	return result;
}

// �������������� int to CString
CString CReport::IntToCString(const int& number)
{
	CString result;
	result.Format(L"%d", number);
	return result;
}

// ������� ����� ���������� � ������� ������ (����� amountInfo)
void CReport::ErrorTable(CStdioFile& file)
{
	file.WriteString(L"<html>\n"
		"\t<head>\n"
		"\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n"
		"\t\t<title>���������</title>\n"
		"\t</head>\n"
		"\t<body>\n"
		"\t\t<h3>����������.</h3>\n"
		"\t\t<table border=\"1\"cellspacing=\"0\">\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">���������� ������ �����</th>\n"
		"\t\t\t\t<td align=\"center\">");	
	file.WriteString(IntToCString(amount.all));	
	file.WriteString(L"</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">���������� ������ � ��������</th>\n"
		"\t\t\t\t<td align=\"center\">");	
	file.WriteString(IntToCString(amount.withError)); 
	file.WriteString(L"</td> \n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">���������� ������ ��� ������</th>\n"
		"\t\t\t\t<td align=\"center\">");	
	file.WriteString(IntToCString(amount.withoutError)); 
	file.WriteString(L"</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">����� ������</th>\n"
		"\t\t\t\t<td align=\"center\">"); 
	file.WriteString(IntToCString(amount.error));
	file.WriteString(L"</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">����� ��������������</th>\n"
		"\t\t\t\t<td align=\"center\">");	
	file.WriteString(IntToCString(amount.warning));
	file.WriteString(L"</td>\n"
		"\t\t\t</tr>\n"
		"\t\t</table>\n"
		"\t\t<br/>\n");
}

// ������� ���������� � ������� ������
#pragma region Amount
// ���������� ������ � ������� �������
int CReport::CountError(const vector<errorSignal>& set)
{
	int result = 0;
	for (size_t i = 0; i < set.size(); i++)
		result += (int) set[i].error.size();
	return result;
}

// ������� ���������� ������� ������ (�����, � ��������, ��� ������)
void CReport::SetAmount(list <bookData>& books)
{
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++) 
	{
		amount.all += (int)it->sheets.size();
		for (size_t j = 0; j < it->sheets.size(); j++)
			if (it->sheets[j].error)
				amount.withError++;
	}
	amount.withoutError = amount.all - amount.withError;
}

// ������� ������ � ��������������
void CReport::SetAmountError(list <errorData>& db) 
{
	for (list<errorData>::iterator it = db.begin(); it != db.end(); it++)
		for (size_t i = 0; i < it->set.size(); i++) 
		{
			amount.error += CountError(it->set[i].syntax) + CountError(it->set[i].simantic);
			amount.warning += CountError(it->set[i].warning);
		}
}
#pragma endregion

#pragma endregion

// ��������� txt ������
#pragma region GENERATE_TXT
// ������ 
void CReport::GetTxt(const bookData& book, const CString& pathToSave, const pivParam& params)
{
	if (pathToSave.IsEmpty())
		throw EmptyPathException();
	
	path = pathToSave;
	CreateDirectory(path, NULL);
	path.Format(L"%s%s", path, TEXT_FOLDER);
	CreateDirectory(path, NULL);

	Generate(book, params);
}

// ����������
void CReport::GetTxt(list <bookData>& books, const CString& pathToSave, const pivParam& params) 
{
	if (pathToSave.IsEmpty())
		throw EmptyPathException();

	path = pathToSave;
	CreateDirectory(path, NULL);
	path.Format(L"%s%s", path, TEXT_FOLDER);
	CreateDirectory(path, NULL);

	// ����� �� ������
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++)
		Generate(*it, params);
}

// ��������� txt ����������
void CReport::Generate(const bookData& book, const pivParam& params) 
{
	CString tPath = path;
	tPath.Format(L"%s\\%s", path, book.name);
	CreateDirectory(tPath, NULL);

	CString filePath = path;
	filePath.Format(L"%s\\_ProtocolMain.txt", tPath);

	// �������� �������� ����� ��� ������
	ofstream mainFile;
	mainFile.open(filePath);

	// ����� �� ���������
	int cNP = 1, cNumWord = 1;	// ��������� �������� ��� arinc ��������
	for (size_t i = 0; i < book.sheets.size(); i++, cNP++, cNumWord = 1) 
	{
		// ���� ��� �� �������� ������
		if (!book.sheets[i].error || params.bGenTxt) 
		{ 
			CString name;
			sheetInfo info;

			info.arinc = book.sheets[i].arinc;
			info.arinc ? info.np = cNP : info.np = book.sheets[i].np;
			info.pk = book.sheets[i].pk;
			info.bPK = params.bNumPK;

			// �������� txt ����� ��� ������
			name.Format(L"NP_%d_%s.txt", info.np, book.sheets[i].name);
			filePath.Format(L"%s\\%s", tPath, name);
			
			ofstream tmpFile(filePath);
			mainFile << "#include \"";
			mainFile << CT2A(name);
			mainFile << "\"\n";

			for (size_t j = 0; j < book.sheets[i].signals.size(); j++) 
			{
				if (j != 0 && info.arinc)
					if (book.sheets[i].signals[j].numWord.value[0] != book.sheets[i].signals[j - 1].numWord.value[0])
						cNumWord++;
				WriteTxtParam(tmpFile, book.sheets[i].signals[j], info, params.iProject, cNumWord);	// ������ ���������
			}
			tmpFile.close();
		}
		else 
			continue;
	}
	mainFile.close();
}

// ������ ������ ������ ������ �� ������� � txt ����
void CReport::WriteTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info, const int& iProject, const int& arincNum)
{
	CString buffer = signal.title[0];

	if (buffer.Find(RESERVE_SIGNAL) != -1 || (iProject == project::kprno35 && dwPart::checkLow(buffer)))
		return;

	buffer = signal.title[1];
	buffer.Remove(L' ');
	if (signal.repWord && info.pk != -1)
		buffer.Format(L"%s_%d", buffer, info.pk);
	buffer.Format(L"PAR=%s\n", buffer);	// ������ ����������� �������
	file << CT2A(buffer);

	buffer = signal.title[0];
	bool dwKprno = false;
	if (iProject == project::kprno35)
		dwKprno = dwPart::deleleHight(buffer);
	buffer.Replace(L"\"", L"\\\"");
	buffer.Format(L"\tNAME=\"%s\"\n", buffer); // ������������ �������
	file << CT2A(buffer);

	if (!signal.dimension.IsEmpty()) 
	{
		buffer.Format(L"\tUNIT=\"%s\"\n", signal.dimension);	// �����������
		file << CT2A(buffer);
	}

	// ��������� ������
	info.pk > 0 && info.bPK ? buffer.Format(L"\tSET=%d:%d\n", info.pk, info.np) : buffer.Format(L"\tSET=%d\n", info.np);
	file << CT2A(buffer);

	int max = (signal.bit.value[1] == -1 ? signal.bit.value[0] : signal.bit.value[1]);

	// ������ ������ ����� � �����
	if (signal.numWord.value.size() == 2)
	{ 
		file << "\tMERGE\n";
		buffer.Format(L"\t\tWDADDR = %d,%d,%d\n", signal.numWord.value[0], signal.bit.value[0], max);
		file << CT2A(buffer);

		max = (signal.bit.value[3] == -1) ? signal.bit.value[2] : signal.bit.value[3];
		buffer.Format(L"\t\tWDADDR = %d,%d,%d\n", signal.numWord.value[1], signal.bit.value[2], max);
		file << CT2A(buffer);
		file << "\tEND_MERGE\n";
	}
	else if (dwKprno && signal.part != nullptr)
	{
		file << "\tMERGE\n";
		buffer.Format(L"\t\tWDADDR = %d,%d,%d\n", signal.numWord.value[0], signal.bit.value[0], max);
		file << CT2A(buffer);

		max = (signal.part->bit.value[1] == -1) ? signal.part->bit.value[0] : signal.part->bit.value[1];
		buffer.Format(L"\t\tWDADDR = %d,%d,%d\n", signal.part->numWord.value[0], signal.part->bit.value[0], max);
		file << CT2A(buffer);
		file << "\tEND_MERGE\n";
	}
	else
	{
		info.arinc ? buffer.Format(L"\tWDADDR = %d,%d,%d\n", arincNum, signal.bit.value[0], max) :
			buffer.Format(L"\tWDADDR = %d,%d,%d\n", signal.numWord.value[0], signal.bit.value[0], max);
		file << CT2A(buffer);
	}

	// ���, ����, ���
	if (signal.max.value != DBL_MIN && signal.csr.value != DBL_MIN) 
	{ 
		file << "\tVALDESCR\n";
		signal.bitSign ? file << "\t\tSIGNED\n" : file << "\t\tUNSIGNED\n";

		IsInt(signal.min.value) ? buffer.Format(L"\t\tMIN = %.0lf\n", signal.min.value) : buffer.Format(L"\t\tMIN = %lf\n", signal.min.value);
		file << CT2A(buffer);

		IsInt(signal.max.value) ? buffer.Format(L"\t\tMAX = %.0lf\n", signal.max.value) : buffer.Format(L"\t\tMAX = %lf\n", signal.max.value);
		file << CT2A(buffer);
			
		IsInt(signal.csr.value) ? buffer.Format(L"\t\tMSB = %.0lf\n", signal.csr.value) : buffer.Format(L"\t\tMSB = %lf\n", signal.csr.value);
		file << CT2A(buffer);

		file << "\tEND_VALDESCR\n";
	}

	// ������ ������������
	if (!signal.comment.IsEmpty()) 
	{	
		file << "\tCOMMENT\n";

		buffer = signal.comment;
		buffer.Replace(L"\"", L"");
		buffer.Replace(L"\n", L"\"\n\t\t\"");
		buffer.Insert(0, L"\t\t\"");
		buffer.Insert(buffer.GetLength(), L"\"\n");

		file << CT2A(buffer);
		file << "\tEND_COMMENT\n";
	}
	file << "END_PAR\n\n";
}

// �������� �� int ��������
bool CReport::IsInt(const double& numeric)
{
	int whole = (int)numeric;
	double fract = numeric - whole;
	bool result;
	fract == 0 ? result = true : result = false;
	return result;
}
#pragma endregion