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
// ��������� ������ �� �������
void CReport::GetReport(pivData& data, const bool& isProj) 
{
	if (settings.GetPath().IsEmpty())
		throw EmptyPathException();

	logger >> L"�������� ������ � ���������� �������...";
	this->isProject = isProj;

	CString tPath;	// ���� � �������� �����

	// �������� � �������� �����
	isProject ? tPath.Format(L"%s%s", settings.GetPath(), settings.folders[project]) :
		tPath.Format(L"%s%s", settings.GetPath(), settings.folders[other]);
	CreateDirectory(tPath, NULL);
	CreateMainFile(data, tPath);
	logger >> L"�������� ������ � ���������� ������� ���������";
}

// �������� ��������� �����
void CReport::CreateMainFile(pivData& data, CString path)
{
	logger >> L"�������� �������� �����-�����...";
	CreateFolders(path);
	path.Format(L"%s\\%s", path, REPORT_NAME);
	CStdioFile file(path, CFile::modeCreate | CFile::modeWrite | CFile::typeUnicode);	// ����� ������ � ����
	file.WriteString(L"<html>\n"
		"\t<head>\n"
		"\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n"
		"\t\t<title>���������</title>\n"
		"\t</head>\n"
		"\t<body>\n");
	InfoTable(file, SetAmount(data));
	MainTable(file, data.db);
	file.WriteString(L"\t</body>\n"
		"</html>\n");
	file.Close();
}

// �������� ���������� ��� ������
void CReport::CreateFolders(const CString& path)
{
	CString folder = path;
	folder.Format(L"%s%s", path, settings.folders[error]);
	CreateDirectory(folder, NULL);
	folder.Format(L"%s%s", path, settings.folders[syntax]);
	CreateDirectory(folder, NULL);
	folder.Format(L"%s%s", path, settings.folders[simantic]);
	CreateDirectory(folder, NULL);
	folder.Format(L"%s%s", path, settings.folders[warning]);
	CreateDirectory(folder, NULL);
}

// ������ ������ ���������
void CReport::MainTable(CStdioFile& file, list <errorData>& db) 
{
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
	for (list <errorData>::iterator it = db.begin(); it != db.end(); it++) 
	{
		file.WriteString(L"\t\t\t<tr>\n"
			"\t\t\t\t<td>" + it->book->name + L"</td>\n"
			"\t\t\t\t<td align=\"center\">\n"
			"\t\t\t\t\t<dl>\n");
		WriteBook(file, it);	// ������ ������
		logger >> L"������ ����� �� ��������� \"" + it->book->name + L"\"";
		file.WriteString(L"\t\t\t</tr>\n");
	}
	file.WriteString(L"\t\t</table>\n");
}

// ������� ����� ���������� � ������� ������
void CReport::InfoTable(CStdioFile& file, const amountInfo& amount)
{
	file.WriteString(L"\t\t<h3>����������.</h3>\n"
		"\t\t<table border=\"1\"cellspacing=\"0\">\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">���������� ������ �����</th>\n"
		"\t\t\t\t<td align=\"center\">" + IntToCString(amount.all) + L"</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">���������� ������ � ��������</th>\n"
		"\t\t\t\t<td align=\"center\">" + IntToCString(amount.withError) + L"</td> \n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">���������� ������ ��� ������</th>\n"
		"\t\t\t\t<td align=\"center\">" + IntToCString(amount.withoutError) + L"</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">����� ������</th>\n"
		"\t\t\t\t<td align=\"center\">" + IntToCString(amount.error) + L"</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">����� ��������������</th>\n"
		"\t\t\t\t<td align=\"center\">" + IntToCString(amount.warning) + L"</td>\n"
		"\t\t\t</tr>\n"
		"\t\t</table>\n"
		"\t\t<br/>\n");
}

// ������ ������ �������� ���������
void CReport::WriteBook(CStdioFile& file, list <errorData>::iterator& it) 
{
	// ������������ ����� �������
	for (size_t i = 0; i < it->set.size(); i++)
	{
		file.WriteString(L"\t\t\t\t\t\t<dt>" + it->set[i].data->name + L"</dt>\n");
	}
	file.WriteString(L"\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n");
	
	// ������ �������������� ������
	file.WriteString(L"\t\t\t\t<td align=\"center\">\n"
		"\t\t\t\t\t<dl>\n"); 
	for (size_t i = 0; i < it->set.size(); i++)
		file.WriteString(WriteSheets(it->set[i].data, it->set[i].syntax, settings.folders[syntax], it->book->name));

	file.WriteString(L"\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n");
	// ������ ������������� ������
	file.WriteString(L"\t\t\t\t<td align=\"center\">\n"
		"\t\t\t\t\t<dl>\n");
	for (size_t i = 0; i < it->set.size(); i++)
		file.WriteString(WriteSheets(it->set[i].data, it->set[i].simantic, settings.folders[simantic], it->book->name));

	file.WriteString(L"\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n");
	// ������ ���������
	file.WriteString(L"\t\t\t\t<td colspan=\"3\" align=\"center\">\n"
		"\t\t\t\t\t<dl>\n");
	for (size_t i = 0; i < it->set.size(); i++)
		file.WriteString(WriteSheets(it->set[i].data, it->set[i].warning, settings.folders[warning], it->book->name));

	file.WriteString(L"\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n");
}

// ������ ������� � �����
CString CReport::WriteSheets(sheetData* sheet, const vector <errorSignal>& db, const CString& folder, const CString& bookName) 
{
	CString pathFile;
	isProject ? pathFile.Format(L"%s%s%s\\%s", settings.GetPath(), settings.folders[project], folder, bookName) : 
		pathFile.Format(L"%s%s%s\\%s", settings.GetPath(), settings.folders[other], folder, bookName);
	CreateDirectory(pathFile, NULL);
	
	CString result;	// �������������� ������ ��� ������ ������ � ������� ����
	int count = CountError(db);

	if (count > 0) 
	{
		pathFile.Format(L"%s\\%s.html", pathFile, sheet->name);
		CString relativePath = pathFile;
		relativePath.Delete(0, settings.GetPath().GetLength());
		isProject ? relativePath.Delete(0, settings.folders[project].GetLength()) : relativePath.Delete(0, settings.folders[other].GetLength());
		relativePath.Insert(0, L".");
		result.Format(L"\t\t\t\t\t\t<dt><a href=\"%s\">%d</a></dt>\n", relativePath, count);	// ������������ �������������� ������ (������)
		
		CStdioFile file(pathFile, CFile::modeCreate | CFile::modeWrite | CFile::typeUnicode);
		// �������
		file.WriteString(L"<!DOCTYPE html>\n"
			"<html>\n"
			"<head>\n"
			"\t<meta charset=\"UTF-8\">\n"
			"\t<title>��������� �� ����� \"" + bookName + L"\"</title>\n");
		CssStyle(file);
		file.WriteString(L"</head>\n"
			"<body>\n"
		);

		SheetTableHeader(file, bookName, sheet->name, sheet->arinc);
		file.WriteString(L"\t\t<tbody>\n");
		// ������ ������� ������
		for (size_t i = 0; i < db.size(); i++)
		{
			// ������ ������
			file.WriteString(L"\t\t\t<tr>\n"
				"\t\t\t\t<th rowspan=\"2\"> &nbsp " + IntToCString((int)i + 1) + L"</th>\n");
			WriteSignal(file, db[i]);
		}

		file.WriteString(L"\t\t</tbody>\n"
			"\t</table>\n"
			"</body>\n"
			"</html>");
		file.Close();
	}
	else
		result.Format(L"\t\t\t\t\t\t<dt>-</dt>\n"); // ������������ �������������� ������
	return result;
}

// ����� ������� � �������� �����
void CReport::SheetTableHeader(CStdioFile& file, const CString& book, const CString& sheet, bool arinc)
{
	file.WriteString(L"\t<h1> ���������. ����� \"" + book + L"\". ���� \"" + sheet + L"\". </h1>\n"
		"\t<table>\n"
		"\t\t<thead>\n"
		"\t\t<tr>\n"
		"\t\t\t<th style=\"width: 5%\">� ���������</th>\n");
	arinc ? file.WriteString(L"\t\t\t<th style=\"width: 5%\">�����</th>\n") : file.WriteString(L"\t\t\t<th style=\"width: 5%\">������</th>\n");
	file.WriteString(L"\t\t\t<th style=\"width: 15%\">������������ �������</th>\n"
		"\t\t\t<th style=\"width: 10%\">�������� ����������� ��������� / �������</th>\n"
		"\t\t\t<th style=\"width: 5%\">������� ���������</th>\n"
		"\t\t\t<th style=\"width: 5%\">����������� ��������</th>\n"
		"\t\t\t<th style=\"width: 5%\">������������ ��������</th>\n"
		"\t\t\t<th style=\"width: 5%\">���� �������� �������</th>\n"
		"\t\t\t<th style=\"width: 10%\">������������ �������</th>\n"
		"\t\t\t<th style=\"width: 30%\">����������</th>\n"
		"\t\t</tr>\n"
		"\t\t</thead>\n");
}

// ������ CSS �����
void CReport::CssStyle(CStdioFile& file)
{
	// TODO: ��������� ������ � ��������� ������
	file.WriteString(L"\t<style type=\"text/css\">\n"
		"\t\ttable {\n"
		"\t\t\twidth: 100%;\n"
		"\t\t\tborder-collapse: collapse;\n"
		"\t\t}\n"

		"\t\tthead {\n"
		"\t\t\tdisplay: block;\n"
		"\t\t\twidth: 100%;\n"
		"\t\t\toverflow: auto;\n"
		"\t\t\tcolor: #fff;\n"
		"\t\t\tbackground: #000;\n"
		"\t\t}\n"

		"\t\ttbody {\n"
		"\t\t\tdisplay: block;\n"
		"\t\t\twidth: 100%;\n"
		"\t\t\theight: 200px;\n"
		"\t\t\tborder: 1px;\n"
		"\t\t\tborder-color: grey;\n"
		"\t\t\tbackground: white;\n"
		"\t\t\toverflow: auto;\n"
		"\t\t}\n"

		"\t\tth, td {\n"
		"\t\t\tpadding: .5em 1em;\n"
		"\t\t\ttext-align: left;\n"
		"\t\t\tvertical-align: top;\n"
		"\t\t\tborder-left: 1px solid #fff;\n"
		"\t\t}\n"
		"\t</style>\n"
	);
}

// ������ ������ ������ ���������� �������
void CReport::WriteSignal(CStdioFile& file, const errorSignal& signal) 
{
	file.WriteString(FormValue(signal.data->numWord.field, signal.check[check::numword], 5));
	CString buffer;
	buffer.Format(L"\t\t\t\t<td align=\"center\" style=\"width: 15%%\"> &nbsp %s</td>\n", signal.data->title[0]);
	file.WriteString(buffer);

	file.WriteString(FormValue(signal.data->title[1], signal.check[check::title], 10));
	buffer.Format(L"\t\t\t\t<td align=\"center\" style=\"width: 5%%\"> &nbsp %s</td>\n", signal.data->dimension);
	file.WriteString(buffer);

	file.WriteString(FormValue(signal.data->min.field, signal.check[check::min], 5));
	file.WriteString(FormValue(signal.data->max.field, signal.check[check::max], 5));
	file.WriteString(FormValue(signal.data->csr.field, signal.check[check::csr], 5));

	file.WriteString(FormValue(signal.data->bit.field, signal.check[check::bits], 10));
	file.WriteString(FormValue(signal.data->comment, signal.check[check::comment], 30));

	file.WriteString(L"\t\t\t</tr>\n"
		"\t\t\t<tr>\n");

	// ������ ���� ������
	file.WriteString(L"\t\t\t\t<td style=\"padding-left: 20; padding-top: 0; padding-bottom: 15\" colspan=\"10\" bgcolor = \"#FDFCD0\">\n");
	for (size_t j = 0; j < signal.error.size(); j++) 
	{
		buffer.Format(L"\t\t\t\t\t\t<p style=\"margin-left: 20px\"> � %s</p>\n", signal.error[j]);
		file.WriteString(buffer);
	}
	file.WriteString(L"\t\t\t\t</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<td colspan=\"10\"> &nbsp </td>\n"
		"\t\t\t</tr>\n");
}

// ������������ ������ ��������� �� �������
CString CReport::FormValue(CString field, const bool& color, const int& width) 
{
	CString result;
	field.Replace(L"\n", L" <br> ");
	color ? result = L"bgcolor = \"#FDFCD0\"" : result = L"";
	result.Format(L"\t\t\t\t<td align=\"center\" %s style=\"width: %d%%\"> %s</td>\n", result, width, field);
	return result;
}

// �������������� int to CString
CString CReport::IntToCString(const int& number)
{
	CString result;
	result.Format(L"%d", number);
	return result;
}

// �������������� ������ � utf8
/*string CReport::ToUtf8(const CString& text)
{
	int nChars = ::WideCharToMultiByte(CP_UTF8, 0, text, (int)text.GetLength(), NULL, 0, NULL, NULL);
	if (nChars == 0) 
		return "";
	string result;
	result.resize(nChars);
	::WideCharToMultiByte(CP_UTF8, 0, text, (int)text.GetLength(), const_cast< char* >(result.c_str()), nChars, NULL, NULL);

	return result;
}*/

// ���������� ������ � ������� �������
int CReport::CountError(const vector<errorSignal>& signal)
{
	int result = 0;
	for (size_t i = 0; i < signal.size(); i++)
		result += (int) signal[i].error.size();
	return result;
}

// ������� ���������� ������( �����, � �������� � ��� ������), ������ � ��������������
amountInfo CReport::SetAmount(pivData& data)
{
	amountInfo result;
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
	{
		result.all += (int)it->sheets.size();
		for (size_t j = 0; j < it->sheets.size(); j++)
			if (it->sheets[j].error)
				result.withError++;
	}
	result.withoutError = result.all - result.withError;

	for (list<errorData>::iterator it = data.db.begin(); it != data.db.end(); it++)
	{
		for (size_t i = 0; i < it->set.size(); i++)
		{
			result.error += CountError(it->set[i].syntax) + CountError(it->set[i].simantic);
			result.warning += CountError(it->set[i].warning);
		}
	}
	return result;
}
#pragma endregion

// ��������� txt ������
#pragma region GENERATE_TXT
// ������ 
void CReport::GetTxt(const bookData& book)
{
	if (settings.GetPath().IsEmpty())
		throw EmptyPathException();
	
	CString path;
	path.Format(L"%s%s", settings.GetPath(), settings.folders[text]);
	CreateDirectory(path, NULL);

	Generate(book);
}

// ����������
void CReport::GetTxt(list <bookData>& books) 
{
	if (settings.GetPath().IsEmpty())
		throw EmptyPathException();

	CString path;
	path.Format(L"%s%s", settings.GetPath(), settings.folders[text]);
	CreateDirectory(path, NULL);

	// ����� �� ������
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++)
		Generate(*it);
}

// ��������� txt ����������
void CReport::Generate(const bookData& book) 
{
	logger >> L"�������� txt-������ ��������� \"" + book.name + L"\"...";
	CString tPath = settings.GetPath();
	tPath.Format(L"%s%s\\%s", tPath, settings.folders[text], book.name);
	CreateDirectory(tPath, NULL);

	CString filePath = settings.GetPath();
	filePath.Format(L"%s\\_ProtocolMain.txt", tPath);
	
	// ����������� �������� ��� ��������� ��������� ������
	settings.GetGenTxt() ? logger >> L"�������� txt-������ ��� ������� ������ �� ����� - ��������" : 
		logger >> L"�������� txt-������ ��� ������� ������ �� ����� - ���������";
	settings.GetNumPk() ? logger >> L"��������� ������ �������� ��� ������ - ��������" : 
		logger >> L"��������� ������ �������� ��� ������ - ���������";

	// �������� �������� ����� ��� ������
	ofstream mainFile;
	mainFile.open(filePath);

	// ����� �� ���������
	int cNP = 1, cNumWord = 1;	// ��������� �������� ��� arinc ��������
	for (size_t i = 0; i < book.sheets.size(); i++, cNP++, cNumWord = 1) 
	{
		// ���� ��� �� �������� ������
		if (!book.sheets[i].error || settings.GetGenTxt()) 
		{ 
			CString name;
			sheetInfo info;

			info.arinc = book.sheets[i].arinc;
			info.arinc ? info.np = cNP : info.np = book.sheets[i].np;
			info.pk = book.sheets[i].pk;
			info.name = book.sheets[i].name;

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
				WriteTxtParam(tmpFile, book.sheets[i].signals[j], info, cNumWord);	// ������ ���������
			}
			tmpFile.close();
		}
		else 
			logger >> L"Txt-���� ����� \"" + book.sheets[i].name + L"\" �� ������, �� ����� ������� ������";
	}
	mainFile.close();
	logger >> L"�������� txt-������ ��������� \"" + book.name + L"\" ���������";
}

// ������ ������ ������ ������ �� ������� � txt ����
void CReport::WriteTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info, const int& arincNum)
{
	CString buffer = signal.title[0];

	if (buffer.Find(RESERVE_SIGNAL) != -1 || (settings.GetProject() == project::kprno35 && dwPart::checkLow(buffer)))
		return;

	bool dwKprno = WriteParamTitle(file, signal, info);
	if (!signal.dimension.IsEmpty()) 
	{
		buffer.Format(L"\tUNIT=\"%s\"\n", signal.dimension);	// �����������
		file << CT2A(buffer);
	}

	// ��������� ������
	info.pk > 0 && settings.GetNumPk() ? buffer.Format(L"\tSET=%d:%d\n", info.pk, info.np) : buffer.Format(L"\tSET=%d\n", info.np);
	file << CT2A(buffer);
	
	// ��������� ������ ���������
	paramInfo par;
	par.arinc = info.arinc;
	par.arincNum = arincNum;
	par.dwKprno = dwKprno;
	WriteParamAdr(file, signal, par);

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

// ������ ������������ ���������
bool CReport::WriteParamTitle(ofstream& file, const signalData& signal, const sheetInfo& info)
{
	CString buffer = signal.title[1];
	buffer.Remove(L' ');
	if (signal.repWord)	// �������� ����������� � ������ ������, ���������� �������� � ��� ����� ��������
	{
		CString pk;
		pk.Format(L"%d", info.pk);
		if (info.pk == PK_EMPTY || info.pk == PK_FAILED)
			logger >> L"�������� " + signal.title[1] + L" (���� \"" + info.name + L"\") � ���������  �� �������� ����������, �������� ������� � ID �� ������� (PK = " + pk + L")";
		else
		{
			buffer.Format(L"%s_%d", buffer, info.pk);
			logger >> L"�������� " + signal.title[1] + L" (���� \"" + info.name + L"\") � ��������� �� �������� ���������� (PK = " + pk + L")";
		}
	}

	buffer.Format(L"PAR=%s\n", buffer);	// ������ ����������� �������
	file << CT2A(buffer);

	buffer = signal.title[0];
	bool dwKprno = false;
	if (settings.GetProject() == project::kprno35)
		dwKprno = dwPart::deleleHight(buffer);
	buffer.Replace(L"\"", L"\\\"");
	buffer.Format(L"\tNAME=\"%s\"\n", buffer); // ������������ �������
	file << CT2A(buffer);
	return dwKprno;
}

// ������ ������ ���������
void CReport::WriteParamAdr(ofstream& file, const signalData& signal, const paramInfo& param)
{
	CString buffer;
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
	else if (param.dwKprno && signal.part != nullptr)
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
		param.arinc ? buffer.Format(L"\tWDADDR = %d,%d,%d\n", param.arincNum, signal.bit.value[0], max) :
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