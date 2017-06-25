#include "stdafx.h"
#include "Report.h"

// Конструктор
CReport::CReport()	{	}

// Деструктор
CReport::~CReport()	{	}

#pragma region GenerateReport

// Генерация отчета об ошибках
void CReport::getReport(list <bookData>& books, list<errorSet>& Db, const CString& pathToSave) {
	if (pathToSave.IsEmpty())
		throw EmptyPathException();

	path = pathToSave;

	// Подсчет количества наборов и ошибок
	setAmount(books);
	setAmountError(Db);

	makeReport(books, Db);
}

// Генерация не пустого отчета
void CReport::makeReport(list <bookData>& books, list <errorSet>& errorDB) {
	ofstream file;	// Поток записи в файл
	CString tPath;	// Путь к текущему файлу

	// Создание и открытие файла
	tPath.Format(_T("%s\\Отчет.html"), path);
	file.open(tPath);

	// Запись в файл
	file << "<html>\n"
		"  <head>"
		"     <meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n"
		"     <title>Замечания</title>\n"
		"  </head>\n"
		"  <body>\n"
		"      <h3>Статистика.</h3>\n"
		"      <table border=\"1\"cellspacing=\"0\">\n"
		"          <tr>\n"
		"              <th align=\"left\">Наборов параметров всего</th>\n"
		"              <td align=\"center\">";	file << amount.all;	file << "</td>\n"
		"          </tr>\n"
		"          <tr>\n"
		"              <th align=\"left\">Наборов параметров с ошибками</th>\n"
		"              <td align=\"center\">";	file << amount.withError; file << "</td> \n"
		"          </tr>\n"
		"          <tr>\n"
		"              <th align=\"left\">Наборов параметров без ошибок</th>\n"
		"              <td align=\"center\">";	file << amount.withoutError; file << "</td>\n"
		"          </tr>\n"
		"          <tr>\n"
		"              <th align=\"left\">Всего ошибок</th>\n"
		"              <td align=\"center\">"; file << amount.error; file << "</td>\n"
		"          </tr>\n"
		"          <tr>\n"
		"              <th align=\"left\">Всего предупреждений</th>\n"
		"              <td align=\"center\">";	file << amount.warning;	file << "</td>\n"
		"          </tr>\n"
		"      </table>\n"
		"      <br/>\n";

	if (amount.error + amount.warning != 0)
		startWrite(file, errorDB);
	else
		file << "      <h2>Ошибок и замечаний нет!</h2>\n";
	
	file << "  </body>\n"
		"</html>\n";

	file.close();	// Закрытие файла
}

void CReport::startWrite(ofstream& file, list <errorSet>& errorDB) {
	CString tPath;

	// Создание директорий для отчета
	tPath.Format(_T("%s\\Error"), path);
	CreateDirectory(tPath, NULL);
	tPath.Format(_T("%s\\Warning"), path);
	CreateDirectory(tPath, NULL);
	tPath.Format(_T("%s\\Error\\Syntax"), path);
	CreateDirectory(tPath, NULL);
	tPath.Format(_T("%s\\Error\\Simantic"), path);
	CreateDirectory(tPath, NULL);

	file << "      <h3>Отчет.</h3>\n";
		"      <table border=\"1\"cellspacing=\"0\">\n"
		"          <tr>\n"
		"              <th rowspan=\"2\">Название книги</th>\n"
		"              <th rowspan=\"2\">Название листа</th>\n"
		"              <th colspan=\"3\">Ошибки и замечания</th>\n"
		"              <th rowspan=\"2\">Предупреждения</th>\n"
		"          </tr>\n"
		"          <tr>\n"
		"              <th>Синтаксические</th>\n"
		"              <th>Семантические</th>\n"
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

// Создание страниц
void CReport::writeSheets(ofstream& file, vector <errorSheet>& set) {
	for (size_t i = 0; i < set.size(); i++) {
		file << "                     <dt>";
		file << CT2A(set[i].sheet->name);
		file << "</dt>\n";
	}

	file << "                 </dl>\n"
		"              </td>\n";
	// синтаксис
	file << "              <td align=\"center\">\n"
		"                     <dl>";
	for (size_t i = 0; i < set.size(); i++) {
		file << writeError(set[i].sheet, set[i].syntax, SYNTAX_FOLDER);
	}
	file << "                 </dl>\n"
		"              </td>\n";
	// семантика
	file << "              <td align=\"center\">\n"
		"                     <dl>";
	for (size_t i = 0; i < set.size(); i++) {
		file << writeError(set[i].sheet, set[i].simantic, SIMANTIC_FOLDER);
	}
	file << "                 </dl>\n"
		"              </td>\n";
	// предупреждения
	file << "              <td colspan=\"3\" align=\"center\">\n"
		"                     <dl>";
	for (size_t i = 0; i < set.size(); i++) {
		file << writeError(set[i].sheet, set[i].warning, WARNING_FOLDER);
	}
	file << "                 </dl>\n"
		"              </td>\n";
}

CString CReport::writeError(sheetData* sheet, const vector <errorSignal>& db, const CString& folder, const CString& bookName) {
	// Создать файл с ошибками
	// Записать туда ошибки
	// Закрыть поток с файлом

	CString pathFile;
	pathFile.Format(_T("%s\\%s\\%s\\%s.html"), path, folder, bookName, sheet->name);
	CreateDirectory(pathFile, NULL);

	ofstream file;
	file.open(pathFile);

	// Шапочка
	file << "<html>\n"
		"  <head>\n"
		"     <meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n"
		"     <title>Замечания по книге \""; file << CT2A(bookName); file << "\"</title>\n"
		"  </head>\n"
		"  <body>\n"
		"      <table border=\"1\"cellspacing=\"0\">\n"
		"          <tr>\n"
		"              <th rowspan=\"2\">№ Замечания</th>\n"
		"              <th colspan=\"9\">Замечания. Книга \""; file << CT2A(bookName);
	file << "\". Лист \""; file << CT2A(sheet->name); file << "\".</th>\n"
		"          </tr>\n"
		"          <tr>\n"
		"              <th>№Слова</th>\n"
		"              <th>Наименование сигнала</th>\n"
		"              <th>Условное обозначение параметра / сигнала</th>\n"
		"              <th>Единица измерения</th>\n"
		"              <th>Минимальное значение</th>\n"
		"              <th>Максимальное значение</th>\n"
		"              <th>Цена старшего разряда</th>\n"
		"              <th>Используемые разряды</th>\n"
		"              <th>Примечание</th>\n"
		"          </tr>\n";

	int iRow = 1;
	for (size_t i = 0; i < db.size(); i++) {
		file << "          <tr>\n"
			"              <th rowspan=\"2\"> &nbsp "; file << iRow; file << "</th>\n";
	}
}

/*// Создание страниц ошибок
void CReport::writeError(ofstream& file, const vector <errorSignal>& error, const CString& folder) {
	bool pathDirectory = true;
	CString colspan;

	(folder.Compare(_T("Warning")) == 0) ? colspan = "colspan=\"3\"" : colspan = "";

	for (size_t iSheets = 0; iSheets < error.sheets.size(); iSheets++) { // Обход по листам
		if (error.sheets[iSheets].signals.size() > 0) {
			// Создание директории для книги, если ее нет
			if (pathDirectory) {
				pathDirectory = false;

				CString tPath = _T("");
				tPath.Format(_T("%s\\%s\\%s"), path, folder, error.name);
				CreateDirectory(tPath, NULL);

				file << "              <td "; file << CT2A(colspan); file << " align=\"center\">\n"
					"                 <dl>\n";
			}

			// Установка информации в главную страницу отчета
			CString fullPath = _T("");	// Полный путь страницы отчета
			CString pathHref = _T("");
			CString buffer = _T("");
			pathHref.Format(_T("%s\\%s\\%s.html"), folder, error.name, error.sheets[iSheets].name);
			buffer.Format(_T("                     <dt><a href=\"%s\">%d</a></dt>\n"), pathHref, error.sheets[iSheets].signals.size());
			file << CT2A(buffer);

			fullPath.Format(_T("%s\\%s"), path, pathHref);

			// Открытие файла
			ofstream tempFile;
			tempFile.open(fullPath);

			// Запись 
			tempFile << "<html>\n"
				"  <head>\n"
				"     <meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n"
				"     <title>Замечания по книге \""; tempFile << CT2A(error.name); tempFile << "\"</title>\n"
				"  </head>\n"
				"  <body>\n"
				"      <table border=\"1\"cellspacing=\"0\">\n"
				"          <tr>\n"
				"              <th rowspan=\"2\">№ Замечания</th>\n" 
				"              <th colspan=\"9\">Замечания. Книга \""; tempFile << CT2A(error.name);
			tempFile << "\". Лист \""; tempFile << CT2A(error.sheets[iSheets].name); tempFile << "\".</th>\n"
				"          </tr>\n"
				"          <tr>\n"
				"              <th>№Слова</th>\n"
				"              <th>Наименование сигнала</th>\n"
				"              <th>Условное обозначение параметра / сигнала</th>\n"
				"              <th>Единица измерения</th>\n"
				"              <th>Минимальное значение</th>\n"
				"              <th>Максимальное значение</th>\n"
				"              <th>Цена старшего разряда</th>\n"
				"              <th>Используемые разряды</th>\n"
				"              <th>Примечание</th>\n"
				"          </tr>\n";

			// Обход по ячейкам
			int iRow = 1;
			for (list<errorSignalData>::iterator it = error.sheets[iSheets].signals.begin(); it != error.sheets[iSheets].signals.end(); it++, iRow++) {
				tempFile << "          <tr>\n"
					"              <th rowspan=\"2\"> &nbsp "; tempFile << iRow; tempFile << "</th>\n";
				
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

				tempFile << "          </tr>\n"
					"          <tr>\n";

				// Запись описания ошибки
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

			tempFile.close();	// Закрытие
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
//Подсветка ошибочного параметра
void CReport::hightlightError(ofstream& file, int tIndx, int Indx, CString field) {
	CString buffer = _T("");
	CString color = _T("bgcolor = \"#FDFCD0\"");
	buffer.Format(_T("              <td align=\"center\" %s> &nbsp  %s</td>\n"), (tIndx == Indx) ? color : _T(""), field);
	file << CT2A(buffer);
}

// Поиск индекса ошибочного параметра
int CReport::getIndexErrorField(list<errorSignalData>::iterator it) {
	int result = -1;
	CString tab = *it->sErrorField.begin();

	for (int i = 0; i < 7; i++)
		if (tab.Compare(errRemarks[i]) == 0)
			result = i;

	return result;
}

// Установка количества набора данных (всего, с ошибками, без)
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

// Начало генерации txt файлов
void CReport::getTxt(list <bookData>& books, const CString& pathToSave, const bool& bNumPK) {
	// Создание директории
	path = pathToSave;
	path.Format(_T("%s\\Text"), pathToSave);
	CreateDirectory(path, NULL);

	CString filePath = path;
	filePath.Format(_T("%s\\_ProtocolMain.txt"), pathToSave);

	// Открытие главного файла для записи
	ofstream mainFile;
	mainFile.open(filePath);

	// Обход по книгам
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++) {
		// Обход по страницам
		for (size_t i = 0; i < it->sheets.size(); i++) {
			if (!it->sheets[i].error) { // Если нет на странице ошибок
				ofstream tmpFile;
				CString name;
				sheetInfo info;

				info.np = it->sheets[i].np;
				info.pk = it->sheets[i].pk;
				info.bPK = bNumPK;

				// Создание txt файла для записи
				name.Format(_T("NP_%d_%s.txt"), info.np, it->sheets[i].name);
				filePath.Format(_T("%s\\%s"), path, name);
				tmpFile.open(filePath);
				mainFile << "#include \""; mainFile << CT2A(name); mainFile << "\"\n";

				for (size_t j = 0; j < it->sheets[i].signals.size(); j++) 
					writeTxtParam(tmpFile, it->sheets[i].signals[j], info);	// Запись параметра

				tmpFile.close();
			}
			else continue;
		}
	}
	mainFile.close();
}

// Запись сигнала в txt файл
void CReport::writeTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info) {
	CString buffer = signal.title[0];

	if (buffer.Find(RESERVE_SIGNAL) == -1) {
		buffer.Format(_T("PAR=%s\n"), signal.title[1]);	// Запись обозначения сигнала
		file << CT2A(buffer);

		buffer.Replace(_T("\""), _T("\\\""));
		buffer.Format(_T("\t NAME=\"%s\"\n"), signal.title[0]); // Наименования сигнала
		file << CT2A(buffer);

		if (!signal.dimension.IsEmpty()) {
			buffer.Format(_T("\t UNIT=\"%s\"\n"), signal.dimension);	// Размерности
			file << CT2A(buffer);
		}

		// Установка набора
		if (info.pk != 0 && info.bPK)
			buffer.Format(_T("\t SET=%d:%d\n"), info.pk, info.np);
		else
			buffer.Format(_T("\t SET=%d\n"), info.np);
		file << CT2A(buffer);

		int max = (signal.bit[1] == 0 ? signal.bit[0] : signal.bit[1]);

		// Запись номера слова и битов
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

		// Мин, макс, цср
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

		// Запись комментариев
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
