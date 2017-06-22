#include "stdafx.h"
#include "ReaderExcel.h"

// Конструктор
CReaderExcel::CReaderExcel() {
	// Установка набора заголовков
	header.list.push_back(vector<CString>{ _T("№ слов"), _T("№ слова"), _T("№ п/п"), _T("№ слова в подадресе") });			// Для номера слова
	header.list.push_back(vector<CString>{ _T("Наименование параметра"), _T("Наименование сигнала") });						// Для наименование параметра
	header.list.push_back(vector<CString>{ _T("Обозначение сигнала"), _T("Условное обозначение параметра / сигнала") });	// Для сигнала
	header.list.push_back(vector<CString>{ _T("Размерность"), _T("Единица измерения") });									// Для единицы изменерения
	header.list.push_back(vector<CString>{ _T("Минимальное значение"), _T("Миним. значение"), _T("Минимал значение") });	// Минимальное значение
	header.list.push_back(vector<CString>{ _T("Максимальное значение"), _T("Максим. значение"), _T("Максимал значение") });	// Максимальное значение
	header.list.push_back(vector<CString>{ _T("Цена старшего разряда") });													// Для цср
	header.list.push_back(vector<CString>{ _T("Используемые разряды"), _T("Используе-мые разряды") });						// Для используемых разрядов
	header.list.push_back(vector<CString>{ _T("Примечание") });																// Комментарии (пояснения)

	// Добавление всех видов расширений excel файлов
	extension.push_back(_T("xlsx"));	// Текущий основной формат
	extension.push_back(_T("xls"));		// Основной формат до 2003 года
	/*extension.push_back(_T("xlsm"));
	extension.push_back(_T("xlsb"));
	extension.push_back(_T("xltx"));
	extension.push_back(_T("xltm"));
	extension.push_back(_T("xml"));
	extension.push_back(_T("xlam"));
	extension.push_back(_T("xla"));
	extension.push_back(_T("xlw"));
	extension.push_back(_T("xlr"));*/
}

// Деструктор
CReaderExcel::~CReaderExcel() {
	header.list.clear();
	header.list.shrink_to_fit();
	extension.clear();
	extension.shrink_to_fit();
}

// Чтение одной книги
bookData CReaderExcel::getBook(CString pathToExcel) {
	bookData book;

	try {
		if (!checkExtension(pathToExcel))
			throw BadTypeException();

		CWorkExcel work;
		work.openBook(pathToExcel);
		book.name = work.bookName();
		book.sheets = getSheets(work);
	}
	catch (COleDispatchException& exc) {
		TCHAR msg[255];
		CString msgFormated;

		exc.GetErrorMessage(msg, 255);
		msgFormated += msg;

		AccessExcelException myExc;
		myExc.setOleError(msgFormated);
		throw myExc;
	}
	
	return book;
}

// Чтение листов
vector <sheetData> CReaderExcel::getSheets(CWorkExcel& work) {
	vector <sheetData> sheets;
	sheets.resize(work.countSheets());

	try {
		for (long i = 1; i < work.countSheets(); i++) {
			work.openSheet((long)i);
			sheets[i - 1].name = work.sheetName();
			sheets[i - 1].iFieldNP = -1;
			sheets[i - 1].iNumPK = -1;

			// Поиск заголовков на листе
			if (!work.findHeader(header)) {
				NotAllHeaderException exc;
				exc.setParam(sheets[i - 1].name, work.bookName());
				throw exc;
			}

			sheets[i - 1].bError = false;

			sheets[i - 1].iNumPK = getNumPK(work);

			header.adress[header.iRow]++;

			sheets[i - 1].signals = getSignals(work);
		}
	}
	catch (COleException& exc) {
		TCHAR msg[255];
		CString msgFormated;

		exc.GetErrorMessage(msg, 255);
		msgFormated += msg;

		ReadBookException myExc;
		myExc.setParam(msgFormated, work.bookName());
		throw myExc;
	}

	return sheets;
}

// Поиск номера кадра (в противном случае будет равен -1)
int CReaderExcel::getNumPK(CWorkExcel& work) {
	// Переход к столбцу примечаний
	long row = header.adress[header.iRow] - 1;
	long column = header.adress[header.iComment];

	int iNumPK = -1;	// Индекс подкадра

	CString sNumPK = work.cellValue(row, column);
	if (!sNumPK.IsEmpty()) {
		int i = 0;

		while (i != -1) {
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
list <signalData> CReaderExcel::getSignals(CWorkExcel& work) {
	list <signalData> signals;

	for (long i = header.adress[header.iRow]; i < work.countRows() + 1; i++) {
		signalData signal;
		long row, column;

		row = i;
		
		// Чтение параметров:
		// Наименование параметра и обозначение сигнала
		column = header.adress[header.iName];
		signal.sTitleParamField[0] = work.cellValue(row, column);

		column = header.adress[header.iSignal];
		signal.sTitleParamField[1] = work.cellValue(row, column);
		signal.bTitleParamField = false;

		// Чтение номера слова
		column = header.adress[header.iNumWord];
		signal.sNumWordField = work.cellValue(row, column);
		
		signal.b2NumWordField = false;
		signal.bNumWordField = false;

		// Чтение размерности, min, max и csr
		column = header.adress[header.iDimension];
		signal.sDimensionField = work.cellValue(row, column);

		column = header.adress[header.iMin];
		signal.sMinMaxCsrValField[0] = work.cellValue(row, column);
		signal.bMinValField = false;

		column = header.adress[header.iMax];
		signal.sMinMaxCsrValField[1] = work.cellValue(row, column);
		signal.bMaxValField = false;

		column = header.adress[header.iCSR];
		signal.sMinMaxCsrValField[2] = work.cellValue(row, column);
		signal.bCsrValField = false;

		// Чтение разрядов
		column = header.adress[header.iBits];
		signal.sBitField = work.cellValue(row, column);
		signal.bBitField = false;
		signal.b2BitField = false;
		signal.iBitSigns = 0;
		signal.bBitSigns = false;

		// Чтение комментариев
		column = header.adress[header.iComment];
		signal.sCommentField = work.cellValue(row, column);
		signal.bCommentField = false;

		bool bEmpty = isEmpty(work, row);
		bool bRemark = isRemark(work, row);

		// Добавление сигнала
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
	}

	return signals;
}

// Проверка строки на пустоту
bool CReaderExcel::isEmpty(CWorkExcel& work, long row) {
	bool result = true;

	for (long i = 1; i < header.size; i++) {
		long column = header.adress[i];
		CString tmp = work.cellValue(row, column);
		if (!tmp.IsEmpty())
			result = false;
	}

	return result;
}

// Является ли строка примечанием
bool CReaderExcel::isRemark(CWorkExcel& work, long row) {
	bool result = false;

	for (long i = 1; i < header.size; i++) {
		long column = static_cast<long> (header.adress[i]);
		CString tmp = work.cellValue(row, column);
		result = (tmp.Find(_T("Примечания:")) > -1 ||
			tmp.Find(_T("Примечание:")) > -1) ? true : result;
	}

	return result;
}

// Проверка расширения
bool CReaderExcel::checkExtension(CString path) {
	bool result = false;
	int posDot = path.Find(_T("."), path.GetLength() - 5);	// Поиск точечки
	CString pathExtension = path.Mid(posDot + 1);	// Выделение расширения из пути

	for (size_t i = 0; i < extension.size(); i++)
		if (pathExtension.CompareNoCase(extension[i]) == 0)
			result = true;

	return result;
}