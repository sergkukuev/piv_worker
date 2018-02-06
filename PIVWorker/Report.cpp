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

#pragma region GENERATE_REPORT
// TODO: Исправить баги отчета во встроенной версии браузера
// Генерация отчета об ошибках
void CReport::GetReport(pivData& data, const CString& pathToSave, const bool& isProj) 
{
	if (pathToSave.IsEmpty())
		throw EmptyPathException();

	this->isProject = isProj;
	path = pathToSave;
	// Подсчет количества наборов и ошибок
	SetAmount(data.books);
	SetAmountError(data.db);

	CString tPath;	// Путь к текущему файлу

					// Создание и открытие файла
	isProject ? tPath.Format(L"%s%s", path, PROJECT_FOLDER) : tPath.Format(L"%s%s", path, OTHER_FOLDER);
	CreateDirectory(tPath, NULL);
	tPath.Format(L"%s\\Отчет.html", tPath);
	CStdioFile file(tPath, CFile::modeCreate | CFile::modeWrite | CFile::typeUnicode);	// Поток записи в файл

	ErrorTable(file);

	StartWrite(file, data.db);

	file.WriteString(L"\t</body>\n"
		"</html>\n");
	file.Close();
}

// Начало записи замечаний
void CReport::StartWrite(CStdioFile& file, list <errorData>& Db) 
{
	// Создание директорий для отчета
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

	// Генерация шапки
	file.WriteString(L"\t\t<h3>Отчет.</h3>\n"
		"\t\t<table border=\"1\"cellspacing=\"0\">\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th rowspan=\"2\">Название книги</th>\n"
		"\t\t\t\t<th rowspan=\"2\">Название листа</th>\n"
		"\t\t\t\t<th colspan=\"3\">Ошибки и замечания</th>\n"
		"\t\t\t\t<th rowspan=\"2\">Предупреждения</th>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th>Синтаксические</th>\n"
		"\t\t\t\t<th>Семантические</th>\n"
		"\t\t\t</tr>\n");

	// Обход по книгам
	for (list <errorData>::iterator it = Db.begin(); it != Db.end(); it++) 
	{
		file.WriteString(L"\t\t\t<tr>\n"
			"\t\t\t\t<td>"); 
		file.WriteString(it->book->name); 
		file.WriteString(L"</td>\n"
			"\t\t\t\t<td align=\"center\">\n"
			"\t\t\t\t\t<dl>\n");

		WriteBook(file, it);	// Запись листов
		file.WriteString(L"\t\t\t</tr>\n");
	}
	file.WriteString(L"\t\t</table>\n");
}

// Запись листов текущего протокола
void CReport::WriteBook(CStdioFile& file, list <errorData>::iterator& it) 
{
	// Формирование шапки таблицы
	for (size_t i = 0; i < it->set.size(); i++)
	{
		file.WriteString(L"\t\t\t\t\t\t<dt>");
		file.WriteString(it->set[i].data->name);
		file.WriteString(L"</dt>\n");
	}
	file.WriteString(L"\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n");
	
	// Запись синтаксических ошибок
	file.WriteString(L"\t\t\t\t<td align=\"center\">\n"
		"\t\t\t\t\t<dl>\n");
	for (size_t i = 0; i < it->set.size(); i++)
		file.WriteString(WriteSheets(it->set[i].data, it->set[i].syntax, SYNTAX_FOLDER, it->book->name));

	file.WriteString(L"\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n");
	// Запись семантических ошибок
	file.WriteString(L"\t\t\t\t<td align=\"center\">\n"
		"\t\t\t\t\t<dl>\n");
	for (size_t i = 0; i < it->set.size(); i++)
		file.WriteString(WriteSheets(it->set[i].data, it->set[i].simantic, SIMANTIC_FOLDER, it->book->name));

	file.WriteString(L"\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n");
	// Запись замечаний
	file.WriteString(L"\t\t\t\t<td colspan=\"3\" align=\"center\">\n"
		"\t\t\t\t\t<dl>\n");
	for (size_t i = 0; i < it->set.size(); i++)
		file.WriteString(WriteSheets(it->set[i].data, it->set[i].warning, WARNING_FOLDER, it->book->name));

	file.WriteString(L"\t\t\t\t\t</dl>\n"
		"\t\t\t\t</td>\n");
}

// Запись таблицы с листа
CString CReport::WriteSheets(sheetData* sheet, const vector <errorSignal>& db, const CString& folder, const CString& bookName) 
{
	CString pathFile;
	isProject ? pathFile.Format(L"%s%s%s\\%s", path, PROJECT_FOLDER, folder, bookName) : pathFile.Format(L"%s%s%s\\%s", path, OTHER_FOLDER, folder, bookName);
	CreateDirectory(pathFile, NULL);
	
	CString result;	// Результирующая строка для записи ссылки в главный файл
	int count = CountError(db);

	if (count > 0) 
	{
		pathFile.Format(L"%s\\%s.html", pathFile, sheet->name);
		CString relativePath = pathFile;
		relativePath.Delete(0, path.GetLength());
		isProject ? relativePath.Delete(0, PROJECT_SIZE) : relativePath.Delete(0, OTHER_SIZE);
		relativePath.Insert(0, L".");
		result.Format(L"\t\t\t\t\t\t<dt><a href=\"%s\">%d</a></dt>\n", relativePath, count);	// Формирование результирующей строки (ссылки)
		
		CStdioFile file(pathFile, CFile::modeCreate | CFile::modeWrite | CFile::typeUnicode);
		// Шапочка
		file.WriteString(L"<html>\n"
			"\t<style>"
			"\t\t.fixed {\n\t\t\tposition: fixed; \n\t\t\tbackground: white; \n\t\t\tcolor: black; \n\t\t}\n"
			"\t</style>\n"
			"\t<head>\n"
			"\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n"
			"\t\t<title>Замечания по книге \""); 
		file.WriteString(bookName);
		file.WriteString(L"\"</title>\n"
			"\t</head>\n"
			"\t<body style=\"margin: 0px; padding: 0px\">\n"
		);/* "\t\t<table style=\"word-break: break-all;\" border=\"1\"cellspacing=\"0\" class=\"fixed\">\n"
			"\t\t\t<tr>\n"
			"\t\t\t\t<th rowspan=\"2\" style=\"width: 5%\">№ Замечания</th>\n"
			"\t\t\t\t<th colspan=\"9\">Замечания. Книга \"");
		file.WriteString(bookName);
		file.WriteString(L"\". Лист \"");
		file.WriteString(sheet->name);
		file.WriteString(L"\".</th>\n"
			"\t\t\t</tr>\n"
			"\t\t\t<tr>\n");
		sheet->arinc ? file.WriteString(L"\t\t\t\t<th style=\"width: 5%\">Адрес</th>\n") : file.WriteString(L"\t\t\t\t<th style=\"width: 5%\">№Слова</th>\n");
		file.WriteString(L"\t\t\t\t<th style=\"width: 15%\">Наименование сигнала</th>\n"
			"\t\t\t\t<th style=\"width: 10%\">Условное обозначение параметра / сигнала</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">Единица измерения</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">Минимальное значение</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">Максимальное значение</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">Цена старшего разряда</th>\n"
			"\t\t\t\t<th style=\"width: 10%\">Используемые разряды</th>\n"
			"\t\t\t\t<th style=\"width: 30%\">Примечание</th>\n"
			"\t\t\t</tr>\n"
			"\t\t</table>\n"*/
		file.WriteString(L"\t\t<table style=\"word-break: break-all;\" border=\"1\"cellspacing=\"0\">\n"
			"\t\t\t<tr>\n"
			"\t\t\t\t<th rowspan=\"2\" style=\"width: 5%\">№ Замечания</th>\n"
			"\t\t\t\t<th colspan=\"9\">Замечания. Книга \""); 
		file.WriteString(bookName);
		file.WriteString(L"\". Лист \""); 
		file.WriteString(sheet->name);
		file.WriteString(L"\".</th>\n"
			"\t\t\t</tr>\n"
			"\t\t\t<tr>\n");
		sheet->arinc ? file.WriteString(L"\t\t\t\t<th style=\"width: 5%\">Адрес</th>\n") : file.WriteString(L"\t\t\t\t<th style=\"width: 5%\">№Слова</th>\n");
		file.WriteString(L"\t\t\t\t<th style=\"width: 15%\">Наименование сигнала</th>\n"
			"\t\t\t\t<th style=\"width: 10%\">Условное обозначение параметра / сигнала</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">Единица измерения</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">Минимальное значение</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">Максимальное значение</th>\n"
			"\t\t\t\t<th style=\"width: 5%\">Цена старшего разряда</th>\n"
			"\t\t\t\t<th style=\"width: 10%\">Используемые разряды</th>\n"
			"\t\t\t\t<th style=\"width: 30%\">Примечание</th>\n"
			"\t\t\t</tr>\n");

		// Запись наборов данных
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
		result.Format(L"\t\t\t\t\t\t<dt>-</dt>\n"); // Формирование результирующей строки
	return result;
}

// Запись одного набора параметров таблицы
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

	// Запись всех ошибок
	file.WriteString(L"\t\t\t\t<td style=\"padding-left: 20; padding-top: 0; padding-bottom: 15\" colspan=\"10\" bgcolor = \"#FDFCD0\">\n");
	for (size_t j = 0; j < set.error.size(); j++) 
	{
		buffer.Format(L"<br/>\t\t\t\t\t\t – %s\n", set.error[j]);
		file.WriteString(buffer);
	}

	file.WriteString(L"\t\t\t\t</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<td colspan=\"10\"> &nbsp </td>\n"
		"\t\t\t</tr>\n");
}

// Запись одного параметра из набора
CString CReport::WriteParam(const CString& field, const bool& color, const int& width) 
{
	CString result;
	CString tmp = field;
	tmp.Replace(L"\n", L" <br> ");
	color ? result = L"bgcolor = \"#FDFCD0\"" : result = L"";
	result.Format(L"\t\t\t\t<td align=\"center\" %s style=\"width: %d%%\"> %s</td>\n", result, width, tmp);
	return result;
}

// Преобразование int to CString
CString CReport::IntToCString(const int& number)
{
	CString result;
	result.Format(L"%d", number);
	return result;
}

// Таблица общей информации о наборах данных (вывод amountInfo)
void CReport::ErrorTable(CStdioFile& file)
{
	file.WriteString(L"<html>\n"
		"\t<head>\n"
		"\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=win-1251\" />\n"
		"\t\t<title>Замечания</title>\n"
		"\t</head>\n"
		"\t<body>\n"
		"\t\t<h3>Статистика.</h3>\n"
		"\t\t<table border=\"1\"cellspacing=\"0\">\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">Количество листов всего</th>\n"
		"\t\t\t\t<td align=\"center\">");	
	file.WriteString(IntToCString(amount.all));	
	file.WriteString(L"</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">Количество листов с ошибками</th>\n"
		"\t\t\t\t<td align=\"center\">");	
	file.WriteString(IntToCString(amount.withError)); 
	file.WriteString(L"</td> \n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">Количество листов без ошибок</th>\n"
		"\t\t\t\t<td align=\"center\">");	
	file.WriteString(IntToCString(amount.withoutError)); 
	file.WriteString(L"</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">Всего ошибок</th>\n"
		"\t\t\t\t<td align=\"center\">"); 
	file.WriteString(IntToCString(amount.error));
	file.WriteString(L"</td>\n"
		"\t\t\t</tr>\n"
		"\t\t\t<tr>\n"
		"\t\t\t\t<th align=\"left\">Всего предупреждений</th>\n"
		"\t\t\t\t<td align=\"center\">");	
	file.WriteString(IntToCString(amount.warning));
	file.WriteString(L"</td>\n"
		"\t\t\t</tr>\n"
		"\t\t</table>\n"
		"\t\t<br/>\n");
}

// Подсчет информации о наборах данных
#pragma region Amount
// Количество ошибок в текущей таблице
int CReport::CountError(const vector<errorSignal>& set)
{
	int result = 0;
	for (size_t i = 0; i < set.size(); i++)
		result += (int) set[i].error.size();
	return result;
}

// Подсчет количества наборов данных (всего, с ошибками, без ошибок)
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

// Подсчет ошибок и предупреждений
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

// Генерация txt файлов
#pragma region GENERATE_TXT
// одного 
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

// нескольких
void CReport::GetTxt(list <bookData>& books, const CString& pathToSave, const pivParam& params) 
{
	if (pathToSave.IsEmpty())
		throw EmptyPathException();

	path = pathToSave;
	CreateDirectory(path, NULL);
	path.Format(L"%s%s", path, TEXT_FOLDER);
	CreateDirectory(path, NULL);

	// Обход по книгам
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++)
		Generate(*it, params);
}

// Генерация txt протоколов
void CReport::Generate(const bookData& book, const pivParam& params) 
{
	CString tPath = path;
	tPath.Format(L"%s\\%s", path, book.name);
	CreateDirectory(tPath, NULL);

	CString filePath = path;
	filePath.Format(L"%s\\_ProtocolMain.txt", tPath);

	// Открытие главного файла для записи
	ofstream mainFile;
	mainFile.open(filePath);

	// Обход по страницам
	int cNP = 1, cNumWord = 1;	// Отдельные счетчики для arinc сигналов
	for (size_t i = 0; i < book.sheets.size(); i++, cNP++, cNumWord = 1) 
	{
		// Если нет на странице ошибок
		if (!book.sheets[i].error || params.bGenTxt) 
		{ 
			CString name;
			sheetInfo info;

			info.arinc = book.sheets[i].arinc;
			info.arinc ? info.np = cNP : info.np = book.sheets[i].np;
			info.pk = book.sheets[i].pk;
			info.bPK = params.bNumPK;

			// Создание txt файла для записи
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
				WriteTxtParam(tmpFile, book.sheets[i].signals[j], info, params.iProject, cNumWord);	// Запись параметра
			}
			tmpFile.close();
		}
		else 
			continue;
	}
	mainFile.close();
}

// Запись одного набора данных из таблицы в txt файл
void CReport::WriteTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info, const int& iProject, const int& arincNum)
{
	CString buffer = signal.title[0];

	if (buffer.Find(RESERVE_SIGNAL) != -1 || (iProject == project::kprno35 && dwPart::checkLow(buffer)))
		return;

	buffer = signal.title[1];
	buffer.Remove(L' ');
	if (signal.repWord && info.pk != -1)
		buffer.Format(L"%s_%d", buffer, info.pk);
	buffer.Format(L"PAR=%s\n", buffer);	// Запись обозначения сигнала
	file << CT2A(buffer);

	buffer = signal.title[0];
	bool dwKprno = false;
	if (iProject == project::kprno35)
		dwKprno = dwPart::deleleHight(buffer);
	buffer.Replace(L"\"", L"\\\"");
	buffer.Format(L"\tNAME=\"%s\"\n", buffer); // Наименования сигнала
	file << CT2A(buffer);

	if (!signal.dimension.IsEmpty()) 
	{
		buffer.Format(L"\tUNIT=\"%s\"\n", signal.dimension);	// Размерности
		file << CT2A(buffer);
	}

	// Установка набора
	info.pk > 0 && info.bPK ? buffer.Format(L"\tSET=%d:%d\n", info.pk, info.np) : buffer.Format(L"\tSET=%d\n", info.np);
	file << CT2A(buffer);

	int max = (signal.bit.value[1] == -1 ? signal.bit.value[0] : signal.bit.value[1]);

	// Запись номера слова и битов
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

	// Мин, макс, цср
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

	// Запись комментариев
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

// Проверка на int значение
bool CReport::IsInt(const double& numeric)
{
	int whole = (int)numeric;
	double fract = numeric - whole;
	bool result;
	fract == 0 ? result = true : result = false;
	return result;
}
#pragma endregion