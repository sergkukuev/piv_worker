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
bookData CReaderExcel::GetBook(const CString& pathToExcel, bool bProject) 
{
	bookData book;
	CWorkExcel work;

	bProj = bProject;

	if (!CheckExtension(pathToExcel))
		throw BadTypeException(work.BookName(pathToExcel));

	if (!work.OpenBook(pathToExcel))
		throw ReadBookException(work.BookName(pathToExcel));

	book.name = work.BookName();
	GetSheets(book.sheets, work);
	
	return book;
}

// Чтение таблиц протоколов
void CReaderExcel::GetSheets(vector <sheetData>& sheets, CWorkExcel& work) 
{
	sheets.resize(work.CountSheets());
	for (long i = 1; i < work.CountSheets() + 1; i++) 
	{
		work.OpenSheet((long)i);
		sheets[i - 1].name = work.SheetName();

		if (!work.FindHeader(header)) // Поиск заголовков таблицы
			throw NotAllHeaderException(work.BookName(), sheets[i - 1].name);

		sheets[i - 1].arinc = work.IsArinc();
		sheets[i - 1].pk = work.PkValue(header);

		header.adress[header.iRow]++;

		GetSignals(sheets[i - 1].signals, work, sheets[i - 1].arinc);
		sheets[i - 1].arinc ? sheets[i - 1].np = -1 : sheets[i - 1].np = work.NpValue(sheets[i - 1].signals[0].comment/*header*/);
	}
}

// Чтение параметров на листе
void CReaderExcel::GetSignals(vector <signalData>& signals, CWorkExcel& work, const bool& isArinc) 
{
	arincData arinc;
	for (long merge = 1, row = header.adress[header.iRow], column; row < work.CountRows() + 1; row += merge, merge = 1) 
	{
		signalData signal;
		// Надстройка для arinc
		if (isArinc) 
		{
			if (IsTitle(work, row))
				continue;
			//	Поиск повторений
			CString remark = work.CellValue(row - 1, 1);
			if (remark.Find(ARINC_REMARK) != -1 && arinc.flag && arinc.startRow != row)	// Опачки, появилось
				GetArinc(remark, row, arinc);
			else if ((remark.Find(ARINC_REMARK) != -1 || IsTitle(work, row - 1)) && !arinc.flag) 
			{
				if (arinc.current == arinc.amount)
					GetArinc(remark, row, arinc);
				else
				{
					row = arinc.startRow;
					arinc.current++;
				}
			}
		}

		// Чтение параметров одного сигнала
		// Адрес (ARINC) записывается вместо номера слова
		if (!isArinc) 
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
			// Сделать соответствие латинских и кириллицу (хитрецы пишут разными буковками)
			tmp.Format(L"%d", arinc.current);
			int pos = signal.title[1].Find(arinc.symbol);
			signal.title[1].Replace(arinc.symbol, tmp);
		}

		column = header.adress[header.iDimension];
		signal.dimension = work.CellValue(row, column);
		GetMinMaxCsr(signal, work, row);	
		column = header.adress[header.iBits];
		signal.bit = GetBits(work.CellValue(row, column), (int)signal.numWord.value.size());
		signal.comment = GetComment(work, row, merge, signal.bitSign);

		bool bEmpty = IsEmpty(work, row);	
		bool bRemark = IsRemark(work, row);
		
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
		
		// TODO: Исправление костыля (обход повторений в случае конца таблицы)
		if (work.CountRows() == row + merge - 1 && !arinc.flag && arinc.amount != arinc.current)
			row = arinc.startRow - merge;
	}
	// TODO: Флаг для кпрно35
	if (!bProj)
		ConcatDW(signals);
}

// Чтение циклов повторений в ARINC протоколе (порядковый номер в кадре)
void CReaderExcel::GetArinc(const CString& field, const long& row, arincData& arinc) 
{
	CString numeric = field;
	int posEqual = numeric.Find(L'=');

	if (posEqual != -1) 
	{
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

// Объединение двойных слов
void CReaderExcel::ConcatDW(vector <signalData>& signals)
{
	CString LOW_PART = L"(мл.ч)";
	CString HIGHT_PART = L"(ст.ч)";

	for (size_t i = 0; i < signals.size(); i++)
	{
		if (signals[i].title[0].Find(LOW_PART) != -1) 
		{
			int numeric = ParsePart(signals[i].comment);
			if (numeric != -1)
			{
				CString temp = L"мл.ч. в сл." + signals[i].numWord.value[0];
				int index = findSignalByNum(signals, i, numeric, HIGHT_PART, temp);
				if (index != -1) 
				{
					signals[i].title[0].Replace(LOW_PART, L"");
					temp.Format(L"ст.ч. в сл.%d", numeric);
					signals[i].comment.Replace(temp, L"");
					signals[i].numWord.value.push_back(signals[index].numWord.value[0]);
					signals[i].bit.value.push_back(signals[index].bit.value[0]);
					signals[i].bit.value.push_back(signals[index].bit.value[1]);

					signals.erase(signals.begin() + index);
				}
			}
		}
		else if (signals[i].title[0].Find(HIGHT_PART) != -1) 
		{
			int numeric = ParsePart(signals[i].comment);
			if (numeric != -1)
			{
				CString temp = L"ст.ч. в сл.";
				temp.Format(L"%s%d",temp, signals[i].numWord.value[0]);
				int index = findSignalByNum(signals, i, numeric, LOW_PART, temp);
				if (index != -1)
				{
					signals[i].title[0].Replace(HIGHT_PART, L"");
					temp.Format(L"мл.ч. в сл.%d", numeric);
					signals[i].comment.Replace(temp, L"");
					signals[i].numWord.value.push_back(signals[index].numWord.value[0]);
					signals[i].bit.value.push_back(signals[index].bit.value[0]);
					signals[i].bit.value.push_back(signals[index].bit.value[1]);

					signals.erase(signals.begin() + index);
				}
			}
		}
	}
}

// Парсинг строки с расположением второй части слова
int CReaderExcel::ParsePart(const CString& part)
{
	// мл.ч. в сл. // ст.ч. в сл.
	CString temp = L".ч. в сл.";
	int result = -1;
	int posPart = part.Find(temp);
	
	if (posPart != -1)
	{
		bool error = false;
		CString num = part.Mid(posPart + temp.GetLength());
		result = GetInt(num, error);
		if (error)
			result = -1;
	}
	return result;
}

int CReaderExcel::findSignalByNum(vector <signalData>& signals, size_t start, int numeric, CString& partNum, CString& partComment)
{
	int result = -1;
	for (size_t i = start; i < signals.size(); i++)
	{
		if (signals[i].numWord.value[0] == numeric)
			if (signals[i].title[0].Find(partNum) != -1 && signals[i].comment.Find(partComment) != -1)
			{
				result = i;
				break;
			}
	}
	return result;
}

// Отдельное чтение требуемых параметров таблицы
#pragma region Parameters
// Адреса (ARINC)
intData CReaderExcel::GetAdress(const CString& field, const int& num) 
{
	intData result;
	CString numeric = field;
	int adr = -1;
	int posDel, current = num;
	bool flag = true;

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
	} 
	while (numeric.GetLength() != 0 && flag);

	result.value.push_back(adr);
	result.field = field;
	return result;
}

// Доп функция для адреса
vector <int> CReaderExcel::StepAdress(const CString& adress, bool& flag) 
{
	vector <int> result = {-1, -1};
	CString num = adress;
	int indxDel = num.Find(L'.') == -1 ? num.Find(L'…') : num.Find(L'.');	// Поиск индекса разделителей

	// Разделителей нет, используется один разряд
	if (indxDel == -1) 
	{ 
		result[1] = GetSubIndex(num);
		result[0] = GetInt(num, flag);
	}
	else 
	{
		result.push_back(-1);
		result.push_back(-1);
		int posDot = num.ReverseFind(L'.');
		if (posDot != -1)
			num.Delete(indxDel, posDot - indxDel);

		CString num2 = num;
		num.Delete(indxDel, num.GetLength());	// Первое число
		num2.Delete(0, indxDel + 1);			// Второе число
		num.Trim();	num2.Trim();
		
		result[1] = GetSubIndex(num);
		result[3] = GetSubIndex(num2);
		result[0] = GetInt(num, flag);
		result[2] = GetInt(num2, flag);
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

// Номер слова
intData CReaderExcel::GetNumWord(const CString& field) 
{
	intData result;
	vector <int> vec = { -1 };
	CString numeric = field;
	int posDot = numeric.Find(L',');
	numeric.Trim();

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
	result.field = field;
	return result;
}

// Минимальное, максимальное и цср
void CReaderExcel::GetMinMaxCsr(signalData& signal, CWorkExcel& work, const long& row) 
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

// Используемые разряды
intData CReaderExcel::GetBits(const CString& field, const int& size)
{
	intData result;
	CString bits = field;
	int posDot = bits.Find(L',');
	vector <int> vec;

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
	result.field = field;
	result.value = vec;
	return result;
}

// Дополнительная функция для перевода разрядов
vector <int> CReaderExcel::StepBits(const CString& bits, bool& flag) 
{
	vector <int> result = { -1, -1 };
	CString num = bits;
	num.Trim();

	int indxDel = num.Find(L'.') == -1 ? num.Find(L'…') : num.Find(L'.');	// Поиск индекса разделителей

	if (indxDel == -1)	// Разделителей нет, используется один разряд
		result[0] = GetInt(num, flag);
	else
	{
		int posDot = num.ReverseFind(L'.');
		if (posDot != -1)
			num.Delete(indxDel, posDot - indxDel);

		CString num2 = num;
		num.Delete(indxDel, num.GetLength());	// Первое число
		num2.Delete(0, indxDel + 1);	// Второе число
		num2.Trim();	num.Trim();

		result[0] = GetInt(num, flag);
		result[1] = GetInt(num2, flag);
	}
	return result;
}

// Чтение примечания
CString CReaderExcel::GetComment(CWorkExcel& work, const long& row, const int& size, bool& flag) 
{
	long column = header.adress[header.iComment];
	long tmpRow = row;
	int merge = work.GetMerge(tmpRow, column);
	CString result;

	size > merge ? merge = size : merge = merge;
	for (int i = 0; i < merge; i++) 
	{
		CString tmp = work.CellValue(tmpRow + i, column);
		if (!tmp.IsEmpty()) 
		{
			(tmp.Find(SIGN_FIELD) != -1) ? flag = true : flag = flag;
			result.IsEmpty() ? result = tmp : result += L'\n' + tmp;
		}
	}
	return result;
}
#pragma endregion

#pragma region Converters
// Конвертер double значения
double CReaderExcel::GetDouble(const CString& field, bool& flag) 
{
	double result = DBL_MIN;
	CString temp = field;
	temp.Trim();
	temp.Replace(L',', L'.');
	CStringA tmp(temp);
	char* str = new char[tmp.GetLength() + 1];
	strcpy_s(str, tmp.GetLength() + 1, tmp);

	if (!temp.IsEmpty()) 
	{
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
int CReaderExcel::GetInt(const CString& field, bool& flag) 
{
	int result = -1;
	CString temp = field;
	temp.Trim();
	CStringA tmp(temp);
	char* str = new char[tmp.GetLength() + 1];
	strcpy_s(str, tmp.GetLength() + 1, tmp);

	if (!field.IsEmpty()) 
	{
		char* end;
		errno = 0;
		result = strtol(str, &end, 10);
		(*end != '\0' || errno != 0) ? flag = true : flag = flag;
	}
	else
		flag = true;

	delete[] str;
	return result;
}
#pragma endregion

// Различные методы проверок
#pragma region SUB_FUNCTION
// Проверка заголовка
bool CReaderExcel::IsTitle(CWorkExcel& work, const long& row)
{
	bool result = false;
	CString numeric = work.CellValue(row, 1);
	if (!numeric.IsEmpty())
		GetInt(numeric, result);
	return result;
}

// Проверка пустоты
bool CReaderExcel::IsEmpty(CWorkExcel& work, const long& row) 
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

// Проверка примечания
bool CReaderExcel::IsRemark(CWorkExcel& work, const long& row)
{
	bool result = false;
	for (long i = 1; i < header.size; i++) 
	{
		long column = header.adress[i];
		if (header.adress[i] != -1)
		{
			CString tmp = work.CellValue(row, column);
			result = (tmp.Find(REMARK1) > -1 ||
				tmp.Find(REMARK2) > -1) ? true : result;
		}
	}
	return result;
}

// Проверка расширения
bool CReaderExcel::CheckExtension(const CString& path)
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