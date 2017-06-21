#include "stdafx.h"
#include "ReaderExcel.h"
#include <ctime>

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
	unsigned int s = clock();
	if (!checkExtension(pathToExcel))
		throw BadTypeException();

	CWorkExcel work;
	bookData book;

	work.openBook(pathToExcel);
	book.name = work.bookName();

	book.sheets = getSheets(work);
	
	unsigned int e = clock();
	unsigned int res = e - s;
	
	return book;
}

// Чтение листов
vector <sheetData> CReaderExcel::getSheets(CWorkExcel& work)
{
	vector <sheetData> sheets;
	sheets.resize(work.countSheet());

	for (long i = 1; i < work.countSheet(); i++)
	{
		long iSheet = static_cast<long> (i);

		work.setActiveSheet(iSheet);
		sheets[i - 1].name = work.sheetName();
		sheets[i - 1].iFieldNP = -1;
		sheets[i - 1].iNumPK = -1;

		//if (!findHeader(work))		// Поиск заголовков на листе
			//throw NotAllHeaderException();
		header.adress[header.iRow] = 8;
		header.adress[1] = 1;
		header.adress[2] = 2;
		header.adress[3] = 3;
		header.adress[4] = 4;
		header.adress[5] = 5;
		header.adress[6] = 6;
		header.adress[7] = 7;
		header.adress[8] = 8;
		header.adress[9] = 9;

		sheets[i - 1].bError = false;

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
	
	long row = static_cast<long> (header.adress[header.iRow] - 1);
	long column = static_cast<long> (header.adress[header.iComment]);

	int iNumPK = -1;	// Индекс подкадра

	CString res = work.cellValue(row, column);
	if (res != "")
	{
		CString sNumPK = work.cellValue(row, column);
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
		iNumPK = _wtoi(sNumPK);
	}

	return iNumPK;
}

// Чтение параметров на листе
list <signalData> CReaderExcel::getSignals(CWorkExcel& work)
{
	list <signalData> signals;
	bool bRemark = false;	// Является ли строка примечанием
	int cEmpty = 0;			// Счетчик пустых строк

	for (int i = header.adress[header.iRow]; i < work.countSignal() + 1; i++)
	{
		signalData signal;
		long row, column;

		row = static_cast<long>(i);
		
		// Чтение параметров:
		// Наименование параметра и обозначение сигнала
		column = static_cast<long> (header.adress[header.iName]);
		signal.sTitleParamField[0] = work.cellValue(row, column);

		column = static_cast<long> (header.adress[header.iSignal]);
		signal.sTitleParamField[1] = work.cellValue(row, column);
		signal.bTitleParamField = false;

		// Чтение номера слова
		column = static_cast<long> (header.adress[header.iNumWord]);
		signal.sNumWordField = work.cellValue(row, column);
		
		signal.b2NumWordField = false;
		signal.bNumWordField = false;

		// Чтение размерности, min, max и csr
		column = static_cast<long> (header.adress[header.iDimension]);
		signal.sDimensionField = work.cellValue(row, column);

		column = static_cast<long> (header.adress[header.iMin]);
		signal.sMinMaxCsrValField[0] = work.cellValue(row, column);
		signal.bMinValField = false;

		column = static_cast<long> (header.adress[header.iMax]);
		signal.sMinMaxCsrValField[1] = work.cellValue(row, column);
		signal.bMaxValField = false;

		column = static_cast<long> (header.adress[header.iCSR]);
		signal.sMinMaxCsrValField[2] = work.cellValue(row, column);
		signal.bCsrValField = false;

		// Чтение разрядов
		column = static_cast<long> (header.adress[header.iBits]);
		signal.sBitField = work.cellValue(row, column);
		signal.bBitField = false;
		signal.b2BitField = false;
		signal.iBitSigns = 0;
		signal.bBitSigns = false;

		// Чтение комментариев
		column = static_cast<long> (header.adress[header.iComment]);
		signal.sCommentField = work.cellValue(row, column);
		signal.bCommentField = false;

		bool bEmpty = IsEmpty(work, row);
		bRemark = IsRemark(work, row);

		if (bEmpty)
			cEmpty++;	// инкрементация счетчика пустых строк
		else
			cEmpty = 0;	// сброс счетчика

		// Добавление сигнала
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
	}

	return signals;
}

// Проверка строки на пустоту
bool CReaderExcel::IsEmpty(CWorkExcel& work, long row)
{
	bool result = true;

	for (long i = 1; i < header.size; i++)
	{
		long column = static_cast<long> (header.adress[i]);
		CString res = work.cellValue(row, column);
		if (res != "")
			result = false;
	}

	return result;
}

// Является ли строка примечанием
bool CReaderExcel::IsRemark(CWorkExcel& work, long row)
{
	bool result = false;

	for (long i = 1; i < header.size; i++)
	{
		long column = static_cast<long> (header.adress[i]);
		CString res = work.cellValue(row, column);
		result = (res.Find(_T("Примечания:")) > -1 ||
			res.Find(_T("Примечание:")) > -1) ? true : result;
	}

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
/*bool CReaderExcel::findHeader(CWorkExcel& work)
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

			bFind = work.findHeader(*it, cell);
		}
		header.adress[header.iRow] = cell.row;
		header.adress[i + 1] = cell.column;
	}

	return true;
}*/
