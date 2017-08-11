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
	header.list.push_back(vector<CString>{ L"Адрес"});																// Адрес (arinc)

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

	if (!work.openBook(pathToExcel)) {
		ReadBookException exc;
		exc.setParam(work.bookName(pathToExcel));
		throw exc;
	}

	book.name = work.bookName();
	getSheets(book.sheets, work);
	
	return book;
}

// Чтение листов
void CReaderExcel::getSheets(vector <sheetData>& sheets, CWorkExcel& work) {
	sheets.resize(work.countSheets());
	for (long i = 1; i < work.countSheets() + 1; i++) {
		work.openSheet((long)i);
		sheets[i - 1].name = work.sheetName();

		// Поиск заголовков на листе
		if (!work.findHeader(header)) {
			NotAllHeaderException exc;
			exc.setParam(sheets[i - 1].name, work.bookName());
			throw exc;
		}

		sheets[i - 1].arinc = work.isArinc();
		sheets[i - 1].pk = work.pkValue(header);	// Установка подкадра
		sheets[i - 1].arinc ? sheets[i - 1].np = -1 : sheets[i - 1].np = work.npValue(header);	// Установка номера набора

		header.adress[header.iRow]++;

		getSignals(sheets[i - 1].signals, work, sheets[i - 1].arinc);
	}
}

// Чтение параметров на листе
void CReaderExcel::getSignals(vector <signalData>& signals, CWorkExcel& work, const bool& isArinc) {
	arincData arinc;
	for (long merge = 1, i = header.adress[header.iRow]; i < work.countRows() + 1; i += merge, merge = 1) {
		signalData signal;
		long column, row = i;

		// Надстройка для arinc
		if (isArinc) {
			if (isTitle(work, row))	// Пропуск строки заголовка
				continue;
			//	Поиск повторений
			CString remark = work.cellValue(row - 1, 1);
			if (remark.Find(ARINC_REMARK) != -1 && arinc.flag && arinc.startRow != row)	// Появилось повторение
				getArinc(remark, row, arinc);
			else if ((remark.Find(ARINC_REMARK) != -1 || isTitle(work, row - 1)) && !arinc.flag) {
				if (arinc.current == arinc.amount)
					getArinc(remark, row, arinc);
				else {
					i = row = arinc.startRow;
					arinc.current++;
				}
			}
		}

		// Чтение параметров одного сигнала
		// Адрес или номер слова, в зависимости от линии передачи
		if (!isArinc) {
			column = header.adress[header.iNumWord];
			merge = work.cPrevEmpty(row, column);
			signal.numWord = getNumWord(work.cellValue(row, column));
		}
		else {
			column = header.adress[header.iAdress];
			merge = work.cPrevEmpty(row, column);
			signal.numWord = getAdress(work.cellValue(row, column), arinc.current);
		}
		
		column = header.adress[header.iSignal];	row = i;			
		merge = work.cNextEmpty(row, column) + work.cPrevEmpty(row, column);
		signal.title[1] = work.cellValue(row, column);
		
		column = header.adress[header.iName];
		signal.title[0] = work.cellValue(row, column);

		// Замена буквы, если имеется повторение
		if (!arinc.flag) {
			CString tmp;
			// Сделать соответствие латинских и кириллицу (хитрецы пишут разными буковками)
			tmp.Format(L"%d", arinc.current);
			int pos = signal.title[1].Find(arinc.symbol);
			signal.title[1].Replace(arinc.symbol, tmp);
		}

		column = header.adress[header.iDimension];
		signal.dimension = work.cellValue(row, column);
		getMinMaxCsr(signal, work, row);	
		column = header.adress[header.iBits];
		signal.bit = getBits(work.cellValue(row, column), (int)signal.numWord.value.size());
		signal.comment = getComment(work, row, merge, signal.bitSign);

		bool bEmpty = isEmpty(work, row);		// Проверка на пустую строку
		bool bRemark = isRemark(work, row);		// Проверка на примечание

		if (work.countRows() == row + merge - 1 && !arinc.flag && arinc.amount != arinc.current) // Фича для обхода повторений в случае конца файла
			i = arinc.startRow - merge;

		// Добавление сигнала
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
	}
}

// Чтение циклов повторений в arinc протоколе(порядковый номер в кадре)
void CReaderExcel::getArinc(const CString& field, const long& row, arincData& arinc) {
	CString numeric = field;
	int posEqual = numeric.Find(L'=');

	if (posEqual != -1) {
		arinc.symbol = field[posEqual - 1];
		arinc.startRow = row;
		arinc.flag = false;

		// Получение второго числа диапазона
		numeric.Delete(0, posEqual + 1);
		int posDel = numeric.ReverseFind(L'…') == -1 ? numeric.ReverseFind(L'.') : numeric.ReverseFind(L'…');
		CString numeric1 = numeric.Mid(posDel + 1, numeric.GetLength() - posDel);
		
		// Получение первого числа диапазона
		posDel = numeric.Find(L'…') == -1 ? numeric.Find(L'.') : numeric.Find(L'…');
		numeric.Delete(posDel, numeric.GetLength() - posDel);
		numeric.Trim();	numeric1.Trim();
		
		arinc.current = getInt(numeric, arinc.flag);
		arinc.amount = getInt(numeric1, arinc.flag);
	}
	else {
		// Сброс параметра повторения
		arinc.flag = true;
		arinc.symbol = L"";
		arinc.amount = 0;
		arinc.current = 0;
	}
}

#pragma region Parameters
// Получить адрес из строки в число
intData CReaderExcel::getAdress(const CString& field, const int& num) {
	intData result;
	CString numeric = field;
	int adr = -1;
	int posDel, current = num;
	bool flag = true;

	do {
		posDel = numeric.Find(L',') == -1 ? numeric.GetLength() : numeric.Find(L',');

		CString str = numeric.Mid(0, posDel);
		numeric.Delete(0, posDel + 1);
		str.Replace(L'\n', L' ');
		vector<int> temp = stepGetAdress(str, result.flag);	// четные - адрес, нечетные - система счисления
		
		if (temp.size() == 4) {
			if (current > temp[2] - temp[0] + 1)
				current -= temp[2] - temp[0] + 1;
			else {
				adr = temp[0] + current - 1;
				result.sys = temp[1];
				flag = false;
			}
		}
		else if (temp.size() == 2 && current == 0) {
			adr = temp[0];
			result.sys = temp[1];
			flag = false;
		}
		else
			result.flag = true;
	} while (numeric.GetLength() != 0 && flag);

	result.value.push_back(adr);
	result.field = field;
	return result;
}

// Доп функция для адреса
vector <int> CReaderExcel::stepGetAdress(const CString& adress, bool& flag) {
	vector <int> result = {-1, -1};
	CString num = adress;
	int indxDel = num.Find(L'.') == -1 ? num.Find(L'…') : num.Find(L'.');	// Поиск индекса разделителей

	if (indxDel == -1) { // Разделителей нет, используется один разряд
		result[1] = getSubIndex(num);
		result[0] = getInt(num, flag);
	}
	else {
		result.push_back(-1);
		result.push_back(-1);
		int posDot = num.ReverseFind(L'.');
		if (posDot != -1)
			num.Delete(indxDel, posDot - indxDel);

		CString num2 = num;
		num.Delete(indxDel, num.GetLength());	// Первое число
		num2.Delete(0, indxDel + 1);			// Второе число
		num.Trim();	num2.Trim();
		
		result[1] = getSubIndex(num);
		result[3] = getSubIndex(num2);
		result[0] = getInt(num, flag);
		result[2] = getInt(num2, flag);
	}
	return result;
}

// Получение подстрочного индекса и удаление его из строки
int CReaderExcel::getSubIndex(CString& numeric) {
	// Подстрочные сиволы с кодом(8320 - 8329)
	CString sub;
	// Поиск индекса подстрочного символа
	for (int i = 0; i < numeric.GetLength(); i++) {
		if ((int)numeric[i] > 8319) {
			sub = numeric.Mid(i, numeric.GetLength() - i);
			numeric.Delete(i, numeric.GetLength() - i);
			break;
		}
	}

	int l = sub.GetLength();
	for (int i = 0; i < l; i++)
		sub += (wchar_t)((int)sub[i] - 8272); // Смещение подстрочного в обычное число
	sub.Delete(0, l);

	// Перевод в число int
	bool flag = false;
	int result = getInt(sub, flag);
	flag ? result = -1 : result = result;
	return result;
}

// Получить номера слов из ячейки в числа
intData CReaderExcel::getNumWord(const CString& field) {
	intData result;
	vector <int> vec = { -1 };
	CString numeric = field;
	int posDot = numeric.Find(L',');
	numeric.Trim();

	if (posDot == -1)	// Один номер
		vec[0] = getInt(numeric, result.flag);
	else {
		vec.push_back(-1);
		CString numeric2 = numeric;	
		numeric.Delete(posDot, numeric.GetLength());	// Первое число
		numeric2.Delete(0, posDot + 1);					// Второе число
		numeric.Trim();	numeric2.Trim();

		vec[0] = getInt(numeric, result.flag);
		vec[1] = getInt(numeric2, result.flag);
	}
	result.value = vec;
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
intData CReaderExcel::getBits(const CString& field, const int& size) {
	intData result;
	CString bits = field;
	int posDot = bits.Find(L',');
	vector <int> vec;

	// Для одного промежутка
	if (posDot == -1)
		vec = stepGetBits(bits, result.flag);
	else { // Для двух промежутков
		CString bits2 = bits;
		bits2.Delete(0, posDot + 1);
		bits.Delete(posDot, bits.GetLength());
		bits.Trim();	bits2.Trim();

		vec = stepGetBits(bits, result.flag);		// Первый промежуток
		vector <int> vec2 = stepGetBits(bits2, result.flag);	// Второй промежуток

		if (size == 2) {
			vec.resize(4);
			for (size_t i = 0; i < vec2.size(); i++)
				vec[i + 2] = vec2[i];
		}
		else
			vec[1] = vec2[0];
	}
	result.field = field;
	result.value = vec;
	return result;
}

// Дополнительная функция для перевода разрядов
vector <int> CReaderExcel::stepGetBits(const CString& bits, bool& flag) {
	vector <int> result = { -1, -1 };
	CString num = bits;
	num.Trim();

	int indxDel = num.Find(L'.') == -1 ? num.Find(L'…') : num.Find(L'.');	// Поиск индекса разделителей

	if (indxDel == -1)	// Разделителей нет, используется один разряд
		result[0] = getInt(num, flag);
	else {
		int posDot = num.ReverseFind(L'.');
		if (posDot != -1)
			num.Delete(indxDel, posDot - indxDel);

		CString num2 = num;
		num.Delete(indxDel, num.GetLength());	// Первое число
		num2.Delete(0, indxDel + 1);	// Второе число
		num2.Trim();	num.Trim();

		result[0] = getInt(num, flag);
		result[1] = getInt(num2, flag);
	}

	return result;
}

// Чтение примечания
CString CReaderExcel::getComment(CWorkExcel& work, const long& row, const int& size, bool& flag) {
	long column = header.adress[header.iComment];
	long tmpRow = row;
	int merge = work.getMerge(tmpRow, column);
	CString result;

	size > merge ? merge = size : merge = merge;
	for (int i = 0; i < merge; i++) {
		CString tmp = work.cellValue(tmpRow + i, column);
		if (!tmp.IsEmpty()) {
			(tmp.Find(SIGN_FIELD) != -1) ? flag = true : flag = flag;
			result.IsEmpty() ? result = tmp : result += L'\n' + tmp;
		}
	}
	return result;
}
#pragma endregion

#pragma region Convert String
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
		(*end != '\0' || errno != 0) ? flag = true : flag = flag;
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
		(*end != '\0' || errno != 0) ? flag = true : flag = flag;
	}
	else
		flag = true;

	return result;
}
#pragma endregion

#pragma region subFunc
// Является ли строка заголовком
bool CReaderExcel::isTitle(CWorkExcel& work, const long& row) {
	bool result = false;
	CString numeric = work.cellValue(row, 1);
	if (!numeric.IsEmpty())
		getInt(numeric, result);
	return result;
}

// Является ли строка пустой
bool CReaderExcel::isEmpty(CWorkExcel& work, const long& row) {
	bool result = true;
	for (long i = 1; i < header.size; i++) {
		long column = header.adress[i];
		if (header.adress[i] != -1) {
			CString tmp = work.cellValue(row, column);
			if (!tmp.IsEmpty())
				result = false;
		}
	}
	return result;
}

// Является ли строка примечанием
bool CReaderExcel::isRemark(CWorkExcel& work, const long& row) {
	bool result = false;
	for (long i = 1; i < header.size; i++) {
		long column = header.adress[i];
		if (header.adress[i] != -1) {
			CString tmp = work.cellValue(row, column);
			result = (tmp.Find(REMARK1) > -1 ||
				tmp.Find(REMARK2) > -1) ? true : result;
		}
	}
	return result;
}

// Проверка расширения
bool CReaderExcel::checkExtension(const CString& path) {
	bool result = false;
	int dotPos = path.ReverseFind(L'.');	// Поиск точечки
	CString ext = path.Mid(dotPos + 1);		// Выделение расширения из пути

	for (size_t i = 0; i < extension.size(); i++)
		if (ext.CompareNoCase(extension[i]) == 0)
			result = true;

	return result;
}
#pragma endregion