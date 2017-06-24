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
bookData CReaderExcel::getBook(const CString& pathToExcel) {
	bookData book;
	CWorkExcel work;

	if (!checkExtension(pathToExcel)) {
		BadTypeException exc;
		exc.setName(work.bookName(pathToExcel));
		throw exc;
	}

	if (!work.openBook(pathToExcel))
		throw ReadBookException();

	book.name = work.bookName();
	getSheets(book.sheets, work);
	
	return book;
}

// Чтение листов
void CReaderExcel::getSheets(vector <sheetData>& sheets, CWorkExcel& work) {
	sheets.resize(work.countSheets());
	for (long i = 1; i < work.countSheets() + 1; i++) {
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

// Чтение параметров на листе
void CReaderExcel::getSignals(vector <signalData>& signals, CWorkExcel& work) {
	for (long merge, i = header.adress[header.iRow]; i < work.countRows() + 1; i += merge) {
		signalData signal;
		long column, row = i;

		// Чтение параметров одного сигнала
		column = header.adress[header.iNumWord];
		merge = work.getMerge(row, column);
		signal.numWord = getNumWord(work.cellValue(row, column), signal.flags.num);
		
		column = header.adress[header.iName];	row = i;
		merge = work.getMerge(row, column);
		signal.title[0] = work.cellValue(row, column);

		column = header.adress[header.iSignal];
		signal.title[1] = work.cellValue(row, column);

		column = header.adress[header.iDimension];
		signal.dimension = work.cellValue(row, column);

		getMinMaxCsr(signal, work, row);	
		
		column = header.adress[header.iBits];
		signal.bit = getBits(work.cellValue(row, column), signal.flags.bit);

		signal.comment = getComment(work, row, merge, signal.bitSign);

		bool bEmpty = isEmpty(work, row);	// Проверка на пустую строку
		bool bRemark = isRemark(work, row);	// Проверка на примечание

		// Добавление сигнала
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
	}
}

// Получить номера слов из ячейки в числа
vector <int> CReaderExcel::getNumWord(const CString& field, bool& flag) {
	CString numeric = field;
	int posDot = numeric.Find(_T(','));
	vector <int> result;
	result.push_back(-1);

	if (posDot == -1) {
		numeric.Trim();
		result[0] = getInt(numeric, flag);
	}
	else {
		result.push_back(-1);
		CString numeric2 = numeric;	
		numeric.Delete(posDot, numeric.GetLength());	// Первое число
		numeric.Trim();
		result[0] = getInt(numeric, flag);

		numeric2.Delete(0, posDot + 1);					// Второе число
		numeric2.Trim();
		result[1] = getInt(numeric2, flag);
	}
	return result;
}

// Получить значения мин, макс и цср
void CReaderExcel::getMinMaxCsr(signalData& signal, CWorkExcel& work, const long& row) {
	long column = header.adress[header.iMin];
	signal.min = getDouble(work.cellValue(row, column), signal.flags.min);

	column = header.adress[header.iMax];
	signal.max = getDouble(work.cellValue(row, column), signal.flags.max);

	column = header.adress[header.iCSR];
	signal.csr = getDouble(work.cellValue(row, column), signal.flags.csr);

	// Сброс флагов, если одновременно все параметры пусты
	if (signal.min == signal.max == signal.csr == DBL_MIN) {
		signal.flags.min = false;
		signal.flags.max = false;
		signal.flags.csr = false;
	}
}

// Перевод из используемых разрядов из строки в числа
vector <int> CReaderExcel::getBits(const CString& field, bool& flag) {
	CString bits = field;
	int posDot = bits.Find(_T(','));
	vector <int> result;

	// Для одного промежутка
	if (posDot == -1)
		result = stepGetBits(bits, flag);
	else { // Для двух промежутков
		CString bits2 = bits;
		bits2.Delete(0, posDot + 1);
		bits.Delete(posDot, bits.GetLength());
		bits.Trim();	bits2.Trim();

		result = stepGetBits(bits, flag);
		result[1] = -1;
		result.resize(4);

		vector <int> result2 = stepGetBits(bits2, flag);
		for (size_t i = 0; i < result2.size(); i++)
			result[i + 2] = result2[i];
	}
	return result;
}

// Дополнительная функция для перевода разрядов
vector <int> CReaderExcel::stepGetBits(const CString& bits, bool& flag) {
	vector <int> result;
	CString num = bits;
	num.Trim();

	result.push_back(-1);
	result.push_back(-1);

	// Поиск индекса разделителей
	int indxDel = num.Find(_T('.'));
	if (indxDel == -1)
		indxDel = num.Find(_T('…'));

	if (indxDel == -1)	// Разделителей нет, используется один разряд
		result[0] = getInt(num, flag);
	else {
		CString num2 = num;	
		int posDot = num.ReverseFind(_T('.'));

		if (posDot != -1)
			indxDel = posDot;

		num.Delete(indxDel, num.GetLength());	// Первое число
		num.Trim();
		result[0] = getInt(num, flag);

		num2.Delete(0, indxDel + 1);	// Второе число
		num2.Trim();
		result[1] = getInt(num2, flag);
	}

	return result;
}

// Конвертер double значения
double CReaderExcel::getDouble(const CString& field, bool& flag) {
	double result = DBL_MIN;
	char* end;
	errno = 0;

	CStringA tmp(field);
	char* str = new char[tmp.GetLength() + 1];
	strcpy_s(str, tmp.GetLength() + 1, tmp);

	if (!field.IsEmpty()) {
		result = strtod(str, &end);
		(*end != 0 || errno != 0) ? flag = true : flag = flag;
	}
	else
		flag = true;

	delete[] str;
	return result;
}

// Конвертер int значения
int CReaderExcel::getInt(const CString& field, bool& flag) {
	int result = -1;

	if (!field.IsEmpty()) {
		result = _wtoi(field);
		(result == 0) ? flag = true : flag = flag;
	}
	else
		flag = true;

	return result;
}

// Чтение примечания
CString CReaderExcel::getComment(CWorkExcel& work, const long& row, const int& size, bool& flag) {
	long column = header.adress[header.iComment];
	CString result;

	for (int i = 0; i < size; i++) {
		CString tmp = work.cellValue(row + i, column);
		tmp.Trim();
		if (!tmp.IsEmpty())
			(tmp.Find(SIGN_FIELD) != -1) ? flag = true : flag = flag;
			result += _T("\n") + tmp;
	}
		
	return result;
}

// Проверка строки на пустоту
bool CReaderExcel::isEmpty(CWorkExcel& work, const long& row) {
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
bool CReaderExcel::isRemark(CWorkExcel& work, const long& row) {
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
bool CReaderExcel::checkExtension(const CString& path) {
	bool result = false;
	int dotPos = path.ReverseFind(_T('.'));	// Поиск точечки
	CString ext = path.Mid(dotPos + 1);		// Выделение расширения из пути

	for (size_t i = 0; i < extension.size(); i++)
		if (ext.CompareNoCase(extension[i]) == 0)
			result = true;

	return result;
}