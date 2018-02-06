#include "stdafx.h"
#include "ReaderExcel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Конструктор
CReaderExcel::CReaderExcel()
{
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
	header.list.push_back(vector<CString>{ L"Адрес"});																// Адрес (для ARINC)

																													// Поддерживаемые расширения Excel файлов
	extension.push_back(L"xlsx");	// Текущий основной формат
	extension.push_back(L"xls");	// Основной формат до 2003 года
}

// Деструктор
CReaderExcel::~CReaderExcel()
{
	header.list.clear();
	header.list.shrink_to_fit();
	extension.clear();
	extension.shrink_to_fit();
}

// Чтение протокола
bookData CReaderExcel::GetBook(const CString& pathToExcel, const int& iProject)
{
	bookData book;
	this->iProject = iProject;

	if (!ExtensionChecker(pathToExcel))
		throw BadTypeException(work.BookName(pathToExcel));

	if (!work.OpenBook(pathToExcel))
		throw ReadBookException(work.BookName(pathToExcel));

	book.name = work.BookName();
	GetSheets(book.sheets);

	return book;
}

// Чтение таблиц протоколов (листов)
void CReaderExcel::GetSheets(vector <sheetData>& sheets)
{
	sheets.resize(work.CountSheets());
	for (long i = 1; i < work.CountSheets() + 1; i++)
	{
		work.OpenSheet(i);
		sheets[i - 1].name = work.SheetName();

		if (!work.FindHeader(header)) // Поиск заголовков таблицы
			throw NotAllHeaderException(work.BookName(), sheets[i - 1].name);

		sheets[i - 1].arinc = work.IsArinc();
		sheets[i - 1].pk = work.PkValue(header);

		header.adress[header.iRow]++;

		GetSignals(sheets[i - 1].signals, sheets[i - 1].arinc);
		sheets[i - 1].arinc ? sheets[i - 1].np = -1 : sheets[i - 1].np = work.NpValue(sheets[i - 1].signals[0].comment);
	}
}

// Чтение параметров на листе
void CReaderExcel::GetSignals(vector <signalData>& signals, const bool& bArinc)
{
	arincData arinc;
	for (long merge = 1, row = header.adress[header.iRow], column; row <= work.CountRows(); row += merge, merge = 1)
	{
		signalData signal;
		// Надстройка для arinc
		if (bArinc)
			if (!IsTitle(row))
				ArincChecker(arinc, row);
			else
				continue;

		// Чтение параметров одного сигнала. Адрес (для ARINC) записывается вместо номера слова
		if (!bArinc)
		{
			column = header.adress[header.iNumWord];
			int range = work.CPrevEmpty(row, column);
			signal.numWord = GetNumWord(work.CellValue(row - range, column));
		}
		else
		{
			column = header.adress[header.iAdress];
			int range = work.CPrevEmpty(row, column);
			signal.numWord = GetAdress(work.CellValue(row - range, column), arinc.current);
		}

		column = header.adress[header.iName];
		int range = work.CPrevEmpty(row, column);
		signal.title[0] = work.CellValue(row - range, column);
		range = work.CNextEmpty(row, column);

		column = header.adress[header.iSignal];
		merge = work.CNextEmpty(row, column);
		signal.title[1] = work.CellValue(row, column);

		if (range < merge)
			merge = range;

		// Замена буквы, если имеется повторение
		if (!arinc.flag)
		{
			CString tmp;
			// TODO: Сделать соответствие латинских и кириллицу (потому что хитрецы пишут разными буковками)
			tmp.Format(L"%d", arinc.current);
			int pos = signal.title[1].Find(arinc.symbol);
			signal.title[1].Replace(arinc.symbol, tmp);
		}

		column = header.adress[header.iDimension];
		signal.dimension = work.CellValue(row, column);
		GetMinMaxCsr(signal, row);
		column = header.adress[header.iBits];
		signal.bit = GetBits(work.CellValue(row, column), (int)signal.numWord.value.size());
		signal.comment = GetComment(row, merge, signal.bitSign);

		bool bEmpty = IsEmpty(row);
		bool bRemark = IsRemark(row);

		if (!bEmpty && !bRemark)
			signals.push_back(signal);

		// TODO: Исправление костыля (обход повторений в случае конца таблицы)
		if (work.CountRows() == row + merge - 1 && !arinc.flag && arinc.amount != arinc.current)
			row = arinc.startRow - merge;
	}

	// Поиск двойных слов в КПРНО35
	if (iProject == project::kprno35)
		ConcatDW(signals);
}

// Объединение двойных слов (установка указателя)
void CReaderExcel::ConcatDW(vector <signalData>& signals)
{
	for (size_t i = 0; i < signals.size(); i++)
	{
		for (size_t j = 0; j < dwPart::low.size(); j++)
		{
			if (signals[i].title[0].Find(dwPart::hight[j]) != -1)
				findDW(signals, i, dwPart::hight[j], dwPart::low);
			else if (signals[i].title[0].Find(dwPart::hight2[j]) != -1)
				findDW(signals, i, dwPart::hight2[j], dwPart::low);
			else if (signals[i].title[0].Find(dwPart::low[j]) != -1)
				findDW(signals, i, dwPart::low[j], dwPart::hight);
		}
	}
}

// Поиск второй части двойного слова
void CReaderExcel::findDW(vector <signalData>& signals, size_t start, CString old, vector <CString> revert)
{
	CString title = signals[start].title[0];
	title.Remove(L'\n');

	int index = -1, rev_index = 0;
	for (size_t i = start; i < signals.size(); i++)
	{
		CString tmp = signals[i].title[0];
		tmp.Remove(L' ');
		tmp.Remove(L'\n');
		for (size_t j = 0; j < revert.size(); j++)
		{
			CString rev_title = title;
			rev_title.Replace(old, revert[j]);
			rev_title.Remove(L' ');
			if (tmp.Compare(rev_title) == 0)
			{
				rev_index = (int)j;
				index = (int)i;
				break;
			}
		}
		if (index != -1)
		{
			signals[start].part = &signals[index];
			signals[index].part = &signals[start];
			break;
		}
	}
}

#pragma region ARINC
//  Поиск повторяющихся блоков
void CReaderExcel::ArincChecker(arincData& arinc, long& row)
{
	//	Поиск повторений
	CString field = work.CellValue(row - 1, 1);
	if (field.Find(ARINC_REMARK) != -1 && arinc.flag && arinc.startRow != row)	// Опачки, появилось
		GetArinc(arinc, row, field);
	else if ((field.Find(ARINC_REMARK) != -1 || IsTitle(row - 1)) && !arinc.flag)
	{
		if (arinc.current == arinc.amount)
			GetArinc(arinc, row, field);
		else
		{
			row = arinc.startRow;
			arinc.current++;
		}
	}
}

// Чтение циклов повторений в ARINC протоколе (порядковый номер в кадре)
void CReaderExcel::GetArinc(arincData& arinc, const long& row, CString numeric)
{
	int posEqual = numeric.Find(L'=');
	if (posEqual != -1)
	{
		arinc.symbol = numeric[posEqual - 1];
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

		arinc.current = GetInt(numeric, arinc.flag);
		arinc.amount = GetInt(numeric1, arinc.flag);
	}
	else
	{
		// Сброс параметра повторения
		arinc.flag = true;
		arinc.symbol = L"";
		arinc.amount = 0;
		arinc.current = 0;
	}
}
#pragma endregion

#pragma region Read_params
// Адреса (ARINC)
intData CReaderExcel::GetAdress(CString numeric, int current)
{
	intData result;
	int adr = -1;
	int posDel;
	bool flag = true;
	result.field = numeric;

	do
	{
		posDel = numeric.Find(L',') == -1 ? numeric.GetLength() : numeric.Find(L',');
		CString str = numeric.Mid(0, posDel);
		numeric.Delete(0, posDel + 1);
		str.Replace(L'\n', L' ');
		vector<int> temp = StepAdress(str, result.flag);	// четные - адрес, нечетные - система счисления

		if (temp.size() == 4)
		{
			if (current > temp[2] - temp[0] + 1)
				current -= temp[2] - temp[0] + 1;
			else
			{
				adr = temp[0] + current - 1;
				result.sys = temp[1];
				flag = false;
			}
		}
		else if (temp.size() == 2 && current == 0)
		{
			adr = temp[0];
			result.sys = temp[1];
			flag = false;
		}
		else
			result.flag = true;
	} while (numeric.GetLength() != 0 && flag);

	result.value.push_back(adr);
	return result;
}

// Дополнительная функция для перевода адреса
vector <int> CReaderExcel::StepAdress(CString numeric, bool& flag)
{
	vector <int> result = { -1, -1 };
	int indxDel = numeric.Find(L'.') == -1 ? numeric.Find(L'…') : numeric.Find(L'.');	// Поиск индекса разделителей

	// Разделителей нет, используется один разряд
	if (indxDel == -1)
	{
		result[1] = GetSubIndex(numeric);
		result[0] = GetInt(numeric, flag);
	}
	else
	{
		result.push_back(-1);
		result.push_back(-1);
		int posDot = numeric.ReverseFind(L'.');
		if (posDot != -1)
			numeric.Delete(indxDel, posDot - indxDel);

		CString numeric2 = numeric;
		numeric.Delete(indxDel, numeric.GetLength());	// Первое число
		numeric2.Delete(0, indxDel + 1);			// Второе число
		numeric.Trim();	numeric2.Trim();

		result[1] = GetSubIndex(numeric);
		result[3] = GetSubIndex(numeric2);
		result[0] = GetInt(numeric, flag);
		result[2] = GetInt(numeric2, flag);
	}
	return result;
}

// Получение подстрочного индекса и удаление его из строки
// Подстрочные сиволы с кодом(8320 - 8329)
// TODO: Исправление костыля (в случае смены системы счисления, отличной от 8-ой, может перестать работать) 
int CReaderExcel::GetSubIndex(CString& numeric)
{
	CString sub;
	// Поиск индекса подстрочного символа
	for (int i = 0; i < numeric.GetLength(); i++)
	{
		if ((int)numeric[i] > 8319)
		{
			sub = numeric.Mid(i, numeric.GetLength() - i);
			numeric.Delete(i, numeric.GetLength() - i);
			break;
		}
	}

	int l = sub.GetLength();
	for (int i = 0; i < l; i++)
		sub += (wchar_t)((int)sub[i] - 8272); // Смещение подстрочного в обычное число
	sub.Delete(0, l);

	bool flag = false;
	int result = GetInt(sub, flag);
	flag ? result = -1 : result = result;
	return result;
}

// Чтение номера слова
intData CReaderExcel::GetNumWord(CString numeric)
{
	intData result;
	vector <int> vec = { -1 };
	int posDot = numeric.Find(L',');
	result.field = numeric;

	if (posDot == -1)	// Один номер
		vec[0] = GetInt(numeric, result.flag);
	else
	{
		vec.push_back(-1);
		CString numeric2 = numeric;
		numeric.Delete(posDot, numeric.GetLength());	// Первое число
		numeric2.Delete(0, posDot + 1);					// Второе число
		numeric.Trim();	numeric2.Trim();

		vec[0] = GetInt(numeric, result.flag);
		vec[1] = GetInt(numeric2, result.flag);
	}
	result.value = vec;
	return result;
}

// Чтение минимального, максимального и цср
void CReaderExcel::GetMinMaxCsr(signalData& signal, const long& row)
{
	long column = header.adress[header.iMin];
	signal.min.field = work.CellValue(row, column);
	signal.min.value = GetDouble(signal.min.field, signal.min.flag);

	column = header.adress[header.iMax];
	signal.max.field = work.CellValue(row, column);
	signal.max.value = GetDouble(signal.max.field, signal.max.flag);

	column = header.adress[header.iCSR];
	signal.csr.field = work.CellValue(row, column);
	signal.csr.value = GetDouble(signal.csr.field, signal.csr.flag);

	// Сброс флагов, если одновременно все параметры пусты
	if (signal.min.value == DBL_MIN && signal.max.value == DBL_MIN && signal.csr.value == DBL_MIN)
	{
		signal.min.flag = false;
		signal.max.flag = false;
		signal.csr.flag = false;
	}
}

// Чтение используемых разрядов
intData CReaderExcel::GetBits(CString bits, const int& size)
{
	intData result;
	int posDot = bits.Find(L',');
	vector <int> vec;
	result.field = bits;

	// Для одного промежутка
	if (posDot == -1)
		vec = StepBits(bits, result.flag);
	else // Для двух промежутков
	{
		CString bits2 = bits;
		bits2.Delete(0, posDot + 1);
		bits.Delete(posDot, bits.GetLength());
		bits.Trim();	bits2.Trim();

		vec = StepBits(bits, result.flag);		// Первый промежуток
		vector <int> vec2 = StepBits(bits2, result.flag);	// Второй промежуток

		if (size == 2)
		{
			vec.resize(4);
			for (size_t i = 0; i < vec2.size(); i++)
				vec[i + 2] = vec2[i];
		}
		else
			vec[1] = vec2[0];
	}
	result.value = vec;
	return result;
}

// Дополнительная функция для перевода разрядов
vector <int> CReaderExcel::StepBits(CString numeric, bool& flag)
{
	vector <int> result = { -1, -1 };

	int indxDel = numeric.Find(L'.') == -1 ? numeric.Find(L'…') : numeric.Find(L'.');	// Поиск индекса разделителей

	if (indxDel == -1)	// Разделителей нет, используется один разряд
		result[0] = GetInt(numeric, flag);
	else
	{
		int posDot = numeric.ReverseFind(L'.');
		if (posDot != -1)
			numeric.Delete(indxDel, posDot - indxDel);

		CString numeric2 = numeric;
		numeric.Delete(indxDel, numeric.GetLength());	// Первое число
		numeric2.Delete(0, indxDel + 1);	// Второе число
		numeric2.Trim();	numeric.Trim();

		result[0] = GetInt(numeric, flag);
		result[1] = GetInt(numeric2, flag);
	}
	return result;
}

// Чтение примечания
CString CReaderExcel::GetComment(long row, const int& size, bool& flag)
{
	long column = header.adress[header.iComment];
	int merge = work.GetMerge(row, column);
	CString result;

	size > merge ? merge = size : merge = merge;
	for (int i = 0; i < merge; i++)
	{
		CString tmp = work.CellValue(row + i, column);
		if (!tmp.IsEmpty())
		{
			for (size_t j = 0; j < SIGN_FIELD.size(); j++)
				tmp.Find(SIGN_FIELD[j]) != -1 ? flag = true : flag = flag;
			result.IsEmpty() ? result = tmp : result += L'\n' + tmp;
		}
	}
	return result;
}
#pragma endregion

#pragma region Converters
// Конвертер int значения
int CReaderExcel::GetInt(CString field, bool& flag)
{
	int result = -1;
	CStringA tmp(field);
	char* buffer = new char[tmp.GetLength() + 1];
	strcpy_s(buffer, tmp.GetLength() + 1, tmp);

	if (!field.IsEmpty())
	{
		char* end;
		errno = 0;
		result = strtol(buffer, &end, 10);
		(*end != '\0' || errno != 0) ? flag = true : flag = flag;
	}
	else
		flag = true;

	delete[] buffer;
	return result;
}

// Конвертер double значения
double CReaderExcel::GetDouble(CString field, bool& flag)
{
	double result = DBL_MIN;
	field.Replace(L',', L'.');
	CStringA tmp(field);
	char* buffer = new char[tmp.GetLength() + 1];
	strcpy_s(buffer, tmp.GetLength() + 1, tmp);

	if (!field.IsEmpty())
	{
		char* end;
		errno = 0;
		result = strtod(buffer, &end);
		(*end != '\0' || errno != 0) ? flag = true : flag = flag;
	}
	else
		flag = true;

	delete[] buffer;
	return result;
}
#pragma endregion

#pragma region Checkers
// Проверка строки на заголовок
bool CReaderExcel::IsTitle(const long& row)
{
	bool result = false;
	CString numeric = work.CellValue(row, 1);
	if (!numeric.IsEmpty())
		GetInt(numeric, result);
	return result;
}

// Проверка строки на пустоту
bool CReaderExcel::IsEmpty(const long& row)
{
	bool result = true;
	for (long i = 1; i < header.size; i++)
	{
		long column = header.adress[i];
		if (header.adress[i] != -1)
		{
			CString tmp = work.CellValue(row, column);
			if (!tmp.IsEmpty())
				result = false;
		}
	}
	return result;
}

// Проверка строки на примечание
bool CReaderExcel::IsRemark(const long& row)
{
	bool result = false;
	for (long i = 1; i < header.size; i++)
	{
		long column = header.adress[i];
		if (header.adress[i] != -1)
		{
			CString tmp = work.CellValue(row, column);
			for (size_t j = 0; j < REMARK.size(); j++)
				result = tmp.Find(REMARK[j]) > -1 ? true : result;
		}
	}
	return result;
}

// Проверка расширения файла
bool CReaderExcel::ExtensionChecker(const CString& path)
{
	bool result = false;
	int dotPos = path.ReverseFind(L'.');	// Поиск точечки
	CString ext = path.Mid(dotPos + 1);		// Выделение расширения из пути

	for (size_t i = 0; i < extension.size(); i++)
		if (ext.CompareNoCase(extension[i]) == 0)
			result = true;

	return result;
}

#pragma endregion