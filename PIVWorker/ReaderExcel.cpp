#include "stdafx.h"
#include "ReaderExcel.h"

// Конструктор
CReaderExcel::CReaderExcel()
{
	// Установка набора заголовков
	header.list.push_back(list<CString>{ _T("№ слов"), _T("№ п/п"), _T("№ слова в подадресе") });						// Для номера слова
	header.list.push_back(list<CString>{ _T("Наименование параметра"), _T("Наименование сигнала") });					// Для наименование параметра
	header.list.push_back(list<CString>{ _T("Обозначение сигнала"), _T("Условное обозначение параметра / сигнала") });	// Для сигнала
	header.list.push_back(list<CString>{ _T("Размерность"), _T("Единица измерения") });									// Для единицы изменерения
	header.list.push_back(list<CString>{ _T("Минимальное значение"), _T("Миним. значение"), _T("Минимал значение") });	// Минимальное значение
	header.list.push_back(list<CString>{ _T("Максимальное значение"), _T("Максим. значение"), _T("Максимал значение") });// Максимальное значение
	header.list.push_back(list<CString>{ _T("Цена старшего разряда") });												// Для цср
	header.list.push_back(list<CString>{ _T("Используемые разряды"), _T("Используе-мые разряды") });					// Для используемых разрядов
	header.list.push_back(list<CString>{ _T("Примечание") });															// Комментарии (пояснения)

	// Добавление всех видов расширений excel файлов
	extension.push_back(_T("xlsx"));	// Текущий основной формат
	extension.push_back(_T("xlsm"));
	extension.push_back(_T("xlsb"));
	extension.push_back(_T("xltx"));
	extension.push_back(_T("xltm"));
	extension.push_back(_T("xls"));		// Основной формат до 2003 года
	extension.push_back(_T("xml"));
	extension.push_back(_T("xlam"));
	extension.push_back(_T("xla"));
	extension.push_back(_T("xlw"));
	extension.push_back(_T("xlr"));
}

// Деструктор
CReaderExcel::~CReaderExcel()
{
	header.list.clear();
	extension.clear();
}

// Чтение одной книги
bookData CReaderExcel::getBook(CString pathToExcel)
{
	if (!checkExtension(pathToExcel))
		throw BadTypeException();

	CWorkExcel work;
	bookData book;

	work.openWorkBook(pathToExcel);	// Открытие книги

	if (work.getCountBooks() == 0)
		throw NoBooksException();

	long clBook = (long)1;
	work.setActivBook(clBook);

	book.name = work.getNameBook();
	book.sheets.resize(work.getCountSheets());

	book.sheets = getSheets(work);

	work.closeWorkBooks();	// Закрытие книги

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

	// Открытие книг
	for (size_t i = 0; i < pathToExcel.size(); i++)
		work.openWorkBook(pathToExcel[i]);

	if (work.getCountBooks() == 0)
		throw NoBooksException();

	books.resize(work.getCountBooks());

	for (int i = 1; i < books.size() + 1; i++)
	{
		long clBooks = static_cast<long>(i);

		work.setActivBook(clBooks);
		books[i - 1].name = work.getNameBook();
		books[i - 1].sheets.resize(work.getCountSheets());

		books[i - 1].sheets = getSheets(work);
	}

	work.closeWorkBooks();	// Закрытие книг

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
		sheets[i - 1].name = work.getNameSheet();
		sheets[i - 1].iFieldNP = -1;
		sheets[i - 1].iNumPK = -1;

		if (!findHeader(work))		// Поиск заголовков на листе
			throw NotAllHeaderException();

		sheets[i - 1].bError = true;

		sheets[i - 1].iNumPK = getNumPK(work);

		header.adress[header.iRow]++;

		sheets[i - 1].signals = getSignals(work);
	}

	return sheets;
}

// Поиск номера кадра (в противном случае будет равен -1)
int CReaderExcel::getNumPK(CWorkExcel& work)
{
	// Переход к столбцу примечаний
	Cell cell;
	cell.row = static_cast<long> (header.adress[header.iRow] - 1);
	cell.column = static_cast<long> (header.adress[header.iComment]);

	int iNumPK = -1;	// Индекс подкадра

	if (work.getCellValue(cell) != "")
	{
		CString sNumPK = work.getCellValue(cell);
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
	bool bRemark = false;	// Является ли строка примечанием
	int cEmpty = 0;			// Счетчик пустых строк

	do
	{
		signalData signal;
		Cell cell;

		cell.row = static_cast<long> (header.adress[header.iRow]);
		
		// Чтение параметров:
		// Наименование параетра и обозначение сигнала
		cell.column = static_cast<long> (header.adress[header.iName]);
		signal.sTitleParamField[0] = getCell(work, cell);
		long cMergeName = work.getMergeCount(cell);

		cell.column = static_cast<long> (header.adress[header.iSignal]);
		signal.sTitleParamField[1] = getCell(work, cell);

		signal.bTitleParamField = true;

		// Чтение номера слова
		cell.column = static_cast<long> (header.adress[header.iNumWord]);
		signal.sNumWordField = getCell(work, cell, cMergeName);
		
		signal.b2NumWordField = true;
		signal.bNumWordField = true;

		// Чтение размерности, min, max и csr
		cell.column = static_cast<long> (header.adress[header.iDimension]);
		signal.sDimensionField = getCell(work, cell, cMergeName);

		cell.column = static_cast<long> (header.adress[header.iMin]);
		signal.sMinMaxCsrValField[0] = getCell(work, cell, cMergeName);
		signal.bMinValField = true;

		cell.column = static_cast<long> (header.adress[header.iMax]);
		signal.sMinMaxCsrValField[1] = getCell(work, cell, cMergeName);
		signal.bMaxValField = true;

		cell.column = static_cast<long> (header.adress[header.iCSR]);
		signal.sMinMaxCsrValField[2] = getCell(work, cell, cMergeName);
		signal.bCsrValField = true;

		// Чтение разрядов
		cell.column = static_cast<long> (header.adress[header.iBits]);
		signal.sBitField = getCell(work, cell, cMergeName);
		signal.bBitField = true;
		signal.b2BitField = true;
		signal.iBitSigns = 0;
		signal.bBitSigns = true;

		// Чтение комментариев
		cell.column = static_cast<long> (header.adress[header.iComment]);
		signal.sCommentField = getCell(work, cell, cMergeName);
		signal.bCommentField = false;

		bool bEmpty = IsEmpty(work, cell.row);
		bRemark = IsRemark(work, cell.row);

		if (bEmpty)
			cEmpty++;	// инкрементация счетчика пустых строк
		else
			cEmpty = 0;	// сброс счетчика

		// Добавление сигнала
		if (!bEmpty && !bRemark)
			signals.push_back(signal);

		header.adress[header.iRow] += cMergeName;

	} while (cEmpty < 5 && !bRemark);

	return signals;
}

// Проверка строки на пустоту
bool CReaderExcel::IsEmpty(CWorkExcel& work, long row)
{
	bool result = true;
	Cell cell;

	cell.row = row;
	for (long i = 1; i < header.size; i++)
	{
		cell.column = static_cast<long> (header.adress[i]);
		if (work.getCellValue(cell) != "")
			result = false;
	}

	return result;
}

// Является ли строка примечанием
bool CReaderExcel::IsRemark(CWorkExcel& work, long row)
{
	bool result = true;
	Cell cell;

	cell.row = row;
	for (long i = 1; i < header.size; i++)
	{
		cell.column = static_cast<long> (header.adress[i]);
		result = (work.getCellValue(cell).Find(_T("Примечания:")) > -1 ||
			work.getCellValue(cell).Find(_T("Примечание:")) > -1) ? true : false;
	}

	return result;
}

// Чтение ячейки
CString CReaderExcel::getCell(CWorkExcel& work, Cell cell, long cName)
{
	CString result;
	long size = work.getMergeCount(cell);

	// Если слитая ячейка наименования больше, чем текущее, значит читаем все ячейки в количестве слитых ячеек наименования
	if (cName > size)	
	{
		long start = work.getStartMerge(cell);
		for (long i = start; i < start + cName; i++)
		{
			cell.row = i;
			CString tmp = work.getCellValue(cell);

			if (i != (start + size - 1) || (!result.IsEmpty() && !tmp.IsEmpty()))
				result += _T(", ");

			if (!tmp.IsEmpty())
				result += work.getCellValue(cell);
		}
	}
	else
	{
		cell.row = work.getStartMerge(cell);
		result = work.getCellValue(cell);
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
	int posDot = path.Find(_T("."), path.GetLength() - 5);	// Поиск точечки

	CString pathExtension = path.Mid(posDot + 1);	// Выделение расширения из пути

	for (size_t i = 0; i < extension.size(); i++)
		if (pathExtension.CompareNoCase(extension[i]) == 0)
			return true;

	return false;
}

// Поиск индексов заголовков
bool CReaderExcel::findHeader(CWorkExcel& work)
{
	for (size_t i = 0; i < header.list.size(); i++)
	{
		list<CString>::iterator it = header.list[i].begin();
		bool bFind = false;
		Cell cell;

		for (it; !bFind; it++)	// Поиск хотя бы одного варианта заголовка из списка
		{
			if ((it == header.list[i].end()) && !bFind)	// Ничего не нашли и список закончился
				return false;

			bFind = work.findOneDateCells(*it, cell);
		}
		header.adress[header.iRow] = cell.row;
		header.adress[i + 1] = cell.column;
	}

	return true;
}
