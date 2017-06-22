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

		column = header.adress[header.iNumWord];
		int step = 0;
		Cell cell;
		cell.row = row; cell.column = column;
		signal.numWord = getNumWord(work, cell, step);
		
		cell.column = header.adress[header.iSignal];
		long rowID = 0; int size = 0;
		signal.title = getTitle(work, cell, rowID, size);

		// Чтение размерности, min, max и csr
		column = header.adress[header.iDimension];
		signal.dimension = work.cellValue(rowID, column);

		column = header.adress[header.iMin];
		signal.min = work.cellValue(rowID, column);

		column = header.adress[header.iMax];
		signal.max= work.cellValue(rowID, column);
		
		column = header.adress[header.iCSR];
		signal.csr = work.cellValue(rowID, column);
		
		// Чтение разрядов
		column = header.adress[header.iBits];
		signal.bit = work.cellValue(rowID, column);

		// Чтение комментариев
		cell.column = header.adress[header.iComment];
		cell.row = rowID;
		signal.comment = getComment(work, cell, size);

		bool bEmpty = isEmpty(work, row);
		bool bRemark = isRemark(work, row);

		// Добавление сигнала
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
	}
}

// Получить номера слов
vector <int> CReaderExcel::getNumWord(CWorkExcel& work, Cell cell, int& step) {
	CString field = work.cellValue(cell);
	step = 0;

	while (field.IsEmpty()) {
		cell.row--;
		field = work.cellValue(cell);
	}

	vector <int> result;
	translateNumWord(field, result);

	do {
		cell.row++;	step++;
		field = work.cellValue(cell);
	} while (field.IsEmpty() && cell.row < work.countRows());
	if (cell.row < work.countRows())
		step++;
	return result;
}

// Перевод номеров слов из строки в числа
void CReaderExcel::translateNumWord(CString numeric, vector <int>& numWord) {
	int posDot = numeric.Find(_T(','));

	if (posDot == -1) {
		numeric.Trim();	// Удаление пробелов
		numWord.push_back(_wtoi(numeric));
	}
	else {
		CString numeric2 = numeric;	
		numeric.Delete(posDot, numeric.GetLength());	// Первое число
		numeric.Trim();
		numWord.push_back(_wtoi(numeric));

		numeric2.Delete(0, posDot + 1); // Второе число
		numeric2.Trim();
		numWord.push_back(_wtoi(numeric2));
	}
}

vector <CString> CReaderExcel::getTitle(CWorkExcel& work, Cell cell, long& size, int& step) {
	CString field = work.cellValue(cell);
	size = 0;

	while (field.IsEmpty()) {
		cell.row--;
		field = work.cellValue(cell);
	}
	size = cell.row;

	vector <CString> result = { _T(""), _T("") };
	result[0] = work.cellValue(cell.row, header.adress[header.iName]);
	result[1] = work.cellValue(cell.row, header.adress[header.iSignal]);
	do {
		cell.row++;	step++;
		field = work.cellValue(cell);
	} while (field.IsEmpty() && cell.row < work.countRows());
	if (cell.row < work.countRows())
		step++;
	return result;
}

CString CReaderExcel::getComment(CWorkExcel& work, Cell cell, int size) {
	CString result = work.cellValue(cell);
	for (int i = 1; i < size; i++) {
		CString tmp = work.cellValue(cell.row + i, cell.column);
		result += _T("\n") + tmp;
	}
		
	return result;
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