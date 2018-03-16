#include "stdafx.h"
#include "Report.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Конструктор
CReport::CReport()	{	}

// Деструктор
CReport::~CReport()	{	}

// Генерация отчета об ошибках
void CReport::GetReport(pivData& data, const bool& isProj) 
{
	if (settings.GetPath().IsEmpty())
		throw EmptyPathException();

	logger >> L"Создание отчета о полученных ошибках...";
	this->isProject = isProj;

	CString tPath;	// Путь к текущему файлу

	// Создание и открытие файла
	isProject ? tPath.Format(_T("%s%s"), settings.GetPath(), settings.folders[project]) :
		tPath.Format(_T("%s%s"), settings.GetPath(), settings.folders[other]);
	CreateDirectory(tPath, NULL);
	CreateMainFile(data, tPath);
	logger >> L"Создание отчета о полученных ошибках завершено";
}

// Создание директорий для отчета
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

#pragma region MAIN_FILE
// Создание основного файла
void CReport::CreateMainFile(pivData& data, CString path)
{
	logger >> L"Создание главного отчет-файла...";
	CreateFolders(path);
	path.Format(L"%s\\%s", path, REPORT_NAME);
	CStdioFile file(path, CFile::modeCreate | CFile::modeWrite | CFile::typeUnicode);	// Поток записи в файл
	WriteFile(file, L"<!DOCTYPE html>\n"
		"<html lang=\"ru\">\n"
		"<head>\n"
		+ T(1) + L"<meta charset=\"UTF-8\">\n"
		+ T(1) + L"<title>Замечания</title>\n");
	CssStyle(file, true);
	WriteFile(file, L"</head>\n"
		L"<body>\n"
		+ T(1) + L"<div class=\"tabs\">\n"
		+ T(2) + L"<![if gt IE 9]>\n"
		+ T(3) + L"<input id=\"tab1\" type=\"radio\" name=\"tabs\" checked>\n"
		+ T(3) + L"<label for=\"tab1\">Отчет</label>\n"
		+ T(3) + L"<input id=\"tab2\" type=\"radio\" name=\"tabs\">\n"
		+ T(3) + L"<label for=\"tab2\">Статистика</label>\n"
		+ T(2) + L"<![endif]>\n"
		+ T(2) + L"<section id=\"content-tab1\">\n");
	MainTable(file, data.db);
	WriteFile(file, T(2) + L"</section>\n"
		+ T(2) + L"<![if gt IE 9]>\n"
		+ T(2) + L"<section id=\"content-tab2\">\n");
	InfoTable(file, SetAmount(data));
	WriteFile(file, T(2) + L"</section>\n"
		+ T(2) + L"<![endif]>\n"
		+ T(1) + L"</div>\n");
	WriteFile(file, L"</body>\n");
	//ScriptMain(file);
	WriteFile(file, L"</html>\n");
	file.Close();
}

// Начало записи замечаний
void CReport::MainTable(CStdioFile& file, list <errorData>& db) 
{
	// Генерация шапки
	WriteFile(file, T(3) + L"<table>\n"
		+ T(4) + L"<caption>Отчет</caption>\n"
		+ T(4) + L"<thead>\n"
		+ T(4) + L"<tr>\n"
		+ T(5) + L"<th rowspan=\"2\" width=\"250px\">Название протокола</th>\n"
		+ T(5) + L"<th rowspan=\"2\" width=\"160px\">Название листа</th>\n"
		+ T(5) + L"<th colspan=\"3\">Ошибки и замечания</th>\n"
		+ T(4) + L"</tr>\n"
		+ T(4) + L"<tr>\n"
		+ T(5) + L"<th>Синтаксические</th>\n"
		+ T(5) + L"<th>Семантические</th>\n"
		+ T(5) + L"<th>Предупреждения</th>\n"
		+ T(4) + L"</tr>\n"
		+ T(4) + L"</thead>\n"
		+ T(4) + L"<tbody>\n");

	// Обход по книгам
	for (list <errorData>::iterator it = db.begin(); it != db.end(); it++) 
	{
		WriteFile(file, T(4) + L"<tr>\n"
			+ T(5) + L"<td class=cbooks>" + it->book->name + L"</td>\n");
		WriteBook(file, it);	// Запись листов
		logger >> L"Создан отчет по протоколу \"" + it->book->name + L"\"";
		WriteFile(file, T(4) + L"</tr>\n");
	}
	WriteFile(file, T(4) + L"</tbody>\n"
		+ T(3) + L"</table>\n");
}

// Таблица общей информации о наборах данных
void CReport::InfoTable(CStdioFile& file, const amountInfo& amount)
{
	WriteFile(file, /*T(1) + L"<div style=\"text-align: center; margin-top: 10px\">\n"
		+ T(2) + L"<a class=\"center\" href=\"javascript:sh()\">Статистика</a></br>\n"
		/*+ T(2) + L"<div class=\"center\" id = \"info\" style = \"padding-top: 15px;\">\n"
		+ T(3) + L"<p>Кол-во листов всего: " + IntToCString(amount.all) + L"</p>\n"
		+ T(3) + L"<p>Кол-во листов с ошибками: " + IntToCString(amount.withError) + L"</p>\n"
		+ T(3) + L"<p>Количество листов без ошибок: " + IntToCString(amount.withoutError) + L"</p>\n"
		+ T(3) + L"<p>Всего ошибок: " + IntToCString(amount.error) + L"</p>\n"
		+ T(3) + L"<p>Всего предупреждений: " + IntToCString(amount.warning) + L"</p>\n"*/
		//+ T(2) + L"<div class=\"center\" id = \"info\" style = \"padding-top: 15px;\">\n"
		T(3) + L"<table class=info>\n"
		+ T(4) + L"<thead>\n"
		+ T(4) + L"<tr>\n"
		+ T(5) + L"<th>Кол-во листов всего</th>\n"
		+ T(5) + L"<td>" + IntToCString(amount.all) + L"</td>\n"
		+ T(4) + L"</tr>\n"
		+ T(4) + L"<tr>\n"
		+ T(5) + L"<th>Кол-во листов с ошибками</th>\n"
		+ T(5) + L"<td>" + IntToCString(amount.withError) + L"</td>\n"
		+ T(4) + L"</tr>\n"
		+ T(4) + L"<tr>\n"
		+ T(5) + L"<th>Кол-во листов без ошибок</th>\n"
		+ T(5) + L"<td>" + IntToCString(amount.withoutError) + L"</td>\n"
		+ T(4) + L"</tr>\n"
		+ T(4) + L"<tr>\n"
		+ T(5) + L"<th>Всего ошибок</th>\n"
		+ T(5) + L"<td>" + IntToCString(amount.error) + L"</td>\n"
		+ T(4) + L"</tr>\n"
		+ T(4) + L"<tr>\n"
		+ T(5) + L"<th>Всего предупреждений</th>\n"
		+ T(5) + L"<td>" + IntToCString(amount.warning) + L"</td>\n"
		+ T(4) + L"</tr>\n"
		+ T(4) + L"</thead>\n"
		+ T(3) + L"</table>\n"
		/*+ T(2) + L"</div>\n"
		+ T(1) + L"</div>\n"*/);
}

// Запись листов текущего протокола
void CReport::WriteBook(CStdioFile& file, list <errorData>::iterator& it) 
{
	// Формирование шапки таблицы
	WriteFile(file, T(5) + L"<td class=csheets>\n");
	for (size_t i = 0; i < it->set.size(); i++)
		WriteFile(file, T(6) + L"<dt>" + it->set[i].data->name + L"</dt>\n");
	WriteFile(file, T(5) + L"</td>\n");

	// Запись синтаксических ошибок
	WriteFile(file, T(5) + L"<td class=csyntax>\n"); 
	for (size_t i = 0; i < it->set.size(); i++)
		WriteFile(file, WriteSheets(it->set[i].data, it->set[i].syntax, settings.folders[syntax], it->book->name));
	WriteFile(file, T(5) + L"</td>\n");

	// Запись семантических ошибок
	WriteFile(file, T(5) + L"<td class=csimantic>\n");
	for (size_t i = 0; i < it->set.size(); i++)
		WriteFile(file, WriteSheets(it->set[i].data, it->set[i].simantic, settings.folders[simantic], it->book->name));
	WriteFile(file, T(5) + L"</td>\n");

	// Запись замечаний
	WriteFile(file, T(5) + L"<td class=cwarning>\n");
	for (size_t i = 0; i < it->set.size(); i++)
		WriteFile(file, WriteSheets(it->set[i].data, it->set[i].warning, settings.folders[warning], it->book->name));
	WriteFile(file, T(5) + L"</td>\n");
}
#pragma endregion

#pragma region OTHER_FILES
// Запись таблицы с листа
CString CReport::WriteSheets(sheetData* sheet, const vector <errorSignal>& db, const CString& folder, const CString& bookName) 
{
	CString pathFile;
	isProject ? pathFile.Format(L"%s%s%s\\%s", settings.GetPath(), settings.folders[project], folder, bookName) : 
		pathFile.Format(L"%s%s%s\\%s", settings.GetPath(), settings.folders[other], folder, bookName);
	CreateDirectory(pathFile, NULL);
	
	CString result;	// Результирующая строка для записи ссылки в главный файл
	int count = CountError(db);

	if (count > 0) 
	{
		pathFile.Format(_T("%s\\%s.html"), pathFile, sheet->name);
		CString relativePath = pathFile;
		relativePath.Delete(0, settings.GetPath().GetLength());
		isProject ? relativePath.Delete(0, settings.folders[project].GetLength()) : relativePath.Delete(0, settings.folders[other].GetLength());
		relativePath.Insert(0, L".");
		result.Format(L"%s<dt><a href=\"%s\">%d</a></dt>\n", T(6), relativePath, count);	// Формирование результирующей строки (ссылки)
		
		CStdioFile file(pathFile, CFile::modeCreate | CFile::modeWrite | CFile::typeUnicode);
		// Шапочка
		WriteFile(file, L"<!DOCTYPE html>\n"
			"<html lang=\"ru\">\n"
			"<head>\n"
			+ T(1) + L"<meta charset=\"UTF-8\">\n"
			+ T(1) + L"<title>Замечания по книге \"" + bookName + L"\"</title>\n");
		CssStyle(file);
		WriteFile(file, L"</head>\n"
			"<body>\n"
		);

		SheetTableHeader(file, bookName, sheet->name, sheet->arinc);
		WriteFile(file, T(2) + L"<tbody>\n");
		// Запись наборов данных
		for (size_t i = 0; i < db.size(); i++)
		{
			// Индекс ошибки
			WriteFile(file, T(3) + L"<tr>\n"
				+ T(4) + L"<th rowspan=\"2\" class=cnum-remark>" + IntToCString((int)i + 1) + L"</th>\n");
			WriteSignal(file, db[i]);
		}

		WriteFile(file, T(2) + L"</tbody>\n"
			+ T(1) + L"</table>\n"
			"</body>\n"
			"</html>");
		file.Close();
	}
	else
		result.Format(L"%s<dt>-</dt>\n", T(6)); // Формирование результирующей строки
	return result;
}

// Шапка таблицы с ошибками листа
void CReport::SheetTableHeader(CStdioFile& file, const CString& book, const CString& sheet, bool arinc)
{
	WriteFile(file, T(1) + L"<table>\n"
		+ T(2) + L"<caption> Замечания. Книга \"" + book + L"\". Лист \"" + sheet + L"\". </caption>\n"
		+ T(2) + L"<thead>\n"
		+ T(2) + L"<tr>\n"
		+ T(3) + L"<th width=\"20px\">№ з-я</th>\n");
	arinc ? WriteFile(file, T(3) + L"<th width=\"40px\">Адрес</th>\n") : WriteFile(file, T(3) + L"<th width=\"40px\">№ Сл-а</th>\n");
	WriteFile(file, T(3) + L"<th width=\"180px\">Наименование</th>\n"
		+ T(3) + L"<th width=\"150px\">Идентификатор</th>\n"
		+ T(3) + L"<th width=\"60px\">Ед. изм.</th>\n"
		+ T(3) + L"<th width=\"60px\">Мин. зн.</th>\n"
		+ T(3) + L"<th width=\"60px\">Макс. зн.</th>\n"
		+ T(3) + L"<th width=\"60px\">ЦСР</th>\n"
		+ T(3) + L"<th width=\"100px\">Исполь-ные разряды</th>\n"
		+ T(3) + L"<th width=\"260px\">Примечание</th>\n"
		+ T(2) + L"</tr>\n"
		+ T(2) + L"</thead>\n");
}

// Запись одного набора параметров таблицы
void CReport::WriteSignal(CStdioFile& file, const errorSignal& signal) 
{
	WriteFile(file, FormValue(signal.data->numWord.field, signal.check[check::numword], 1));
	WriteFile(file, FormValue(signal.data->title[0], false, 2));
	WriteFile(file, FormValue(signal.data->title[1], signal.check[check::title], 3));
	WriteFile(file, FormValue(signal.data->dimension, false, 4));
	WriteFile(file, FormValue(signal.data->min.field, signal.check[check::min], 4));
	WriteFile(file, FormValue(signal.data->max.field, signal.check[check::max], 4));
	WriteFile(file, FormValue(signal.data->csr.field, signal.check[check::csr], 4));
	WriteFile(file, FormValue(signal.data->bit.field, signal.check[check::bits], 5));
	WriteFile(file, FormValue(signal.data->comment, signal.check[check::comment], 6));

	WriteFile(file, T(3) + L"</tr>\n"
		+ T(3) + L"<tr>\n");

	// Запись всех ошибок
	WriteFile(file, T(4) + L"<td style=\"padding-left: 20; padding-top: 0; padding-bottom: 15\" colspan=\"10\" bgcolor = \"#FDFCD0\">\n");
	for (size_t j = 0; j < signal.error.size(); j++) 
	{
		CString buffer;
		buffer.Format(L"%s<p class=remarks> – %s</p>\n", T(5), signal.error[j]);
		WriteFile(file, buffer);
	}
	WriteFile(file, T(4) + L"</td>\n"
		+ T(3) + L"</tr>\n"
		+ T(3) + L"<tr>\n"
		//+ T(4) + L"<td colspan=\"10\"> &nbsp </td>\n"
		+ T(3) + L"</tr>\n");
}

// Формирование строки параметра из сигнала
CString CReport::FormValue(CString field, const bool& color, const int& width)
{
	CString result;
	field.Replace(L"\n", L" <br> ");
	color ? result = L"bgcolor = \"#FDFCD0\"" : result = L"";
	result.Format(L"%s<td %s class=c%d> %s</td>\n", T(4), result, width, field);
	//result.Format(L"%s<td %s> %s</td>\n", T(4), result, field);
	return result;
}
#pragma endregion

#pragma region CSS_STYLES
// Табуляция
CString CReport::T(int n)
{
	CString res = L"";
	for (; n > 0; n--)
		res.Format(L"%s\t", res);
	return res;
}

// Запись CSS стиля
void CReport::CssStyle(CStdioFile& file, const bool& isMain)
{
	// TODO: Закончить работу с плавующей шапкой
	WriteFile(file, T(1) + L"<style type=\"text/css\">\n"
		+ CssCaption() +
		CssTable(isMain) +
		CssTh() +
		CssTrTd());
	isMain ? WriteFile(file, CssLinks() + CssTabs() + CssClasses1()) : WriteFile(file, CssClasses2());
	WriteFile(file, T(1) + L"</style>\n");
}

// Стиль таблицы
CString CReport::CssTable(const bool& isMain)
{
	CString res = T(2) + L"table {\n"
		+ T(3) + L"font-family: Arial, Helvetica, sans-serif;\n"
		+ T(3) + L"color: #666;\n"
		+ T(3) + L"font-size: 12px;\n"
		+ T(3) + L"text-shadow: 1px 1px 0px #fff;\n"
		+ T(3) + L"background: #eaebec;\n"
		+ T(3) + L"margin: auto;\n"
		+ T(3) + L"border: #ccc 1px solid;\n"
		+ T(3) + L"border-collapse: separate;\n"
		+ T(3) + L"-moz-border-radius: 3px;\n"
		+ T(3) + L"-webkit-border-radius: 3px;\n"
		+ T(3) + L"border-radius: 3px;\n"
		+ T(3) + L"-moz-box-shadow: 0 1px 2px #d1d1d1;\n"
		+ T(3) + L"-webkit-box-shadow: 0 1px 2px #d1d1d1;\n"
		+ T(3) + L"box-shadow: 0 1px 2px #d1d1d1;\n"
		+ T(2) + L"}\n"

		+ T(2) + L"thead {\n"
		+ T(3) + L"display: block;\n"
		+ T(3) + L"overflow: hidden;\n"
		+ T(2) + L"}\n"

		+ T(2) + L"tbody {\n"
		+ T(3) + L"display: block;\n"
		+ T(3) + L"height: 100%;\n"
		+ T(3) + L"background: #fff;\n"
		+ T(3) + L"overflow-y: scroll;\n"
		+ T(2) + L"}\n";

	isMain ? res.Format(L"%s%sthead, tbody { width: %dpx; }\n%stbody { max-height: %dpx; }\n", res, T(2), 800, T(2), 500) :
		res.Format(L"%s%sthead, tbody { width: %dpx; }\n%stbody { max-height: %dpx; }\n", res, T(2), 1200, T(2), 700);
	return res;
}

// Стиль имени таблицы
CString CReport::CssCaption()
{
	return T(2) + L"caption {\n"
		+ T(3) + L"font-family: Arial, Helvetica, sans-serif;\n"
		+ T(3) + L"font-size: 15px;\n"
		+ T(3) + L"font-weight: bold;\n"
		+ T(3) + L"padding-bottom: 10px;\n"
		+ T(2) + L"}\n";
}

// Стиль хедера таблицы
CString CReport::CssTh()
{
	return T(2) + L"th {\n"
		+ T(3) + L"font-weight: bold;\n"
		+ T(3) + L"padding: 15px 10px 15px 10px;\n"
		+ T(3) + L"border-top: 1px solid #fafafa;\n"
		+ T(3) + L"border-bottom:1px solid #e0e0e0;\n"
		+ T(3) + L"background: #ededed;\n"
		+ T(3) + L"background: -webkit-gradient(linear, left top, left bottom, from(#ededed), to(#ebebeb));\n"
		+ T(3) + L"background: -moz-linear-gradient(top,  #ededed,  #ebebeb);\n"
		+ T(2) + L"}\n"

		+ T(2) + L"th:first-child {\n"
		+ T(3) + L"text-align: center;\n"
		+ T(3) + L"padding-left: 0px;\n"
		+ T(2) + L"}\n"

		+ T(2) + L"tr:first-child th:first-child {\n"
		+ T(3) + L"-moz-border-radius-top-left: 3px;\n"
		+ T(3) + L"-webkit-border-top-left-radius: 3px;\n"
		+ T(3) + L"border-top-left-radius: 3px;\n"
		+ T(2) + L"}\n"

		+ T(2) + L"tr:first-child th:last-child {\n"
		+ T(3) + L"-moz-border-radius-top-right: 3px;\n"
		+ T(3) + L"-webkit-border-top-right-radius: 3px;\n"
		+ T(3) + L"border-top-right-radius: 3px;\n"
		+ T(2) + L"}\n";
}

// Стиль внутренних блоков таблицы
CString CReport::CssTrTd()
{
	return T(2) + L"tr {\n"
		+ T(3) + L"text-align: center;\n"
		+ T(3) + L"padding-left: 0px;\n"
		+ T(2) + L"}\n"

		+ T(2) + L"tr td:first-child{\n"
		+ T(3) + L"text-align: left;\n"
		+ T(3) + L"padding-left: 20px;\n"
		+ T(3) + L"border-left: 0;\n"
		+ T(2) + L"}\n"

		+ T(2) + L"tr td, dt{\n"
		+ T(3) + L"padding: 10px 0px 10px 0px;\n"
		+ T(3) + L"border-top: 1px solid #ffffff;\n"
		+ T(3) + L"border-bottom: 1px solid #e0e0e0;\n"
		+ T(3) + L"border-left: 1px solid #e0e0e0;\n"
		+ T(2) + L"}\n"

		+ T(2) + L"tr:last-child td{\n"
		+ T(3) + L"border-bottom: 0;\n"
		+ T(2) + L"}\n"

		+ T(2) + L"tr:last-child td:first-child{\n"
		+ T(3) + L"-moz-border-radius-bottom-left: 3px;\n"
		+ T(3) + L"-webkit-border-bottom-left-radius: 3px;\n"
		+ T(3) + L"border-bottom-left-radius: 3px;\n"
		+ T(2) + L"}\n"

		+ T(2) + L"tr:last-child td:last-child{\n"
		+ T(3) + L"-moz-border-radius-bottom-right: 3px;\n"
		+ T(3) + L"-webkit-border-bottom-right-radius: 3px;\n"
		+ T(3) + L"border-bottom-right-radius: 3px;\n"
		+ T(2) + L"}\n";
}

// Стилизация ссылок
CString CReport::CssLinks()
{
	return T(2) + L"a:link {\n"
		+ T(3) + L"color: #666;\n"
		+ T(3) + L"font-weight: bold;\n"
		+ T(3) + L"text-decoration: none;\n"
		+ T(2) + L"}\n"

		+ T(2) + L"a:visited {\n"
		+ T(3) + L"color: #999999;\n"
		+ T(3) + L"font-weight: bold;\n"
		+ T(3) + L"text-decoration: none;\n"
		+ T(2) + L"}\n"

		+ T(2) + L"a:active, a:hover { \n"
		+ T(3) + L"color: #bd5a35;\n"
		+ T(3) + L"text-decoration: underline;\n"
		+ T(2) + L"}\n";
}

// Стиль вкладов
CString CReport::CssTabs()
{
	return T(2) + L".tabs{\n"
		+ T(3) + L"min-width: 320px;\n"
		+ T(3) + L"max-width: 900px;\n"
		+ T(3) + L"padding: 0px;\n"
		+ T(3) + L"margin: 0 auto;\n"
		+ T(2) + L"}\n"
		/* Стили секций с содержанием */
		+ T(2) + L".tabs>section {\n"
		+ T(3) + L"display: none;\n"
		+ T(3) + L"padding: 15px;\n"
		+ T(3) + L"background: #fff;\n"
		+ T(3) + L"border: 1px solid #ddd;\n"
		+ T(2) + L"}\n"
		+ T(2) + L".tabs>section>p {\n"
		+ T(3) + L"margin: 0 0 5px;\n"
		+ T(3) + L"line-height: 1.5;\n"
		+ T(3) + L"color: #383838;\n"
		/* прикрутим анимацию */
		+ T(3) + L"-webkit-animation-duration: 1s;\n"
		+ T(3) + L"animation-duration: 1s;\n"
		+ T(3) + L"-webkit-animation-fill-mode: both;\n"
		+ T(3) + L"animation-fill-mode: both;\n"
		+ T(3) + L"-webkit-animation-name: fadeIn;\n"
		+ T(3) + L"animation-name: fadeIn;\n"
		+ T(2) + L"}\n"
		/* Описываем анимацию свойства opacity */
		+ T(2) + L"@-webkit-keyframes fadeIn {\n"
		+ T(3) + L"from { opacity : 0}\n"
		+ T(3) + L"to { opacity: 1; }\n"
		+ T(2) + L"}\n"
		+ T(2) + L"@keyframes fadeIn {\n"
		+ T(3) + L"from { opacity: 0; }\n"
		+ T(3) + L"to { opacity: 1; }\n"
		+ T(2) + L"}\n"
		/* Прячем чекбоксы */
		+ T(2) + L".tabs>input {\n"
		+ T(3) + L"display: none;\n"
		+ T(3) + L"position: absolute;\n"
		+ T(2) + L"}\n"
		/* Стили переключателей вкладок (табов) */
		+ T(2) + L".tabs>label {\n"
		+ T(3) + L"display: inline-block;\n"
		+ T(3) + L"margin: 0 0 -1px;\n"
		+ T(3) + L"padding: 15px 25px;\n"
		+ T(3) + L"font-weight: 600;\n"
		+ T(3) + L"text-align: center;\n"
		+ T(3) + L"color: #aaa;\n"
		+ T(3) + L"border: 0px solid #ddd;\n"
		+ T(3) + L"border-width: 1px 1px 1px 1px;\n"
		+ T(3) + L"background: #f1f1f1;\n"
		+ T(3) + L"border-radius: 3px 3px 0 0;\n"
		+ T(2) + L"}\n"
		/* Изменения стиля переключателей вкладок при наведении */
		+ T(2) + L".tabs>label:hover {\n"
		+ T(3) + L"color: #888;\n"
		+ T(3) + L"cursor: pointer;\n"
		+ T(2) + L"}\n"
		/* Стили для активной вкладки */
		+ T(2) + L".tabs>input:checked+label {\n"
		+ T(3) + L"color: #555;\n"
		+ T(3) + L"border-top: 1px solid #009933;\n"
		+ T(3) + L"border-bottom: 1px solid #fff;\n"
		+ T(3) + L"background: #fff;\n"
		+ T(2) + L"}\n"
		/* Активация секций с помощью псевдокласса :checked */
		+ T(2) + L"#tab1:checked~#content-tab1, #tab2:checked~#content-tab2 {\n"
		+ T(3) + L"display: block;\n"
		+ T(2) + L"}\n";
}

// Особенности ячеек основной таблицы отчета
CString CReport::CssClasses1()
{
	return T(2) + L".info thead{ width: 800px; overflow: hidden; }\n"
		+ T(2) + L".info th{ width: 400px; }\n"
		+ T(2) + L".info td{ width: 400px; background: #fff; }\n"
		+ T(2) + L"td:hover{ background: #fafafa; }\n"
		+ T(2) + L".cbooks { border-bottom: 1px solid #e0e0e0; width: 235px; }\n"
		+ T(2) + L".csheets { padding: 0px 0px 0px 0px; width: 180px; }\n"
		+ T(2) + L".csyntax { padding: 0px 0px 0px 0px; width: 105px; }\n"
		+ T(2) + L".csimantic { padding: 0px 0px 0px 0px; width: 110px; }\n"
		+ T(2) + L".cwarning { padding: 0px 0px 0px 0px; width: 110px; }\n";
}

// Особености ячеек таблицы ошибок
CString CReport::CssClasses2()
{
	return T(2) + L".remarks { margin-left: 20px; }\n"
		+ T(2) + L".cnum-remark { width: 18px; }\n"
		+ T(2) + L".c1 { width: 60px; }\n"
		+ T(2) + L".c2 { width: 200px; }\n"
		+ T(2) + L".c3 { padding-left: 10px; text-align: left; width: 158px; }\n"
		+ T(2) + L".c4 { width: 79px; }\n"
		+ T(2) + L".c5 { width: 120px; }\n"
		+ T(2) + L".c6 { padding-left: 10px; text-align: left; width: 255px; }\n";
}

// Скрипт для скрытия статистики в главном файле отчета
void CReport::ScriptMain(CStdioFile& file)
{
	WriteFile(file, L"<script type = \"text/javascript\">\n"
		+ T(1) + L"sh();\n"
		+ T(1) + L"function sh() {\n"
		+ T(1) + L"obj = document.getElementById(\"info\");\n"
		+ T(1) + L"if (obj.style.display == \"none\") { obj.style.display = \"block\"; }\n"
		+ T(1) + L"else { obj.style.display = \"none\"; }\n"
		+ T(1) + L"}\n"
		L"</script>\n");
}
#pragma endregion


// Преобразование int to CString
CString CReport::IntToCString(const int& number)
{
	CString result;
	result.Format(L"%d", number);
	return result;
}

// Преобразование текста к формату CW2A и запись в файл
void CReport::WriteFile(CStdioFile& file, const CString& text)
{
	CT2CA res(text, CP_UTF8);
	file.Write(res, (UINT)::strlen(res));
}

// Количество ошибок в текущей таблице
int CReport::CountError(const vector<errorSignal>& signal)
{
	int result = 0;
	for (size_t i = 0; i < signal.size(); i++)
		result += (int)signal[i].error.size();
	return result;
}

// Подсчет количества данных( всего, с ошибками и без ошибок), ошибок и предупреждений
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
