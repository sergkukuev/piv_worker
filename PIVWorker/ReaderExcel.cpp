#include "stdafx.h"
#include "ReaderExcel.h"

// Конструктор
CReaderExcel::CReaderExcel()
{
}

// Деструктор
CReaderExcel::~CReaderExcel()
{
}

// Конструктор с параметром
CReaderExcel::CReaderExcel(vector <CString>& pathToExcel)
{
	path = pathToExcel;

	// Открытие книг
	for (size_t i = 0; i < path.size(); i++)
		work.openWorkBook(path[i]);

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
}

// Получить количество книг
int CReaderExcel::getSize()
{
	return work.getCountBooks();
}

// Чтение книг
vector <bookData> CReaderExcel::getBooks()
{
	vector <bookData> books;
	books.resize(work.getCountBooks());

	for (int i = 1; i < books.size() + 1; i++)
	{
		long clBooks = static_cast<long>(i);

		work.setActivBook(clBooks);
		books[i - 1].nameBook = work.getNameBook();
		books[i - 1].sheets.resize(work.getCountSheets());

		books[i - 1].sheets = getSheets();
	}

	return books;
}

// Чтение листов
vector <sheetData> CReaderExcel::getSheets()
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

		if (bHeader)	// Проверка наличия заголовков на листе
		{
			sheets[i - 1].bErrorExcelSheet = true;
			sheets[i - 1].bErrorSheet = true;
		}
		else
		{
			sheets[i - 1].bErrorExcelSheet = false;
			sheets[i - 1].bErrorSheet = false;
		}

		bHeader = findHeader();

		//установить номер подкадра

		iHeader.iRows++;

		sheets[i - 1].signals = getSignals();
	}

	return sheets;
}

// Чтение параметров на листе
list <signalData> CReaderExcel::getSignals()
{
	list <signalData> signals;
	bool bRemark = false;	// Примечание
	int cEmpty = 0;			// Количество пустых строк

	do
	{
		signalData signal;
		long lRow = static_cast<long> (iHeader.iRows);

		// Чтение параметров
		long lColumn = static_cast<long> (iHeader.iNumber);
		signal.sNumWordField = work.getCellValue(lRow, lColumn);
		signal.bNumWordField = true;
		signal.b2NumWordField = false;
		signal.bRepitNumWordField = false;

		lColumn = static_cast<long> (iHeader.iName);
		signal.sTitleParamField[0] = work.getCellValue(lRow, lColumn);

		lColumn = static_cast<long> (iHeader.iNameSignal);
		signal.sTitleParamField[1] = work.getCellValue(lRow, lColumn);
		signal.bTitleParamField = true;

		lColumn = static_cast<long> (iHeader.iDimension);
		signal.sDimensionField = work.getCellValue(lRow, lColumn);

		lColumn = static_cast<long> (iHeader.iMin);
		signal.sMinMaxCsrValField[0] = work.getCellValue(lRow, lColumn);

		lColumn = static_cast<long> (iHeader.iMax);
		signal.sMinMaxCsrValField[1] = work.getCellValue(lRow, lColumn);

		lColumn = static_cast<long> (iHeader.iCSR);
		signal.sMinMaxCsrValField[2] = work.getCellValue(lRow, lColumn);
		signal.bMinValField = true;
		signal.bMaxValField = true;
		signal.bCsrValField = true;

		lColumn = static_cast<long> (iHeader.iBits);
		signal.sBitField = work.getCellValue(lRow, lColumn);
		signal.bBitField = true;
		signal.b2BitField = false;

		signal.iBitSigns = 0;
		signal.bBitSigns = true;

		// Чтение комментариев
		lColumn = static_cast<long> (iHeader.iComments);
		signal.bCommentField = false;

		if (work.getMergeCount(lRow, lColumn) != 1)	// Если комментарий не для одной строчки
		{
			list <signalData>::iterator itS = signals.end();
			itS--;
			signal.sCommentField.push_back(*itS->sCommentField.begin());
		}
		else
			signal.sCommentField.push_back(work.getCellValue(lRow, lColumn));

		bool bEmpty = true;		// Пустая строка
		bool bOnlyOne = false;	// Хотя бы один параметр пустой
		for (int j = 1; j < 9; j++)
		{
			lColumn = static_cast <long>(j);
			if (work.getCellValue(lRow, lColumn) == "")
				bOnlyOne = true;
			else
				bEmpty = false;
		}

		if (bEmpty)
		{
			list <CString>::iterator itC = signal.sCommentField.begin();

			if (*itC != "")	// Является ли пустая строка комментарием
			{
				list <signalData>::iterator itS = signals.end();
				itS--;
				itS->sCommentField.push_back(*itC);
			}
			else
				cEmpty++;
		}
		else
		{
			cEmpty = 0;

			// Является ли строка примечанием или параметром
			for (int j = 1; j < 9; j++)
			{
				lColumn = static_cast <long> (j);
				bRemark = (work.getCellValue(lRow, lColumn).Find(_T("Примечания:")) > -1 ||
					work.getCellValue(lRow, lColumn).Find(_T("Примечания:")) > -1) ? true : false;
			}
		}

		// Добавление сигнала
		if (!bEmpty && !bRemark)
			signals.push_back(signal);

		iHeader.iRows++;

	} while (cEmpty < 5 && !bRemark);

	return signals;
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

// Поиск индексов заголовков
bool CReaderExcel::findHeader()
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
				setHeader(i, cell);
				bEnd = false;
			}
			itL++;
		}
	}

	return result;
}

// Установка заголовка
void CReaderExcel::setHeader(int index, adrCell& cell)
{
	if (index == 1)
	{
		iHeader.iNumber = cell.column;
		iHeader.iRows = cell.row;
	}
	else if (index == 2)
		iHeader.iName = cell.column;
	else if (index == 3)
		iHeader.iNameSignal = cell.column;
	else if (index == 4)
		iHeader.iDimension = cell.column;
	else if (index == 5)
		iHeader.iMin = cell.column;
	else if (index == 6)
		iHeader.iMax = cell.column;
	else if (index == 7)
		iHeader.iCSR = cell.column;
	else if (index == 8)
		iHeader.iBits = cell.column;
	else if (index = 9)
		iHeader.iComments = cell.column;
}
