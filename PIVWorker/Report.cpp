#include "stdafx.h"
#include "Report.h"

// Конструктор
CReport::CReport()	{	}

// Деструктор
CReport::~CReport()	{	}

#pragma region GenerateReport

// Генерация отчета об ошибках
void CReport::getReport(vector <bookData> books, errorSet errorDB, CString pathToSave) {
	if (pathToSave.IsEmpty())
		throw EmptyPathException();

	path = pathToSave;

	// Подсчет количества наборов и ошибок
	setAmount(books);	
	amount.error = getAmountError(errorDB.syntax) + getAmountError(errorDB.simantic);
	amount.warning = getAmountError(errorDB.warning);

	makeReport(books, errorDB);
}

// Генерация не пустого отчета
void CReport::makeReport(vector <bookData> books, errorSet errorDB) {
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
		startWrite(file, books, errorDB);
	else
		file << "      <h2>Ошибок и замечаний нет!</h2>\n";
	
	file << "  </body>\n"
		"</html>\n";

	file.close();	// Закрытие файла
}

void CReport::startWrite(ofstream& file, vector <bookData> books, errorSet errorDB) {
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

	for (size_t cBooks = 0; cBooks < books.size(); cBooks++) {
		writeSheets(file, books, cBooks);

		// Создание отчета о синтаксических, семантических ошибках и предупреждениях
		writeError(file, errorDB.syntax[cBooks], _T("Error\\Syntax"));
		writeError(file, errorDB.simantic[cBooks], _T("Error\\Simantic"));
		writeError(file, errorDB.warning[cBooks], _T("Warning"));
		file << "          </tr>\n";
	}
	file << "      </table>\n";
}

// Создание страниц
void CReport::writeSheets(ofstream& file, vector <bookData> books, int cBooks) {
	file << "          <tr>\n";
		"              <td>"; file << CT2A(books[cBooks].name); file << "</td>\n"
		"              <td align=\"center\">\n"
		"                 <dl>\n";

	for (size_t cSheets = 0; cSheets < books[cBooks].sheets.size(); cSheets++) {
		file << "                     <dt>"; 
		file << CT2A(books[cBooks].sheets[cSheets].name); 
		file << "</dt>\n";
	}

	file << "                 </dl>\n"
		"              </td>\n";
}

// Создание страниц ошибок
void CReport::writeError(ofstream& file, errorBookData error, CString folder) {
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
void CReport::setAmount(vector <bookData> books) {
	for (size_t i = 0; i < books.size(); i++) {
		amount.all += books[i].sheets.size();
		for (size_t j = 0; j < books[i].sheets.size(); j++)
			if (books[i].sheets[j].bError)
				amount.withError++;
	}

	amount.withoutError = amount.all - amount.withError;
}

// Количество ошибок
int CReport::getAmountError(vector <errorBookData> error) {
	int result = 0;

	for (size_t i = 0; i < error.size(); i++)
		for (size_t j = 0; j < error[i].sheets.size(); j++)
			result += error[i].sheets[j].signals.size();

	return result;
}

#pragma endregion

#pragma region GenerateTxt

// Начало генерации txt файлов
void CReport::getTxt(vector <bookData> books, CString pathToSave, bool bNumPK) {
	// Создание директории
	pathToSave.Format(_T("%s\\Text"), pathToSave);
	CreateDirectory(pathToSave, NULL);

	CString filePath = pathToSave;
	filePath.Format(_T("%s\\_ProtocolMain.txt"), pathToSave);

	// Открытие главного файла для записи
	ofstream mainFile;
	mainFile.open(pathToSave);

	// Обход по книгам
	for (size_t iBook = 0; iBook < books.size(); iBook++) {
		// Обход по страницам
		for (size_t iSheet = 0; iSheet < books[iBook].sheets.size(); iSheet++) {
			if (!books[iBook].sheets[iSheet].bError) { // Если нет на странице ошибок
				ofstream tmpFile;
				CString name;
				int NP = books[iBook].sheets[iSheet].iFieldNP;

				// Создание txt файла для записи
				name.Format(_T("NP_%d_%s.txt"), NP, books[iBook].sheets[iSheet].name);
				filePath.Format(_T("%s\\%s"), pathToSave, name);
				tmpFile.open(filePath);
				mainFile << "#include \""; mainFile << CT2A(name); mainFile << "\"\n";

				for (list <signalData>::iterator it = signal_set(iBook, iSheet).begin(); it != signal_set(iBook, iSheet).end(); it++) {
					sheetInfo info;
					info.NP = books[iBook].sheets[iSheet].iFieldNP;
					info.NumPk = books[iBook].sheets[iSheet].iNumPK;
					info.bNumPk = bNumPK;
					writeTxtParam(tmpFile, it, info);	// Запись параметра
				}
				tmpFile.close();
			}
			else continue;
		}
	}
	mainFile.close();
}

// Запись сигнала в txt файл
void CReport::writeTxtParam(ofstream& file, list <signalData>::iterator it, sheetInfo info) {
	CString buffer = it->sTitleParamField[0];

	if (buffer.Find(_T("Резерв")) == -1) {
		buffer.Format(_T("PAR=%s\n"), it->sTitleParamField[1]);	// Запись обозначения сигнала
		file << CT2A(buffer);

		buffer.Replace(_T("\""), _T("\\\""));
		buffer.Format(_T("\t NAME=\"%s\"\n"), it->sTitleParamField[0]); // Наименования сигнала
		file << CT2A(buffer);

		if (!it->sDimensionField.IsEmpty()) {
			buffer.Format(_T("\t UNIT=\"%s\"\n"), it->sDimensionField);	// Размерности
			file << CT2A(buffer);
		}

		// Установка набора
		if (info.NumPk != -1 && info.bNumPk)
			buffer.Format(_T("\t SET=%d:%d\n"), info.NumPk, info.NP);
		else
			buffer.Format(_T("\t SET=%d\n"), info.NP);
		file << CT2A(buffer);

		int max = (it->iBit[1] == 0 ? it->iBit[0] : it->iBit[1]);

		// Запись номера слова и битов
		if (it->b2NumWordField) { 
			file << "\t MERGE\n";
			buffer.Format(_T("\t\t WDADDR = %d,%d,%d\n"), it->iNumWord[0], it->iBit[0], max);
			file << CT2A(buffer);
			buffer.Format(_T("\t\t WDADDR = %d,%d,%d\n"), it->iNumWord[1], it->iBit[0], max);
			file << CT2A(buffer);
			file << "\t END_MERGE\n";
		}
		else {
			buffer.Format(_T("\t\t WDADDR = %d,%d,%d\n"), it->iNumWord[0], it->iBit[0], max);
			file << CT2A(buffer);
		}

		// Мин, макс, цср
		if (it->dMinMaxCsrVal[1] != 0 && it->dMinMaxCsrVal[2] != 0) { 
			file << "\t VALDESCR\n";
			file << (it->bCommentField ? "\t\t SIGNED\n" : "\t\t UNSIGNED\n");

			buffer = it->sMinMaxCsrValField[0];
			buffer.Replace(_T(","), _T("."));
			buffer.Format(_T("\t\t MIN = %s\n"), buffer);
			file << CT2A(buffer);

			buffer = it->sMinMaxCsrValField[1];
			buffer.Replace(_T(","), _T("."));
			buffer.Format(_T("\t\t MAX = %s\n"), buffer);
			file << CT2A(buffer);

			buffer = it->sMinMaxCsrValField[2];
			buffer.Replace(_T(","), _T("."));
			buffer.Format(_T("\t\t MSB = %s\n"), buffer);
			file << CT2A(buffer);

			file << "\t END_VALDESCR\n";
		}

		// Запись комментариев
		if (!it->sCommentField.IsEmpty()) {	
			file << "\t COMMENT\n";

			buffer = it->sCommentField;
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
