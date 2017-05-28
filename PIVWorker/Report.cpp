#include "stdafx.h"
#include "Report.h"

// �����������
CReport::CReport()
{
}

// ����������
CReport::~CReport()
{
	Clear();
}

// ��������� ���� ����������
void CReport::setPath(CString path) 
{ 
	this->path = path;
}

// ��������� ������
bool CReport::Generate(vector <bookData> books, errorSet errorDB)
{
	if (path.IsEmpty())	// �� ����� ���� ����������
		return false;

	this->errorDB = errorDB;
	this->books = books;
	startGenerate();	// ���������
	Clear();

	return true;
}

// ������� ����������
void CReport::Clear()
{
	errorDB.syntax.clear();
	errorDB.simantic.clear();
	errorDB.warning.clear();
}

// ������ ��������� ������
void CReport::startGenerate()
{
	// ������� ���������� ������� � ������
	amount.allSets = getSetsAmount();
	amount.setsWithError = getSetsAmountWithError();
	amount.setsWithoutError = amount.allSets - amount.setsWithError;

	amount.error = getErrorAmount(errorDB.syntax) + getErrorAmount(errorDB.simantic);
	amount.warning = getErrorAmount(errorDB.warning);

	if (amount.error + amount.warning != 0)
		makeReport();
	else
		makeEmptyReport();
}

// ��������� �� ������� ������
void CReport::makeReport()
{
	ofstream file;	// ����� ������ � ����
	CString tPath;	// ���� � �������� �����

	// �������� ���������� ��� ������
	tPath.Format(_T("%s\\ERROR"), path);
	CreateDirectory(tPath, NULL);
	tPath.Empty();
	tPath.Format(_T("%s\\WARNING"), path);
	CreateDirectory(tPath, NULL);
	tPath.Empty();
	tPath.Format(_T("%s\\ERROR\\SyntaxError"), path);
	CreateDirectory(tPath, NULL);
	tPath.Empty();
	tPath.Format(_T("%s\\ERROR\\SimanticError"), path);
	CreateDirectory(tPath, NULL);

	// �������� � �������� �����
	tPath.Empty();
	tPath.Format(_T("%s\\�����.html"), path);
	file.open(tPath);

	// ������ � ����
	file << "<html>\n";
	file << "  <head>";
	file << "     <meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n";
	file << "     <title>���������</title>\n";
	file << "  </head>\n";
	file << "  <body>\n";
	file << "      <h3>����������.</h3>\n";
	file << "      <table border=\"1\"cellspacing=\"0\">\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">������� ���������� �����</th>\n";
	file << "              <td align=\"center\">";	file << amount.allSets;	file << "</td>\n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">������� ���������� � ��������</th>\n";
	file << "              <td align=\"center\">";	file << amount.setsWithError; file << "</td> \n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">������� ���������� ��� ������</th>\n";
	file << "              <td align=\"center\">";	file << amount.setsWithoutError; file << "</td>\n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">����� ������</th>\n";
	file << "              <td align=\"center\">"; file << amount.error; file << "</td>\n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">����� ��������������</th>\n";
	file << "              <td align=\"center\">";	file << amount.warning;	file << "</td>\n";
	file << "          </tr>\n";
	file << "      </table>\n";
	file << "      <br/>\n";
	file << "      <h3>�����.</h3>\n";
	file << "      <table border=\"1\"cellspacing=\"0\">\n";
	file << "          <tr>\n";
	file << "              <th rowspan=\"2\">�������� �����</th>\n";
	file << "              <th rowspan=\"2\">�������� �����</th>\n";
	file << "              <th colspan=\"3\">������ � ���������</th>\n";
	file << "              <th rowspan=\"2\">��������������</th>\n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th>��������������</th>\n";
	file << "              <th>�������������</th>\n";
	file << "          </tr>\n";

	for (size_t cBooks = 0; cBooks < books.size(); cBooks++)
	{
		writeSheets(file, cBooks);
		//writeSyntaxError(file, cBooks);
		//writeSimanticError(file, cBooks);
		//writeWarning(file, cBooks);
		file << "          </tr>\n";
	}
	file << "      </table>\n";
	file << "  </body>\n";
	file << "</html>\n";

	file.close();	// �������� �����
}

// ��������� ������� ������
void CReport::makeEmptyReport()
{
	ofstream file;	// ����� ������ � ����
	CString tPath;	// ���� � �������� �����

	// �������� � �������� �����
	tPath.Format(_T("%s\\�����.html"), path);
	file.open(tPath);

	// ������ � ����
	file << "<html>\n";
	file << "  <head>\n";
	file << "     <meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n";
	file << "     <title>���������</title>\n";
	file << "  </head>\n";
	file << "  <body>\n";
	file << "      <h3>����������.</h3>\n";
	file << "      <table border=\"1\"cellspacing=\"0\">\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">������� ���������� �����</th>\n";
	file << "              <td align=\"center\">";	file << amount.allSets; file << "</td>\n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">������� ���������� � ��������</th>\n";
	file << "              <td align=\"center\">";	file << amount.setsWithError; file << "</td>\n ";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">������� ���������� ��� ������</th>\n";
	file << "              <td align=\"center\">";	file << amount.setsWithoutError; file << "</td>\n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">����� ������</th>\n";
	file << "              <td align=\"center\">"; file << amount.error; file << "</td>\n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">����� ��������������</th>\n";
	file << "              <td align=\"center\">";	file << amount.warning; file << "</td>\n";
	file << "          </tr>\n";
	file << "      </table>\n";
	file << "      <br/>\n";
	file << "      <h2>������ � ��������� ���!</h2>\n";
	file << "  </body>\n";
	file << "</html>\n";

	file.close();	// �������� �����
}

// �������� �������
void CReport::writeSheets(ofstream& file, int cBooks)
{
	bool pathDirectory = true;

	for (size_t cSheets = 0; cSheets < books[cBooks].sheets.size(); cSheets++)
	{
		if (books[cBooks].sheets[cSheets].bErrorSheet) // ������� �� ������ �� �����
		{
			if (pathDirectory)
			{
				pathDirectory = false;
				file << "          <tr>\n";
				file << "              <td>"; file << books[cBooks].nameBook; file << "< / td>\n";
				file << "              <td align=\"center\">\n";
				file << "                 <dl>\n";
			}
			file << "                     <dt>"; file << books[cBooks].sheets[cSheets].nameSheet; file << "< / dt>\n";
		}
	}
	file << "                 </dl>\n";
	file << "              </td>\n";
}

// ���������� ���� ������� ������
int CReport::getSetsAmount()
{
	int result = 0;

	for (size_t i = 0; i < books.size(); i++)
		result += books[i].sheets.size();

	return result;
}

// ���������� ������� ������ � �������
int CReport::getSetsAmountWithError()
{
	int result = 0;

	for (size_t i = 0; i < books.size(); i++)
		for (size_t j = 0; j < books[i].sheets.size(); j++)
			if (books[i].sheets[j].bErrorSheet)
				result++;

	return result;
}

// ���������� ������
int CReport::getErrorAmount(vector <errorBookData> error)
{
	int result = 0;

	for (size_t i = 0; i < error.size(); i++)
		for (size_t j = 0; j < error[i].sheets.size(); j++)
			result += error[i].sheets[j].signals.size();

	return result;
}
