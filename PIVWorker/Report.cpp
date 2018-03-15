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
			"\t\t\t\t<td>\n"
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
		"\t\t\t\t<td>" + IntToCString(amount.all) + L"</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">���������� ������ � ��������</th>\n"
		"\t\t\t\t<td>" + IntToCString(amount.withError) + L"</td> \n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">���������� ������ ��� ������</th>\n"
		"\t\t\t\t<td>" + IntToCString(amount.withoutError) + L"</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">����� ������</th>\n"
		"\t\t\t\t<td>" + IntToCString(amount.error) + L"</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">����� ��������������</th>\n"
		"\t\t\t\t<td>" + IntToCString(amount.warning) + L"</td>\n"
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
	file.WriteString(L"\t\t\t\t<td>\n"
		"\t\t\t\t\t<dl>\n"); 
	for (size_t i = 0; i < it->set.size(); i++)
		file.WriteString(WriteSheets(it->set[i].data, it->set[i].syntax, settings.folders[syntax], it->book->name));

	file.WriteString(L"\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n");
	// ������ ������������� ������
	file.WriteString(L"\t\t\t\t<td>\n"
		"\t\t\t\t\t<dl>\n");
	for (size_t i = 0; i < it->set.size(); i++)
		file.WriteString(WriteSheets(it->set[i].data, it->set[i].simantic, settings.folders[simantic], it->book->name));

	file.WriteString(L"\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n");
	// ������ ���������
	file.WriteString(L"\t\t\t\t<td colspan=\"3\">\n"
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
			"<html lang=\"ru\">\n"
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
				"\t\t\t\t<th rowspan=\"2\" class=width1>" + IntToCString((int)i + 1) + L"</th>\n");
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
	file.WriteString(L"\t<table>\n"
		"\t\t<caption> ���������. ����� \"" + book + L"\". ���� \"" + sheet + L"\". </caption>"
		"\t\t<thead>\n"
		"\t\t<tr>\n"
		"\t\t\t<th width=\"20px\">� �-�</th>\n");
	arinc ? file.WriteString(L"\t\t\t<th width=\"40px\">�����</th>\n") : file.WriteString(L"\t\t\t<th width=\"40px\">� ��-�</th>\n");
	file.WriteString(L"\t\t\t<th width=\"180px\">������������</th>\n"
		"\t\t\t<th width=\"150px\">�������������</th>\n"
		"\t\t\t<th width=\"60px\">��. ���.</th>\n"
		"\t\t\t<th width=\"60px\">���. ��.</th>\n"
		"\t\t\t<th width=\"60px\">����. ��.</th>\n"
		"\t\t\t<th width=\"60px\">���</th>\n"
		"\t\t\t<th width=\"100px\">������-��� �������</th>\n"
		"\t\t\t<th width=\"260px\">����������</th>\n"
		"\t\t</tr>\n"
		"\t\t</thead>\n");
}

// ������ CSS �����
void CReport::CssStyle(CStdioFile& file)
{
	// TODO: ��������� ������ � ��������� ������
	file.WriteString(L"\t<style type=\"text/css\">\n"
		"\t\tcaption {\n"
		"\t\t\tfont-family: Arial, Helvetica, sans-serif;\n"
		"\t\t\tfont-size: 15px;\n"
		"\t\t\tfont-weight: bold;\n"
		"\t\t\tpadding-bottom: 10px;\n"
		"\t\t}\n"

		"\t\ttable {\n"
		"\t\t\tfont-family: Arial, Helvetica, sans-serif;\n"
		"\t\t\tcolor: #666;\n"
		"\t\t\tfont-size: 12px;\n"
		"\t\t\ttext-shadow: 1px 1px 0px #fff;\n"
		"\t\t\tbackground: #eaebec;\n"
		"\t\t\tmargin: auto;\n"
		"\t\t\tborder: #ccc 1px solid;\n"
		"\t\t\tborder-collapse: separate;\n"
		"\t\t\t-moz-border-radius: 3px;\n"
		"\t\t\t-webkit-border-radius: 3px;\n"
		"\t\t\tborder-radius: 3px;\n"
		"\t\t\t-moz-box-shadow: 0 1px 2px #d1d1d1;\n"
		"\t\t\t-webkit-box-shadow: 0 1px 2px #d1d1d1;\n"
		"\t\t\tbox-shadow: 0 1px 2px #d1d1d1;\n"
		"\t\t}\n"

		"\t\tthead {\n"
		"\t\t\tdisplay: block;\n"
		"\t\t\twidth: 1200px ;\n"
		"\t\t\toverflow: hidden;\n"
		"\t\t}\n"
		
		"\t\ttbody {\n"
		"\t\t\tdisplay: block;\n"
		"\t\t\twidth: 1200px;\n"
		"\t\t\theight: 100%;\n"
		"\t\t\tmax-height: 700px;\n"
		"\t\t\tbackground: #fff;\n"
		"\t\t\toverflow-y: scroll;\n"
		"\t\t}\n"
			
		"\t\tth {\n"
		"\t\t\tfont-weight: bold;\n"
		"\t\t\tpadding: 15px 10px 15px 10px;\n"
		"\t\t\tborder-top: 1px solid #fafafa;\n"
		"\t\t\tborder-bottom:1px solid #e0e0e0;\n"
		"\t\t\tbackground: #ededed;\n"
		"\t\t\tbackground: -webkit-gradient(linear, left top, left bottom, from(#ededed), to(#ebebeb));\n"
		"\t\t\tbackground: -moz-linear-gradient(top,  #ededed,  #ebebeb);\n"
		"\t\t}\n"

		"\t\tth:first-child {\n"
		"\t\t\ttext-align: right;\n"
		"\t\t\tpadding-left: 0px;\n"
		"\t\t}\n"
		
		"\t\ttr:first-child th:first-child {\n"
		"\t\t\t-moz-border-radius-top-left: 3px;\n"
		"\t\t\t-webkit-border-top-left-radius: 3px;\n"
		"\t\t\tborder-top-left-radius: 3px;\n"
		"\t\t}\n"

		"\t\ttr:first-child th:last-child {\n"
		"\t\t\t-moz-border-radius-top-right: 3px;\n"
		"\t\t\t-webkit-border-top-right-radius: 3px;\n"
		"\t\t\tborder-top-right-radius: 3px;\n"
		"\t\t}\n"

		"\t\ttr {\n"
		"\t\t\ttext-align: center;\n"
		"\t\t\tpadding-left: 0px;\n"
		"\t\t}\n"

		"\t\ttr td:first-child{\n"
		"\t\t\ttext-align: left;\n"
		"\t\t\tpadding-left: 20px;\n"
		"\t\t\tborder-left: 0;\n"
		"\t\t}\n"

		"\t\ttr td{\n"
		"\t\t\tpadding: 10px 0px 10px 0px;\n"
		"\t\t\tborder-top: 1px solid #ffffff;\n"
		"\t\t\tborder-bottom: 1px solid #e0e0e0;\n"
		"\t\t\tborder-left: 1px solid #e0e0e0;\n"
		"\t\t}\n"

		"\t\ttr:last-child td{\n"
		"\t\t\tborder-bottom: 0;\n"
		"\t\t}\n"
		
		"\t\ttr:last-child td:first-child{\n"
		"\t\t\t-moz-border-radius-bottom-left: 3px;\n"
		"\t\t\t-webkit-border-bottom-left-radius: 3px;\n"
		"\t\t\tborder-bottom-left-radius: 3px;\n"
		"\t\t}\n"

		"\t\ttr:last-child td:last-child{\n"
		"\t\t\t-moz-border-radius-bottom-right: 3px;\n"
		"\t\t\t-webkit-border-bottom-right-radius: 3px;\n"
		"\t\t\tborder-bottom-right-radius: 3px;\n"
		"\t\t}\n"

		"\t\t.width1 { width: 18px; }\n"
		"\t\t.width2 { width: 60px; }\n"
		"\t\t.width3 { width: 168px; }\n"
		"\t\t.width4 { padding-left: 10px; text-align: left; width: 189px; }\n"
		"\t\t.width5 { width: 79px; }\n"
		"\t\t.width6 { width: 120px; }\n"
		"\t\t.width7 { padding-left: 10px; text-align: left; width: 255px; }\n"
		"\t</style>\n"
	);
}

// ������ ������ ������ ���������� �������
void CReport::WriteSignal(CStdioFile& file, const errorSignal& signal) 
{
	file.WriteString(FormValue(signal.data->numWord.field, signal.check[check::numword], 2));
	file.WriteString(FormValue(signal.data->title[0], false, 4));
	file.WriteString(FormValue(signal.data->title[1], signal.check[check::title], 3));
	file.WriteString(FormValue(signal.data->dimension, false, 5));
	file.WriteString(FormValue(signal.data->min.field, signal.check[check::min], 5));
	file.WriteString(FormValue(signal.data->max.field, signal.check[check::max], 5));
	file.WriteString(FormValue(signal.data->csr.field, signal.check[check::csr], 5));
	file.WriteString(FormValue(signal.data->bit.field, signal.check[check::bits], 6));
	file.WriteString(FormValue(signal.data->comment, signal.check[check::comment], 7));

	file.WriteString(L"\t\t\t</tr>\n"
		"\t\t\t<tr>\n");

	// ������ ���� ������
	file.WriteString(L"\t\t\t\t<td style=\"padding-left: 20; padding-top: 0; padding-bottom: 15\" colspan=\"10\" bgcolor = \"#FDFCD0\">\n");
	for (size_t j = 0; j < signal.error.size(); j++) 
	{
		CString buffer;
		buffer.Format(L"\t\t\t\t\t\t<p style=\"margin-left: 20px\"> � %s</p>\n", signal.error[j]);
		file.WriteString(buffer);
	}
	file.WriteString(L"\t\t\t\t</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		//"\t\t\t\t<td colspan=\"10\"> &nbsp </td>\n"
		"\t\t\t</tr>\n");
}

// ������������ ������ ��������� �� �������
CString CReport::FormValue(CString field, const bool& color, const int& width) 
{
	CString result;
	field.Replace(L"\n", L" <br> ");
	color ? result = L" bgcolor = \"#FDFCD0\"" : result = L"";
	result.Format(L"\t\t\t\t<td %s class=width%d> %s</td>\n", result, width, field);
	//result.Format(L"\t\t\t\t<td %s> %s</td>\n", result, field);
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