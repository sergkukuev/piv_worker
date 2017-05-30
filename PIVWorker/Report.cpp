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
	tPath.Format(_T("%s\\Error"), path);
	CreateDirectory(tPath, NULL);
	tPath.Empty();
	tPath.Format(_T("%s\\Warning"), path);
	CreateDirectory(tPath, NULL);
	tPath.Empty();
	tPath.Format(_T("%s\\Error\\Syntax"), path);
	CreateDirectory(tPath, NULL);
	tPath.Empty();
	tPath.Format(_T("%s\\Error\\Simantic"), path);
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

		// �������� ������ � ��������������, ������������� ������� � ���������������
		writeError(file, errorDB.syntax[cBooks], _T("Error\\Syntax"));
		writeError(file, errorDB.simantic[cBooks], _T("Error\\Simantic"));
		writeError(file, errorDB.warning[cBooks], _T("Warning"));
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
		if (!books[cBooks].sheets[cSheets].bErrorSheet) // ������� �� ������ �� �����
		{
			if (pathDirectory)
			{
				pathDirectory = false;
				file << "          <tr>\n";
				file << "              <td>"; file << CT2A(books[cBooks].nameBook); file << "</td>\n";
				file << "              <td align=\"center\">\n";
				file << "                 <dl>\n";
			}
			file << "                     <dt>"; file << CT2A(books[cBooks].sheets[cSheets].nameSheet); file << "</dt>\n";
		}
	}
	file << "                 </dl>\n";
	file << "              </td>\n";
}

// �������� ������� ������
void CReport::writeError(ofstream& file, errorBookData error, CString folder)
{
	bool pathDirectory = true;

	for (size_t iSheets = 0; iSheets < error.sheets.size(); iSheets++)	// ����� �� ������
	{
		if (error.sheets[iSheets].signals.size() > 0)
		{
			CString name = error.name;

			//name.Delete(error.name.Find(_T(".")), error.name.GetLength() - error.name.Find(_T(".")));	// ������� �������� ����� �����	(����������, ���� ��� ����� �����)

			// �������� ���������� ��� �����, ���� �� ���
			if (pathDirectory)
			{
				pathDirectory = false;

				CString tPath = _T("");
				tPath.Format(_T("%s\\%s\\%s"), path, folder, name);
				CreateDirectory(tPath, NULL);

				file << "              <td align=\"center\">\n";
				file << "                 <dl>\n";
			}

			// ��������� ���������� � ������� �������� ������
			CString fullPath = _T("");	// ������ ���� �������� ������
			CString pathHref = _T("");
			CString buffer = _T("");
			pathHref.Format(_T("%s\\%s\\%s.html"), folder, name, error.sheets[iSheets].name);
			buffer.Format(_T("                     <dt><a href=\"%s\">%d</a></dt>\n"), pathHref, error.sheets[iSheets].signals.size());
			file << CT2A(buffer);

			fullPath.Format(_T("%s\\%s"), path, pathHref);

			// �������� �����
			ofstream tempFile;
			tempFile.open(fullPath);

			// ������ 
			tempFile << "<html>\n";
			tempFile << "  <head>\n";
			tempFile << "     <meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n";

			tempFile << "     <title>��������� �� ����� \""; tempFile << CT2A(error.name); tempFile << "\"</title>\n";

			tempFile << "  </head>\n";
			tempFile << "  <body>\n";
			tempFile << "      <table border=\"1\"cellspacing=\"0\">\n";
			tempFile << "          <tr>\n";
			tempFile << "              <th rowspan=\"2\">� ���������</th>\n";

			tempFile << "              <th colspan=\"9\">���������. ����� \""; tempFile << CT2A(error.name);
			tempFile << "\". ���� \""; tempFile << CT2A(error.sheets[iSheets].name); tempFile << "\".</th>\n";

			tempFile << "          </tr>\n";
			tempFile << "          <tr>\n";
			tempFile << "              <th>������</th>\n";
			tempFile << "              <th>������������ �������</th>\n";
			tempFile << "              <th>�������� ����������� ��������� / �������</th>\n";
			tempFile << "              <th>������� ���������</th>\n";
			tempFile << "              <th>����������� ��������</th>\n";
			tempFile << "              <th>������������ ��������</th>\n";
			tempFile << "              <th>���� �������� �������</th>\n";
			tempFile << "              <th>������������ �������</th>\n";
			tempFile << "              <th>����������</th>\n";
			tempFile << "          </tr>\n";

			// ����� �� �������
			int iRow = 1;
			for (list<errorSignalData>::iterator it = error.sheets[iSheets].signals.begin(); it != error.sheets[iSheets].signals.end(); it++, iRow++)
			{
				tempFile << "          <tr>\n";
				tempFile << "              <th rowspan=\"2\"> &nbsp "; tempFile << iRow; tempFile << "</th>\n";
				
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

				tempFile << "          </tr>\n";
				tempFile << "          <tr>\n";

				// ������ �������� ������
				list<CString>::iterator itP = it->sErrorField.begin();
				buffer.Empty();
				buffer.Format(_T("              <td colspan=\"9\" bgcolor = \"#FDFCD0\"> &nbsp %s\n"), *itP);
				tempFile << CT2A(buffer);

				if (it->sErrorField.size() > 1)
				{
					itP++;
					tempFile << "                  <ul>\n";
					for (; itP != it->sErrorField.end(); itP++)
					{
						buffer.Empty();
						buffer.Format(_T("                  <li>%s</li>\n"), *itP);
						tempFile << CT2A(buffer);
					}
					tempFile << "                  </ul>\n";
				}
				tempFile << "              </td>\n";
				tempFile << "          </tr>\n";
				tempFile << "          <tr>\n";
				tempFile << "              <td colspan=\"10\"> &nbsp </td>\n";
				tempFile << "          </tr>\n";
			}
			tempFile << "      </table>\n";
			tempFile << "  </body>\n";
			tempFile << "</html>\n";

			tempFile.close();	// ��������
		}
		else
		{
			file << "              <td colspan=\"3\" align=\"center\">\n";
			file << "                 <dl>\n";
			file << "                     <dt>-</dt>\n";
		}
	}
	file << "                 </dl>\n";
	file << "              </td>\n";
}

//��������� ���������� ���������
void CReport::hightlightError(ofstream& file, int tIndx, int Indx, CString field)
{
	CString buffer = _T("");
	CString color = _T("bgcolor = \"#FDFCD0\"");
	buffer.Format(_T("              <td align=\"center\" %s> &nbsp  %s</td>\n"), (tIndx == Indx) ? color : _T(""), field);
	file << CT2A(buffer);
}

// ����� ������� ���������� ���������
int CReport::getIndexErrorField(list<errorSignalData>::iterator it)
{
	return -1;
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
			if (!books[i].sheets[j].bErrorSheet)
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
