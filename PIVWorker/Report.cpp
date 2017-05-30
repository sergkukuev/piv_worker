#include "stdafx.h"
#include "Report.h"

// Конструктор
CReport::CReport()
{
}

// Деструктор
CReport::~CReport()
{
	Clear();
}

// Установка пути сохранения
void CReport::setPath(CString path) 
{ 
	this->path = path;
}

// Генерация отчета
bool CReport::Generate(vector <bookData> books, errorSet errorDB)
{
	if (path.IsEmpty())	// Не задан путь сохранения
		return false;

	this->errorDB = errorDB;
	this->books = books;
	startGenerate();	// Генерация
	Clear();

	return true;
}

// Очистка переменных
void CReport::Clear()
{
	errorDB.syntax.clear();
	errorDB.simantic.clear();
	errorDB.warning.clear();
}

// Начало генерации отчета
void CReport::startGenerate()
{
	// Подсчет количества наборов и ошибок
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

// Генерация не пустого отчета
void CReport::makeReport()
{
	ofstream file;	// Поток записи в файл
	CString tPath;	// Путь к текущему файлу

	// Создание директорий для отчета
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

	// Создание и открытие файла
	tPath.Empty();
	tPath.Format(_T("%s\\Отчет.html"), path);
	file.open(tPath);

	// Запись в файл
	file << "<html>\n";
	file << "  <head>";
	file << "     <meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n";
	file << "     <title>Замечания</title>\n";
	file << "  </head>\n";
	file << "  <body>\n";
	file << "      <h3>Статистика.</h3>\n";
	file << "      <table border=\"1\"cellspacing=\"0\">\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">Наборов параметров всего</th>\n";
	file << "              <td align=\"center\">";	file << amount.allSets;	file << "</td>\n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">Наборов параметров с ошибками</th>\n";
	file << "              <td align=\"center\">";	file << amount.setsWithError; file << "</td> \n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">Наборов параметров без ошибок</th>\n";
	file << "              <td align=\"center\">";	file << amount.setsWithoutError; file << "</td>\n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">Всего ошибок</th>\n";
	file << "              <td align=\"center\">"; file << amount.error; file << "</td>\n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">Всего предупреждений</th>\n";
	file << "              <td align=\"center\">";	file << amount.warning;	file << "</td>\n";
	file << "          </tr>\n";
	file << "      </table>\n";
	file << "      <br/>\n";
	file << "      <h3>Отчет.</h3>\n";
	file << "      <table border=\"1\"cellspacing=\"0\">\n";
	file << "          <tr>\n";
	file << "              <th rowspan=\"2\">Название книги</th>\n";
	file << "              <th rowspan=\"2\">Название листа</th>\n";
	file << "              <th colspan=\"3\">Ошибки и замечания</th>\n";
	file << "              <th rowspan=\"2\">Предупреждения</th>\n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th>Синтаксические</th>\n";
	file << "              <th>Семантические</th>\n";
	file << "          </tr>\n";

	for (size_t cBooks = 0; cBooks < books.size(); cBooks++)
	{
		writeSheets(file, cBooks);

		// Создание отчета о синтаксических, семантических ошибках и предупреждениях
		writeError(file, errorDB.syntax[cBooks], _T("Error\\Syntax"));
		writeError(file, errorDB.simantic[cBooks], _T("Error\\Simantic"));
		writeError(file, errorDB.warning[cBooks], _T("Warning"));
		file << "          </tr>\n";
	}
	file << "      </table>\n";
	file << "  </body>\n";
	file << "</html>\n";

	file.close();	// Закрытие файла
}

// Генерация пустого отчета
void CReport::makeEmptyReport()
{
	ofstream file;	// Поток записи в файл
	CString tPath;	// Путь к текущему файлу

	// Создание и открытие файла
	tPath.Format(_T("%s\\Отчет.html"), path);
	file.open(tPath);

	// Запись в файл
	file << "<html>\n";
	file << "  <head>\n";
	file << "     <meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n";
	file << "     <title>Замечания</title>\n";
	file << "  </head>\n";
	file << "  <body>\n";
	file << "      <h3>Статистика.</h3>\n";
	file << "      <table border=\"1\"cellspacing=\"0\">\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">Наборов параметров всего</th>\n";
	file << "              <td align=\"center\">";	file << amount.allSets; file << "</td>\n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">Наборов параметров с ошибками</th>\n";
	file << "              <td align=\"center\">";	file << amount.setsWithError; file << "</td>\n ";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">Наборов параметров без ошибок</th>\n";
	file << "              <td align=\"center\">";	file << amount.setsWithoutError; file << "</td>\n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">Всего ошибок</th>\n";
	file << "              <td align=\"center\">"; file << amount.error; file << "</td>\n";
	file << "          </tr>\n";
	file << "          <tr>\n";
	file << "              <th align=\"left\">Всего предупреждений</th>\n";
	file << "              <td align=\"center\">";	file << amount.warning; file << "</td>\n";
	file << "          </tr>\n";
	file << "      </table>\n";
	file << "      <br/>\n";
	file << "      <h2>Ошибок и замечаний нет!</h2>\n";
	file << "  </body>\n";
	file << "</html>\n";

	file.close();	// Закрытие файла
}

// Создание страниц
void CReport::writeSheets(ofstream& file, int cBooks)
{
	bool pathDirectory = true;

	for (size_t cSheets = 0; cSheets < books[cBooks].sheets.size(); cSheets++)
	{
		if (!books[cBooks].sheets[cSheets].bErrorSheet) // Имеется ли ошибка на листе
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

// Создание страниц ошибок
void CReport::writeError(ofstream& file, errorBookData error, CString folder)
{
	bool pathDirectory = true;

	for (size_t iSheets = 0; iSheets < error.sheets.size(); iSheets++)	// Обход по листам
	{
		if (error.sheets[iSheets].signals.size() > 0)
		{
			CString name = error.name;

			//name.Delete(error.name.Find(_T(".")), error.name.GetLength() - error.name.Find(_T(".")));	// Удаляем значения после точки	(ПОСМОТРЕТЬ, ЕСЛИ ТАМ ВОВСЕ ТОЧКА)

			// Создание директории для книги, если ее нет
			if (pathDirectory)
			{
				pathDirectory = false;

				CString tPath = _T("");
				tPath.Format(_T("%s\\%s\\%s"), path, folder, name);
				CreateDirectory(tPath, NULL);

				file << "              <td align=\"center\">\n";
				file << "                 <dl>\n";
			}

			// Установка информации в главную страницу отчета
			CString fullPath = _T("");	// Полный путь страницы отчета
			CString pathHref = _T("");
			CString buffer = _T("");
			pathHref.Format(_T("%s\\%s\\%s.html"), folder, name, error.sheets[iSheets].name);
			buffer.Format(_T("                     <dt><a href=\"%s\">%d</a></dt>\n"), pathHref, error.sheets[iSheets].signals.size());
			file << CT2A(buffer);

			fullPath.Format(_T("%s\\%s"), path, pathHref);

			// Открытие файла
			ofstream tempFile;
			tempFile.open(fullPath);

			// Запись 
			tempFile << "<html>\n";
			tempFile << "  <head>\n";
			tempFile << "     <meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n";

			tempFile << "     <title>Замечания по книге \""; tempFile << CT2A(error.name); tempFile << "\"</title>\n";

			tempFile << "  </head>\n";
			tempFile << "  <body>\n";
			tempFile << "      <table border=\"1\"cellspacing=\"0\">\n";
			tempFile << "          <tr>\n";
			tempFile << "              <th rowspan=\"2\">№ Замечания</th>\n";

			tempFile << "              <th colspan=\"9\">Замечания. Книга \""; tempFile << CT2A(error.name);
			tempFile << "\". Лист \""; tempFile << CT2A(error.sheets[iSheets].name); tempFile << "\".</th>\n";

			tempFile << "          </tr>\n";
			tempFile << "          <tr>\n";
			tempFile << "              <th>№Слова</th>\n";
			tempFile << "              <th>Наименование сигнала</th>\n";
			tempFile << "              <th>Условное обозначение параметра / сигнала</th>\n";
			tempFile << "              <th>Единица измерения</th>\n";
			tempFile << "              <th>Минимальное значение</th>\n";
			tempFile << "              <th>Максимальное значение</th>\n";
			tempFile << "              <th>Цена старшего разряда</th>\n";
			tempFile << "              <th>Используемые разряды</th>\n";
			tempFile << "              <th>Примечание</th>\n";
			tempFile << "          </tr>\n";

			// Обход по ячейкам
			int iRow = 1;
			for (list<errorSignalData>::iterator it = error.sheets[iSheets].signals.begin(); it != error.sheets[iSheets].signals.end(); it++, iRow++)
			{
				tempFile << "          <tr>\n";
				tempFile << "              <th rowspan=\"2\"> &nbsp "; tempFile << iRow; tempFile << "</th>\n";
				
				int indxErr = getIndexErrorField(it);
				
				hightlightError(tempFile, indxErr, 0, it->sNumWordField);	// Подсвечиваем номер слова

				buffer.Empty();
				buffer.Format(_T("              <td >  &nbsp  %s</td>\n"), it->sTitleParamField[0]);
				tempFile << CT2A(buffer);

				hightlightError(tempFile, indxErr, 1, it->sTitleParamField[1]);	// Подсвечиваем идентификатор

				buffer.Empty();
				buffer.Format(_T("              <td align=\"center\"> &nbsp %s</td>\n"), it->sDimensionField);
				tempFile << CT2A(buffer);

				// Подсвечиваем
				hightlightError(tempFile, indxErr, 2, it->sMinMaxCsrValField[0]);	// Мин
				hightlightError(tempFile, indxErr, 3, it->sMinMaxCsrValField[1]);	// Макс
				hightlightError(tempFile, indxErr, 4, it->sMinMaxCsrValField[2]);	//Цср
				hightlightError(tempFile, indxErr, 5, it->sBitField);				// Биты
				hightlightError(tempFile, indxErr, 6, it->sCommentField);			// Коммент

				tempFile << "          </tr>\n";
				tempFile << "          <tr>\n";

				// Запись описания ошибки
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

			tempFile.close();	// Закрытие
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

//Подсветка ошибочного параметра
void CReport::hightlightError(ofstream& file, int tIndx, int Indx, CString field)
{
	CString buffer = _T("");
	CString color = _T("bgcolor = \"#FDFCD0\"");
	buffer.Format(_T("              <td align=\"center\" %s> &nbsp  %s</td>\n"), (tIndx == Indx) ? color : _T(""), field);
	file << CT2A(buffer);
}

// Поиск индекса ошибочного параметра
int CReport::getIndexErrorField(list<errorSignalData>::iterator it)
{
	return -1;
}

// Количество всех наборов данных
int CReport::getSetsAmount()
{
	int result = 0;

	for (size_t i = 0; i < books.size(); i++)
		result += books[i].sheets.size();

	return result;
}

// Количество наборов данных с ошибкой
int CReport::getSetsAmountWithError()
{
	int result = 0;

	for (size_t i = 0; i < books.size(); i++)
		for (size_t j = 0; j < books[i].sheets.size(); j++)
			if (!books[i].sheets[j].bErrorSheet)
				result++;

	return result;
}

// Количество ошибок
int CReport::getErrorAmount(vector <errorBookData> error)
{
	int result = 0;

	for (size_t i = 0; i < error.size(); i++)
		for (size_t j = 0; j < error[i].sheets.size(); j++)
			result += error[i].sheets[j].signals.size();

	return result;
}
