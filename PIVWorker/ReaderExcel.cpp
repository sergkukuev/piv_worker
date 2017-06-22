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
		getSheets(book.sheets, work);
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
void CReaderExcel::getSheets(vector <sheetData>& sheets, CWorkExcel& work) {
	sheets.resize(work.countSheets());
	try {
		for (long i = 1; i < work.countSheets(); i++) {
			work.openSheet((long)i);
			// Поиск заголовков на листе
			if (!work.findHeader(header)) {
				NotAllHeaderException exc;
				exc.setParam(sheets[i - 1].name, work.bookName());
				throw exc;
			}

			sheets[i - 1].name = work.sheetName();
			sheets[i - 1].line = work.lineValue();
			sheets[i - 1].np = work.npValue(header);
			sheets[i - 1].pk = work.pkValue(header);

			header.adress[header.iRow]++;

			getSignals(sheets[i - 1].signals, work);
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
}

// Чтение параметров на листе
void CReaderExcel::getSignals(vector <signalData>& signals, CWorkExcel& work) {

	for (long i = header.adress[header.iRow]; i < work.countRows() + 1; i++) {
		signalData signal;
		long row, column;

		row = i;
		// Чтение параметров
		// Наименование параметра и обозначение сигнала
		column = header.adress[header.iName];
		signal.title[0] = work.cellValue(row, column);

		column = header.adress[header.iSignal];
		signal.title[1] = work.cellValue(row, column);

		// Чтение номера слова
		column = header.adress[header.iNumWord];
		signal.numWord = work.cellValue(row, column);
	
		// Чтение размерности, min, max и csr
		column = header.adress[header.iDimension];
		signal.dimension = work.cellValue(row, column);

		column = header.adress[header.iMin];
		signal.min = work.cellValue(row, column);

		column = header.adress[header.iMax];
		signal.max= work.cellValue(row, column);
		
		column = header.adress[header.iCSR];
		signal.csr = work.cellValue(row, column);

		// Чтение разрядов
		column = header.adress[header.iBits];
		signal.bit = work.cellValue(row, column);

		// Чтение комментариев
		column = header.adress[header.iComment];
		signal.comment = work.cellValue(row, column);

		bool bEmpty = isEmpty(work, row);
		bool bRemark = isRemark(work, row);

		// Добавление сигнала
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
	}
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
		long column = header.adress[i];
		CString tmp = work.cellValue(row, column);
		result = (tmp.Find(_T("Примечания:")) > -1 ||
			tmp.Find(_T("Примечание:")) > -1) ? true : result;
	}

	return result;
}

// Проверка расширения
bool CReaderExcel::checkExtension(CString path) {
	bool result = false;
	int dotPos = path.ReverseFind(_T('.'));	// Поиск точечки
	path = path.Mid(dotPos + 1);			// Выделение расширения из пути

	for (size_t i = 0; i < extension.size(); i++)
		if (path.CompareNoCase(extension[i]) == 0)
			result = true;

	return result;
}