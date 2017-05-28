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
		//writeSyntaxError(file, cBooks);
		//writeSimanticError(file, cBooks);
		//writeWarning(file, cBooks);
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
		if (books[cBooks].sheets[cSheets].bErrorSheet) // Имеется ли ошибка на листе
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
			if (books[i].sheets[j].bErrorSheet)
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
