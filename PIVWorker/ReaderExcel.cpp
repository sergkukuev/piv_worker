#include "stdafx.h"
#include "ReaderExcel.h"

// Конструктор
CReaderExcel::CReaderExcel() {
	// Установка набора заголовков
	header.list.push_back(vector<CString>{ L"№ слов", L"№ слова", L"№ п/п", L"№ слова в подадресе" });				// Для номера слова
	header.list.push_back(vector<CString>{ L"Наименование параметра", L"Наименование сигнала" });					// Для наименование параметра
	header.list.push_back(vector<CString>{ L"Обозначение сигнала", L"Условное обозначение параметра / сигнала" });	// Для сигнала
	header.list.push_back(vector<CString>{ L"Размерность", L"Единица измерения" });									// Для единицы изменерения
	header.list.push_back(vector<CString>{ L"Минимальное значение", L"Миним. значение", L"Минимал значение" });		// Минимальное значение
	header.list.push_back(vector<CString>{ L"Максимальное значение", L"Максим. значение", L"Максимал значение" });	// Максимальное значение
	header.list.push_back(vector<CString>{ L"Цена старшего разряда" });												// Для цср
	header.list.push_back(vector<CString>{ L"Используемые разряды", L"Используе-мые разряды" });					// Для используемых разрядов
	header.list.push_back(vector<CString>{ L"Примечание" });														// Комментарии (пояснения)

	// Добавление всех видов расширений excel файлов
	extension.push_back(L"xlsx");	// Текущий основной формат
	extension.push_back(L"xls");	// Основной формат до 2003 года
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
		merge = work.cPrevEmpty(row, column);
		signal.numWord = getNumWord(work.cellValue(row, column));
		
		column = header.adress[header.iName];	row = i;
		merge = work.cNextEmpty(row, column) + work.cPrevEmpty(row, column);
		signal.title[0] = work.cellValue(row, column);

		column = header.adress[header.iSignal];
		signal.title[1] = work.cellValue(row, column);

		column = header.adress[header.iDimension];
		signal.dimension = work.cellValue(row, column);

		getMinMaxCsr(signal, work, row);	
		
		column = header.adress[header.iBits];
		signal.bit = getBits(work.cellValue(row, column));

		signal.comment = getComment(work, row, merge, signal.bitSign);

		bool bEmpty = isEmpty(work, row);		// Проверка на пустую строку
		bool bRemark = isRemark(work, row);		// Проверка на примечание

		// Добавление сигнала
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
	}
}

// Получить номера слов из ячейки в числа
intData CReaderExcel::getNumWord(const CString& field) {
	intData result;
	CString numeric = field;
	int posDot = numeric.Find(_T(','));
	vector <int> vec;
	vec.push_back(-1);

	if (posDot == -1) {
		numeric.Trim();
		vec[0] = getInt(numeric, result.flag);
	}
	else {
		vec.push_back(-1);
		CString numeric2 = numeric;	
		numeric.Delete(posDot, numeric.GetLength());	// Первое число
		numeric.Trim();
		vec[0] = getInt(numeric, result.flag);

		numeric2.Delete(0, posDot + 1);					// Второе число
		numeric2.Trim();
		vec[1] = getInt(numeric2, result.flag);
	}

	result.vec = vec;
	result.field = field;
	return result;
}

// Получить значения мин, макс и цср
void CReaderExcel::getMinMaxCsr(signalData& signal, CWorkExcel& work, const long& row) {
	long column = header.adress[header.iMin];
	signal.min.field = work.cellValue(row, column);
	signal.min.value = getDouble(signal.min.field, signal.min.flag);

	column = header.adress[header.iMax];
	signal.max.field = work.cellValue(row, column);
	signal.max.value = getDouble(signal.max.field, signal.max.flag);

	column = header.adress[header.iCSR];
	signal.csr.field = work.cellValue(row, column);
	signal.csr.value = getDouble(signal.csr.field, signal.csr.flag);

	// Сброс флагов, если одновременно все параметры пусты
	if (signal.min.value == DBL_MIN && signal.max.value == DBL_MIN && signal.csr.value == DBL_MIN) {
		signal.min.flag = false;
		signal.max.flag = false;
		signal.csr.flag = false;
	}
}

// Перевод из используемых разрядов из строки в числа
intData CReaderExcel::getBits(const CString& field) {
	intData result;
	CString bits = field;
	int posDot = bits.Find(_T(','));
	vector <int> vec;

	// Для одного промежутка
	if (posDot == -1)
		vec = stepGetBits(bits, result.flag);
	else { // Для двух промежутков
		CString bits2 = bits;
		bits2.Delete(0, posDot + 1);
		bits.Delete(posDot, bits.GetLength());
		bits.Trim();	bits2.Trim();

		vec = stepGetBits(bits, result.flag);
		vec[1] = -1;
		vec.resize(4);

		vector <int> vec2 = stepGetBits(bits2, result.flag);
		for (size_t i = 0; i < vec2.size(); i++)
			vec[i + 2] = vec2[i];
	}

	result.field = field;
	result.vec = vec;
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
	int indxDel = num.Find(L'.');
	if (indxDel == -1)
		indxDel = num.Find(L'…');

	if (indxDel == -1)	// Разделителей нет, используется один разряд
		result[0] = getInt(num, flag);
	else {
		int posDot = num.ReverseFind(L'.');
		if (posDot != -1)
			num.Delete(indxDel, posDot - indxDel);

		CString num2 = num;
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
	CString temp = field;
	temp.Trim();
	temp.Replace(L',', L'.');
	CStringA tmp(temp);
	char* str = new char[tmp.GetLength() + 1];
	strcpy_s(str, tmp.GetLength() + 1, tmp);

	if (!temp.IsEmpty()) {
		char* end;
		errno = 0;
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
	CString temp = field;
	temp.Trim();
	CStringA tmp(temp);
	char* str = new char[tmp.GetLength() + 1];
	strcpy_s(str, tmp.GetLength() + 1, tmp);

	if (!field.IsEmpty()) {
		char* end;
		errno = 0;
		result = strtol(str, &end, 10);
		(*end != 0 || errno != 0) ? flag = true : flag = flag;
	}
	else
		flag = true;

	return result;
}

// Чтение примечания
CString CReaderExcel::getComment(CWorkExcel& work, const long& row, const int& size, bool& flag) {
	long column = header.adress[header.iComment];
	long tmpRow = row;
	int merge = work.getMerge(tmpRow, column);
	CString result;

	(size > merge) ? merge = size : merge = merge;
	for (int i = 0; i < merge; i++) {
		CString tmp = work.cellValue(tmpRow + i, column);
		tmp.Trim();	
		if (!tmp.IsEmpty()) {
			(tmp.Find(SIGN_FIELD) != -1) ? flag = true : flag = flag;
			result.IsEmpty() ? result = tmp : result += L'\n' + tmp;
		}
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
		result = (tmp.Find(REMARK1) > -1 ||
			tmp.Find(REMARK2) > -1) ? true : result;
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