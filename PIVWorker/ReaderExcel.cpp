#include "stdafx.h"
#include "ReaderExcel.h"

// Конструктор
CReaderExcel::CReaderExcel()
{
	// Установка набора заголовков
	HeaderTable.push_back(list<CString>{ _T("№ слов"), _T("№ п/п"), _T("№ слова в подадресе") });
	HeaderTable.push_back(list<CString>{ _T("Наименование параметра"), _T("Наименование сигнала") });
	HeaderTable.push_back(list<CString>{ _T("Обозначение сигнала"), _T("Условное обозначение параметра / сигнала") });
	HeaderTable.push_back(list<CString>{ _T("Размерность"), _T("Единица измерения") });
	HeaderTable.push_back(list<CString>{ _T("Минимальное значение"), _T("Миним. значение"), _T("Минимал значение") });
	HeaderTable.push_back(list<CString>{ _T("Максимальное значение"), _T("Максим. значение"), _T("Максимал значение") });
	HeaderTable.push_back(list<CString>{ _T("Цена старшего разряда") });
	HeaderTable.push_back(list<CString>{ _T("Используемые разряды"), _T("Используе-мые разряды") });
	HeaderTable.push_back(list<CString>{ _T("Примечание") });

	// Добавление всех видов расширений excel файлов
	extension.push_back(_T("xlsx"));
	extension.push_back(_T("xlsm"));
	extension.push_back(_T("xlsb"));
	extension.push_back(_T("xltx"));
	extension.push_back(_T("xltm"));
	extension.push_back(_T("xls"));
	extension.push_back(_T("xml"));
	extension.push_back(_T("xlam"));
	extension.push_back(_T("xla"));
	extension.push_back(_T("xlw"));
	extension.push_back(_T("xlr"));

	iHeader = new int[SIZE_IHEADER];
}

// Деструктор
CReaderExcel::~CReaderExcel()
{
	HeaderTable.clear();
	extension.clear();
}

// Чтение одной книги
bookData CReaderExcel::getBook(CString pathToExcel)
{
	if (!checkExtension(pathToExcel))
		throw BadTypeException();

	CWorkExcel work;
	bookData book;

	work.openWorkBook(pathToExcel);

	if (work.getCountBooks() == 0)
		throw NoBooksException();

	long clBook = (long)1;
	work.setActivBook(clBook);

	book.nameBook = work.getNameBook();
	book.sheets.resize(work.getCountSheets());

	book.sheets = getSheets(work);

	work.closeWorkBooks();

	return book;
}

// Чтение книг
vector <bookData> CReaderExcel::getBooks(vector <CString> pathToExcel)
{
	for (size_t i = 0; i < pathToExcel.size(); i++)
		if (!checkExtension(pathToExcel[i]))
			throw BadTypeException();

	CWorkExcel work;
	vector <bookData> books;

	for (size_t i = 0; i < pathToExcel.size(); i++)
		work.openWorkBook(pathToExcel[i]);

	if (work.getCountBooks() == 0)
		throw NoBooksException();

	books.resize(work.getCountBooks());

	for (int i = 1; i < books.size() + 1; i++)
	{
		long clBooks = static_cast<long>(i);

		work.setActivBook(clBooks);
		books[i - 1].nameBook = work.getNameBook();
		books[i - 1].sheets.resize(work.getCountSheets());

		books[i - 1].sheets = getSheets(work);
	}

	work.closeWorkBooks();

	return books;
}

// Чтение листов
vector <sheetData> CReaderExcel::getSheets(CWorkExcel& work)
{
	vector <sheetData> sheets;
	sheets.resize(work.getCountSheets());

	for (int i = 1; i < work.getCountSheets() + 1; i++)
	{
		long iSheet = static_cast<long> (i);

		work.setActivSheet(iSheet);
		sheets[i - 1].nameSheet = work.getNameSheet();
		sheets[i - 1].iCommentFieldNP = -1;
		sheets[i - 1].iNumPodKadra = -1;

		if (!findHeader(work))
			throw NotAllHeaderException();

		sheets[i - 1].bErrorExcelSheet = true;
		sheets[i - 1].bErrorSheet = true;

		sheets[i - 1].iNumPodKadra = getNumPK(work);

		iHeader[INDEX_ROW]++;

		sheets[i - 1].signals = getSignals(work);
	}

	return sheets;
}

// Поиск номера кадра (в противном случае будет равен -1)
int CReaderExcel::getNumPK(CWorkExcel& work)
{
	long lRows = static_cast<long> (iHeader[INDEX_ROW] - 1);
	long lColumns = static_cast<long> (iHeader[INDEX_COMMENTS]);
	int iNumPK = -1;

	if (work.getCellValue(lRows, lColumns) != "")
	{
		CString sNumPK = work.getCellValue(lRows, lColumns);
		int i = 0;

		while (i != -1)
		{
			i = sNumPK.Find(_T("№"), i == 0 ? i : i + 1);
			i = i == sNumPK.GetLength() ? -1 : i;
			iNumPK = i == -1 ? iNumPK : i;
		}

		if (iNumPK != -1)
			sNumPK.Delete(0, iNumPK + 1);

		sNumPK = sNumPK.Trim();

		regex regStr("[[:digit:]]+");
		string stdStr = convertString(sNumPK);

		if (regex_match(stdStr, regStr))
			iNumPK = atoi(stdStr.c_str());
	}

	return iNumPK;
}

// Чтение параметров на листе
list <signalData> CReaderExcel::getSignals(CWorkExcel& work)
{
	list <signalData> signals;
	bool bRemark = false;	// Примечание
	int cEmpty = 0;			// Количество пустых строк

	do
	{
		signalData signal;
		adrCell cell;

		cell.row = static_cast<long> (iHeader[INDEX_ROW]);
		
		// Чтение параметров:
		// Наименование параетра и обозначение сигнала
		cell.column = static_cast<long> (iHeader[INDEX_NAME]);
		signal.sTitleParamField[0] = getCell(work, cell);
		long cMergeName = work.getMergeCount(cell.row, cell.column);

		cell.column = static_cast<long> (iHeader[INDEX_SIGNAL]);
		signal.sTitleParamField[1] = getCell(work, cell);
		long cMergeSignal = work.getMergeCount(cell.row, cell.column);

		if (cMergeName != cMergeSignal)
			signal.bTitleParamField = false;
		else
			signal.bTitleParamField = true;

		// Чтение номера слова
		cell.column = static_cast<long> (iHeader[INDEX_NUM]);
		signal.sNumWordField = getCell(work, cell, cMergeName);
		
		if (signal.sNumWordField.Find(_T(",")) != -1)
			signal.b2NumWordField = false;
		else
			signal.b2NumWordField = true;

		signal.bNumWordField = true;
		signal.bRepitNumWordField = false;

		// Чтение размерности, min, max и csr
		cell.column = static_cast<long> (iHeader[INDEX_DIMENSION]);
		signal.sDimensionField = getCell(work, cell, cMergeName);

		cell.column = static_cast<long> (iHeader[INDEX_MIN]);
		signal.sMinMaxCsrValField[0] = getCell(work, cell, cMergeName);
		signal.bMinValField = true;

		cell.column = static_cast<long> (iHeader[INDEX_MAX]);
		signal.sMinMaxCsrValField[1] = getCell(work, cell, cMergeName);
		signal.bMaxValField = true;

		cell.column = static_cast<long> (iHeader[INDEX_CSR]);
		signal.sMinMaxCsrValField[2] = getCell(work, cell, cMergeName);
		signal.bCsrValField = true;

		// Чтение разрядов
		cell.column = static_cast<long> (iHeader[INDEX_BITS]);
		signal.sBitField = getCell(work, cell, cMergeName);
		signal.bBitField = true;
		signal.b2BitField = false;
		signal.iBitSigns = 0;
		signal.bBitSigns = true;

		// Чтение комментариев
		cell.column = static_cast<long> (iHeader[INDEX_COMMENTS]);
		signal.sCommentField = getCell(work, cell, cMergeName);
		signal.bCommentField = false;

		bool bEmpty = IsEmpty(work, cell.row);
		bRemark = IsRemark(work, cell.row);

		// Добавление сигнала
		if (!bEmpty && !bRemark)
			signals.push_back(signal);

		iHeader[INDEX_ROW] += cMergeName;

	} while (cEmpty < 5 && !bRemark);

	return signals;
}

bool CReaderExcel::IsEmpty(CWorkExcel& work, long row)
{
	adrCell cell;
	bool result;
	cell.row = row;

	for (long i = 1; i < SIZE_IHEADER; i++)
	{
		cell.column = static_cast <long>(i);
		if (work.getCellValue(cell.row, cell.column) != "")
			result = false;
	}

	return result;
}

bool CReaderExcel::IsRemark(CWorkExcel& work, long row)
{
	adrCell cell;
	bool result;
	cell.row = row;

	for (long i = 1; i < SIZE_IHEADER; i++)
	{
		cell.column = static_cast<long> (iHeader[i]);
		result = (work.getCellValue(cell.row, cell.column).Find(_T("Примечания:")) > -1 ||
			work.getCellValue(cell.row, cell.column).Find(_T("Примечание:")) > -1) ? true : false;
	}

	return result;
}

// Чтение ячейки
CString CReaderExcel::getCell(CWorkExcel& work, adrCell cell, long cName)
{
	long size = work.getMergeCount(cell.row, cell.column);
	CString result;

	if (size < cName)
		size = cName;

	long start = work.getStartMerge(cell.row, cell.column);
	for (long i = start; i < start + size; i++)
	{
		CString tmp = work.getCellValue(i, cell.column);

		if (i != (start + size - 1) || (!result.IsEmpty() && !tmp.IsEmpty()))
			result += _T(", ");

		if (!tmp.IsEmpty())
			result += work.getCellValue(i, cell.column);
	}

	return result;
}

// Функция преобразования CString в string
string CReaderExcel::convertString(CString cStr)
{
	int bytes = WideCharToMultiByte(CP_ACP, 0, cStr.GetBuffer(), -1, 0, 0, NULL, NULL);
	char *buffer = new char[bytes];

	WideCharToMultiByte(CP_ACP, 0, cStr.GetBuffer(), -1, buffer, bytes, NULL, NULL);

	string result(buffer);

	delete buffer;
	buffer = nullptr;

	return result;
}

// Проверка расширения
bool CReaderExcel::checkExtension(CString path)
{
	int posDot = path.Find(_T("."), path.GetLength() - 5);

	CString pathExtension = path.Mid(posDot + 1);

	for (size_t i = 0; i < extension.size(); i++)
		if (pathExtension.CompareNoCase(extension[i]) == 0)
			return true;

	return false;
}

// Поиск индексов заголовков
bool CReaderExcel::findHeader(CWorkExcel& work)
{
	vector<list<CString>>::iterator itV = HeaderTable.begin();
	adrCell cell;		// Адрес ячейки заголовка
	bool result = true;	// Найдены ли заголовки

	for (int i = 1; itV != HeaderTable.end(); itV++, i++)
	{
		list<CString>::iterator itL = (*itV).begin();
		bool bEnd = true;

		while (itL != (*itV).end() || bEnd)
		{
			work.findOneDateCells(*itL, cell.row, cell.column);	// Поиск адреса ячейки с именем заголовка

			if (cell.row == -1)
			{
				if (itL == (*itV).end())
					result = bEnd = false;
			}
			else
			{
				//setHeader(i, cell);
				iHeader[i] = cell.column;
				iHeader[i] = cell.row;
				bEnd = false;
			}
			itL++;
		}
	}

	return result;
}

// Установка заголовка
void CReaderExcel::setHeader(int index, adrCell cell)
{
	/*if (index == INDEX_NUM)
	{
		iHeader.iNumber = cell.column;
		iHeader.iRows = cell.row;
	}
	else if (index == INDEX_NAME)
		iHeader.iName = cell.column;
	else if (index == INDEX_SIGNAL)
		iHeader.iNameSignal = cell.column;
	else if (index == INDEX_DIMENSION)
		iHeader.iDimension = cell.column;
	else if (index == INDEX_MIN)
		iHeader.iMin = cell.column;
	else if (index == INDEX_MAX)
		iHeader.iMax = cell.column;
	else if (index == INDEX_CSR)
		iHeader.iCSR = cell.column;
	else if (index == INDEX_BITS)
		iHeader.iBits = cell.column;
	else if (index = INDEX_COMMENTS)
		iHeader.iComments = cell.column;*/
}
