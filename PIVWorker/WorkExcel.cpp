#include "stdafx.h"
#include "WorkExcel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Конструктор
CWorkExcel::CWorkExcel(void) 
{
	if (!app.CreateDispatch(L"Excel.Application", NULL))
		throw AccessExcelException();
	app.put_Visible(FALSE);
	app.put_UserControl(FALSE);
	books.AttachDispatch(app.get_Workbooks());
	cells = NULL;
	// TODO: Создать в настройках флаг учета чтения базы номеров наборов
	ReadNpBase();
}

// Деструктор
CWorkExcel::~CWorkExcel(void) 
{
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();
	book.ReleaseDispatch();
	books.ReleaseDispatch();
	app.Quit();
	app.ReleaseDispatch();
	delete cells;
}
#pragma region NP_BASE
// Чтение номеров наборов из папки odb
void CWorkExcel::ReadNpBase()
{
	vector<CString> fileNames;	// Найденные имена файлов
	// Поиск всех файлов в папке 
	WIN32_FIND_DATA findFiles;
	HANDLE h;
	CString folder = settings.GetDefaultPath() + L"\\odb\\*";
	h = FindFirstFile(folder, &findFiles);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			fileNames.push_back(findFiles.cFileName);
		} while (FindNextFile(h, &findFiles) != 0);
		FindClose(h);
	}
	folder.Delete(folder.GetLength() - 1);	// Удаление звездочки из пути 
	// Чтение файлов 
	for (size_t i = 0; i < fileNames.size(); i++)
	{
		// Проверка разрешения
		int posDot = fileNames[i].ReverseFind(L'.');
		CString temp = fileNames[i].Mid(posDot, fileNames[i].GetLength() - posDot);
		if (temp.Compare(L".np") != 0)
			continue;

		// Построчная работа с файлом
		temp.Format(L"%s\\%s", folder, fileNames[i]);
		CStdioFile file;
		BOOL bOper = file.Open(temp, CFile::modeRead | CFile::typeText);
		do
		{
			bOper = file.ReadString(temp);
			// Смена кодировки с 1252 -> 1251
			int nChar = ::WideCharToMultiByte(CP_UTF8, 0, temp, (int)temp.GetLength(), NULL, 0, NULL, NULL);
			string result;
			result.resize(nChar);
			::WideCharToMultiByte(1252, 0, temp, (int)temp.GetLength(), const_cast<char*>(result.c_str()), nChar, NULL, NULL);
			// Парсинг
			np_s np;
			bool error = false;
			size_t pos = result.find_first_of("\t");
			np.number = ParseNp(result.substr(0, pos), error);
			if (!error)
			{
				result = result.substr(pos + 1, result.size());
				pos = result.find_first_of("\t");
				np.name = ParseFrameName(result.substr(0, pos));
				npBase.push_back(np);
			}
		} while (bOper);
		file.Close();
	}
}

// Парсинг номера набора
int CWorkExcel::ParseNp(string value, bool& flag)
{
	int result = -1;
	if (!value.empty())
	{
		char* end;
		errno = 0;
		result = strtol(value.c_str(), &end, 10);
		(*end != '\0' || errno != 0) ? flag = true : flag = flag;
	}
	else
		flag = true;
	return result;
}

// Парсинг имени кадра
CString CWorkExcel::ParseFrameName(string value)
{
	CString res(value.c_str());
	int pos1, pos2;
	// Удаление ИК обозначения
	do
	{
		pos1 = res.Find(L"ИК \"");
		pos2 = res.Find(L"ИК\"");
		if (pos1 != -1)
			res.Delete(pos1, 2);
		if (pos2 != -1)
			res.Delete(pos2, 2);
	} while (pos1 != -1 || pos2 != -1);

	// Удаление вхождений всех кавычек
	DeleteSymbol(res, L"\"");
	DeleteSymbol(res, L"«");
	DeleteSymbol(res, L"»");
	do // Удаление скобок и их содержимых
	{
		pos1 = res.Find(L'(');
		pos2 = res.Find(L')');
		int a = res.GetLength();
		if (pos1 != -1 && pos2 != -1 && pos1 < pos2)
			res.Delete(pos1, pos2 - pos1 + 1);
	} while (pos1 < pos2);
	res.Trim();
	return res;
}

// Удаление символа из строки
void CWorkExcel::DeleteSymbol(CString& field, const CString& sym)
{
	int pos;
	do
	{
		pos = field.Find(sym);
		if (pos != -1)
			field.Delete(pos);
	} while (pos != -1);
}
#pragma endregion

// Открытие книги
bool CWorkExcel::OpenBook(const CString& path) 
{
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	LPDISPATCH Lp = books.Open(path, covOptional, covOptional, covOptional, covOptional, covOptional,
						  covOptional, covOptional, covOptional, covOptional, covOptional,
						  covOptional, covOptional, covOptional, covOptional);
	if (Lp == NULL)	
		return false;

	book.AttachDispatch(Lp);
	sheets.AttachDispatch(book.get_Sheets());

	return true;
}

// Получение имени книги
CString CWorkExcel::BookName() 
{
	CString result = book.get_Name();
	int startPos = result.ReverseFind(L'.');
	result.Delete(startPos, result.GetLength() - startPos);
	return result;
}

// Получение имени книги из пути
CString CWorkExcel::BookName(const CString& path) 
{
	int posSlash = path.ReverseFind(L'\\');
	return path.Mid(posSlash + 1, path.GetLength());
}

// Открытие листа
bool CWorkExcel::OpenSheet(const long& index) 
{
	LPDISPATCH Lp = sheets.get_Item(COleVariant(index));
	if (Lp == NULL)	
		return false;
	
	CRange range;
	sheet.AttachDispatch(Lp);
	Lp = sheet.get_UsedRange();
	range.AttachDispatch(Lp);

	VARIANT items = range.get_Value2();
	// TODO: Вылетает ошибка, когда подаешь пустой excel
	if (cells != NULL)
		delete cells;

	// Получение границ листа
	cells = new COleSafeArray(items);
	cells->GetLBound(1, &first.row);
	cells->GetLBound(2, &first.column);
	cells->GetUBound(1, &last.row);
	cells->GetUBound(2, &last.column);

	return true;
}

// Получение имени текущего листа
CString CWorkExcel::SheetName()	{ return sheet.get_Name(); }

// Получение количества листов в книге
long CWorkExcel::CountSheets()	{ return sheets.get_Count(); }

// Получение значения линии передачи
bool CWorkExcel::IsArinc() 
{
	CString line;
	Cell cell;
	bool result;

	if (FindCell(LINE_FIELD, cell))
		line = CellValue(cell.row, cell.column + 1);

	line.Find(ARINC) != -1 ? result = true : result = false;
	return result;
}

// Получение номера набора
int CWorkExcel::NpValue(const CString& comment) 
{
	CString item = comment;

	if (!item.IsEmpty())
	{
		int pos = item.Find(NP_FIELD);
		if (pos == -1)	// Поле не найдено
			return -1;
		item.Delete(0, pos + 3);
		item.Trim();
		return _wtoi(item);
	}
	return -1;
}

// Получение номера набора
int CWorkExcel::NpValue(const Header& head)
{
	if (head.adress[head.iRow] + 1 > last.row || head.adress[head.iComment] > last.column)
		return -1;

	CString item = CellValue(head.adress[head.iRow] + 1, head.adress[head.iComment]);
	if (!item.IsEmpty())
	{
		int pos = item.Find(NP_FIELD);
		item.Delete(0, pos + 3);
		item.Trim();
		return _wtoi(item);
	}
	return 0;
}

// Получение значения номера подкадра
int CWorkExcel::PkValue(const Header& head) 
{
	if (head.adress[head.iRow] - 1 < first.row || head.adress[head.iComment] > last.column)
		return 0;

	// Костыль (цикл) для прохода по таблице вправо, для поиска номера подкадра
	for (long i = head.adress[head.iComment]; i <= last.column; i++)
	{
		CString item = CellValue(head.adress[head.iRow] - 1, i);
		if (!item.IsEmpty())
		{
			int pos = item.ReverseFind(PK_FIELD);
			if (pos == -1)         
				continue;
			item.Delete(0, pos + 1);
			item.Trim();

			// Вдруг дальше пробел
			pos = item.Find(L' ');
			if (pos != -1)
				item.Delete(pos, item.GetLength());

			// А вдруг перечисление через запятую (протоколы не поймешь)
			pos = item.Find(L',');
			if (pos != -1)
				item.Delete(pos, item.GetLength());

			int result = _wtoi(item);
			if (result == 0)	
				result = PK_FAILED;	

			return result;
		}
	}
	return PK_EMPTY;	
}

// Получение значения ячейки
CString CWorkExcel::CellValue(const Cell& cell) 
{
	VARIANT item;
	if (cell.row > last.row || cell.column > last.column)
		throw ExcelOverflow(BookName(), SheetName());
	long index[2] = { cell.row, cell.column };
	cells->GetElement(index, &item);
	CString result(item);
	result.Trim();
	return result;
}

CString CWorkExcel::CellValue(const long& row, const long& column) 
{
	VARIANT item;
	if (row > last.row || column > last.column)
		throw ExcelOverflow(BookName(), SheetName());
	long index[2] = { row, column };
	cells->GetElement(index, &item);
	CString result(item);
	result.Trim();
	return result;
}

// Получение количества строк в листе
long CWorkExcel::CountRows() { return last.row; }

// Поиск заголовков на текущем листе
bool CWorkExcel::FindHeader(Header& header) 
{
	for (size_t i = 0; i < header.list.size(); i++) 
	{
		std::vector<CString>::iterator it = header.list[i].begin();
		bool bFind = false;
		Cell cell = first;
		
		for (it; !bFind; it++) 
		{
			if (!IsArinc() && i + 1 == header.iAdress)
				break;
			if ((it == header.list[i].end()) && !bFind)
				return false;
			bFind = FindCell(*it, cell);
		}
		if (!IsArinc() && i + 1 == header.iAdress) 
		{
			header.adress[i + 1] = -1;
			continue;
		}
			
		header.adress[header.iRow] = cell.row;
		header.adress[i + 1] = cell.column;
	}
	return true;
}

// Поиск ячейки по содержимому, в противном cell(-1,-1)
bool CWorkExcel::FindCell(const CString& field, Cell& cell)
{
	for (long i = first.row; i <= last.row; i++) 
		for (long j = first.column; j <= last.column; j++) 
		{
			Cell tmp;
			tmp.row = i; tmp.column = j;
			CString item = CellValue(tmp);

			if (item.CompareNoCase(field) == 0)
			{
				cell = tmp;
				return true;
			}
		}
	return false;
}

// Кол-во пустых ячеек после 
long CWorkExcel::CPrevEmpty(const long& row, const long& column) 
{
	CString field = CellValue(row, column);
	long tmpRow = row;

	// Вверх до первого непустого значения
	while (field.IsEmpty() && tmpRow > first.row)
	{
		tmpRow--;
		field = CellValue(tmpRow, column);
	}

	return row - tmpRow;
}

// Кол-во пустых ячеек после 
long CWorkExcel::CNextEmpty(const long& row, const long& column) 
{
	CString field = CellValue(row, column);
	long tmpRow = row;

	do // Вниз до первого непустого значения
	{
		tmpRow++;
		if (tmpRow <= last.row) 
			field = CellValue(tmpRow, column);
	} 
	while (field.IsEmpty() && tmpRow <= last.row);

	return tmpRow - row;
}

// Количество слитых ячеек
long CWorkExcel::GetMerge(long& row, const long& column) 
{
	CString cell;
	cell.Format(L"%s%d", LongToChar(column), row);
	
	CRange range = sheet.get_Range(COleVariant(cell), COleVariant(cell));
	range = range.get_MergeArea();
	range = range.get_Rows();
	row = range.get_Row();

	return range.get_Count();
}

// Преобразование long к char
CString CWorkExcel::LongToChar(const long& column) 
{
	CString result;
	(column <= 26) ? result.Format(L"%c", static_cast<char>(column + 64)) : StepLongToChar(column, result);
	return result;
}

// Если в обозначении ячейки уже больше одной буквы
void CWorkExcel::StepLongToChar(const long& column, CString& result) 
{
	long iA = column / 26;
	long iB = column % 26;

	if (iB == 0)	
		iB = 26;
	result.Format(L"%s%s%s", result, LongToChar(iA), static_cast<char>(iB + 64));
}