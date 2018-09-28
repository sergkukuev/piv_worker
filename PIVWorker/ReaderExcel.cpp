#include "stdafx.h"
#include "ReaderExcel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace data_pw;

// Конструктор
CReader::CReader()
{
	// Установка набора заголовков:
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
}

// Деструктор
CReader::~CReader()
{
	header.list.clear();
	header.list.shrink_to_fit();
}

// Чтение протокола
bool CReader::ReadBook(const CString& path, CData& data)
{
	if (!ExtensionChecker(path))
		throw BadTypeException(BookName(path));

	if (!OpenBook(path))
		throw ReadBookException(BookName(path));

	data.SetName(BookName());
	log >> L"Добавление протокола \"" + data.Name() + L"\"...";
	GetSheets(data.Sheets());

	return false;
}

// Чтение таблиц протоколов (листов)
void CReader::GetSheets(vector <CData::Sheet>& sheets)
{
	sheets.resize(CountSheets());
	for (long i = 1; i < CountSheets() + 1; i++)
	{
		try
		{
			if (!OpenSheet(i))	// Лист пустой
				throw ReadSheetException(SheetName());

			sheets[i - 1].head.name = SheetName();
			sheets[i - 1].head.line = SetLine();

			if (!FindHeader(sheets[i - 1].head.line))	// Поиск заголовков таблицы
				throw NotAllHeaderException(sheets[i - 1].head.name);

			header.adress[hRow]++;
			GetSignals(sheets[i - 1].signals, sheets[i-1].remarks);
			stg.GetProject() == stgdll::project::kprno35 ? SetPuiPage(sheets[i - 1]) : sheets[i - 1].head.pk = GetPk();
			sheets[i - 1].head.line == ARINC ? sheets[i - 1].head.np = (int)status::opt : SetNp(sheets[i - 1]);

			if (sheets[i - 1].head.pk < 0)	// Значение меньше нуля -> ошибка чтения
				sheets[i - 1].head.pk == (int)status::empty ?
				log >> L"Номер подкадра отсутствует на листе \"" + sheets[i - 1].head.name + L"\"" :
				log >> L"Не удалось прочитать номер подкадра на листе \"" + sheets[i - 1].head.name + L"\"";

			if (sheets[i - 1].head.np < 0)	// Значение меньше нуля -> ошибка чтения
				sheets[i - 1].head.np == (int)status::empty ?
				log >> L"Номер набора параметров отсутствует на листе \"" + sheets[i - 1].head.name + L"\"" :
				log >> L"Не удалось прочитать номер набора параметров на листе \"" + sheets[i - 1].head.name + L"\"";
		}
		catch (MyException& exc)
		{
			log >> exc.GetMsg();
		}
	}
}

// Чтение параметров на листе
void CReader::GetSignals(vector <CData::Signal>& signals, vector<CData::DevRemark>& remarks)
{
	for (long merge = 1, row = header.adress[hRow], column; row <= CountRows(); row += merge, merge = 1)
	{
		CData::Signal signal;
		// Надстройка для arinc
		if (bArinc)
			if (!IsTitle(row))
				ArincChecker(arinc, row);
			else
				continue;

		// Чтение параметров одного сигнала. Адрес (для ARINC) записывается вместо номера слова
		!bArinc ? column = header.adress[hNumWord] : column = header.adress[hAdress];
		int range = CPrevEmpty(row, column);
		signal.value[NUMWORD] = CellValue(row - range, column);

		column = header.adress[hName];
		int range = CPrevEmpty(row, column);
		signal.value[TITLE] = CellValue(row - range, column);
		range = CNextEmpty(row, column);

		column = header.adress[hSignal];
		merge = CNextEmpty(row, column);
		signal.value[ID] = CellValue(row, column);

		if (range < merge)
			merge = range;

		column = header.adress[hDimension]; 
		signal.value[TYPE] = CellValue(row, column);
		column = header.adress[hMin];
		signal.value[MIN] = CellValue(row, column);
		column = header.adress[hMax];
		signal.value[MAX] = CellValue(row, column);
		column = header.adress[hCSR];
		signal.value[CSR] = CellValue(row, column);
		column = header.adress[hBits];
		signal.value[BITS] = CellValue(row, column);
		signal.value[COMMENT] = GetComment(row, merge);

		bool bEmpty = IsEmpty(row);
		bool bRemark = IsRemark(row);
		// TODO: Записать информацию под таблицей для дальнейшей расшифровки
		if (!bEmpty && !bRemark)
			signals.push_back(signal);
		else if (bRemark)
			break;
	}
}

// Поиск заголовков на листе
bool CReader::FindHeader(const int& lineType)
{
	for (size_t i = 0; i < header.list.size(); i++)
	{
		std::vector<CString>::iterator it = header.list[i].begin();
		bool bFind = false;
		Cell cell = Start();

		for (it; !bFind; it++)
		{
			if ((lineType != ARINC) && i + 1 == hAdress)
				break;
			if ((it == header.list[i].end()) && !bFind)
				return false;
			bFind = FindCell(*it, cell);
		}
		if ((lineType != ARINC) && i + 1 == hAdress)
		{
			header.adress[i + 1] = -1;
			continue;
		}

		header.adress[hRow] = cell.row;
		header.adress[i + 1] = cell.column;
	}
	return true;
}

#pragma region DoubleWord
// Объединение двойных слов (установка указателя)
void CReader::ConcatDW(vector <signalData>& signals)
{
	for (size_t i = 0; i < signals.size(); i++)
	{
		for (size_t j = 0; j < dwPart::low.size(); j++)
		{
			ASSERT(dwPart::low.size() != dwPart::hight.size() != dwPart::hight2.size());
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
void CReader::findDW(vector <signalData>& signals, size_t start, CString old, vector <CString> revert)
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
#pragma endregion

#pragma region Arinc
//  Поиск повторяющихся блоков
void CReader::ArincChecker(arincData& arinc, long& row)
{
	//	Поиск повторений
	CString field = CellValue(row - 1, 1);
	if (field.Find(arincRemark) != -1 && arinc.flag && arinc.startRow != row)	// Опачки, появилось
		GetArinc(arinc, row, field);
	else if ((field.Find(arincRemark) != -1 || IsTitle(row - 1)) && !arinc.flag)
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
void CReader::GetArinc(arincData& arinc, const long& row, CString numeric)
{
	int posEqual = numeric.Find(L'=');
	if (posEqual != -1)
	{
		arinc.symbol = numeric[posEqual - 1];
		myctl::Cyr2Lat(arinc.symbol);	// Перевод кириллицы в латиницу
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

// Чтение адреса
intData CReader::GetAdress(CString numeric, int current)
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
vector <int> CReader::StepAdress(CString numeric, bool& flag)
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
		numeric2.Delete(0, indxDel + 1);				// Второе число
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
// TODO: В случае изменения системы счисления (СС), отличной от 8-ой, может выдавать некорректный результат
int CReader::GetSubIndex(CString& numeric)
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
#pragma endregion

#pragma region SheetInfo
// Поиск сигнала по идентификатору
int CReader::FindSignalById(const vector<CData::Signal>& signals, const CString& id)
{
	int index = -1;	// Не определен
	for (size_t i = 0; i < signals.size(); i++)
		if (signals[i].value[ID].Find(id) == 0)
		{
			index = (int)i;
			break;
		}
	return index;
}

// Значение номера набора параметров
void CReader::SetNp(CData::Sheet& sheet)
{
	const vector <CString> npId = { L"NP", L"IK_MFPI" /* МФПИ35 */ };	// Идентификаторы, в которых зашиты номера наборов в ПИВ
	vector<int> result = ParseValueById(sheet.signals, npId);
	sheet.head.iNp = result[0];
	sheet.head.np = result[1];
}

// Значение страницы ПУИ
void CReader::SetPuiPage(CData::Sheet& sheet)
{
	const vector <CString> puiId = { L"PagePUI" };	// Идентификаторы, в которых зашиты номера страниц ПУИ (для КАИ 35)
	vector<int> result  = ParseValueById(sheet.signals, puiId);
	sheet.head.iPk = result[0];
	sheet.head.pk = result[1];
}

// Выделение значения из комментария по идентификатору (для ПУИ и НП)
vector<int> CReader::ParseValueById(const vector<CData::Signal>& signals, const vector<CString>& id)
{
	vector<int> res = { -1 /* индекс */, -1 /* значение */};
	int iId = -1;
	for (size_t i = 0; i < id.size(); i++)	// Находим индекс идентификатора
	{
		res[0] = FindSignalById(signals, id[i]);
		if (res[0] != -1)
		{
			iId = (int)i;
			break;
		}
	}
	// Соответствующий идентификатор не найден
	if (res[0] == -1)
	{
		res[1] = (int)data_pw::status::empty;
		return res;
	}

	CString comment = signals[res[0]].value[COMMENT];
	if (!comment.IsEmpty())
	{
		int pos = comment.Find(id[iId]);
		if (pos == -1)	// Поле не найдено
		{
			res[1] = (int)data_pw::status::failed;
			return res;
		}
		comment.Remove(L' ');	// Удаляем все пробелы для удобного выделения значения
		// TODO: Пропарсить на разделяющие знаки (, . ! \n \t и т.д.)
		bool result = true;
		res[1] = GetInt(comment.Mid(pos + id[iId].GetLength() + 1), result);	// Длина идентификатора + символ "="
	}
	return res;
}

// Получение значения номера подкадра
int CReader::GetPk()
{
	// CRUTCH: Цикл для прохода по таблице вправо, чтобы найти номер подкадра
	for (long i = header.adress[hComment]; i <= Boundary().column; i++)
	{
		CString item = CellValue(header.adress[hRow] - 1, i);
		if (!item.IsEmpty())
		{
			int pos = item.ReverseFind(L'№');
			if (pos == -1)
				continue;
			item.Delete(0, pos + 1);
			item.Trim();
			// Вдруг дальше пробел
			pos = item.Find(L' ');
			if (pos != -1)
				item.Delete(pos, item.GetLength());
			// А вдруг перечисление через запятую (протоколы не поймешь)
			pos = item.Find(L',');
			if (pos != -1)
				item.Delete(pos, item.GetLength());

			bool flag = false;
			return GetInt(item, flag);
		}
	}
	return (int)status::empty;
}
// UNDONE: Наработки для работы с базой данных (определение номеров наборов и ПУИ страниц)
/*
#pragma region NP_BASE
// Чтение номеров наборов из папки odb
void CWorkExcel::ReadNpBase()
{
vector<CString> fileNames;	// Найденные имена файлов
// Поиск всех файлов в папке
WIN32_FIND_DATA findFiles;
HANDLE h;
CString folder = settings.GetDefaultPath() + L"\\odb\\*";
h = FindFirstFile(folder, &findFiles);
if (h != INVALID_HANDLE_VALUE)
{
do
{
fileNames.push_back(findFiles.cFileName);
} while (FindNextFile(h, &findFiles) != 0);
FindClose(h);
}
folder.Delete(folder.GetLength() - 1);	// Удаление звездочки из пути
// Чтение файлов
for (size_t i = 0; i < fileNames.size(); i++)
{
// Проверка разрешения
int posDot = fileNames[i].ReverseFind(L'.');
CString temp = fileNames[i].Mid(posDot, fileNames[i].GetLength() - posDot);
if (temp.Compare(L".np") != 0)
continue;

// Построчная работа с файлом
temp.Format(L"%s\\%s", folder, fileNames[i]);
CStdioFile file;
BOOL bOper = file.Open(temp, CFile::modeRead | CFile::typeText);
do
{
bOper = file.ReadString(temp);
// Смена кодировки с 1252 -> 1251
int nChar = ::WideCharToMultiByte(CP_UTF8, 0, temp, (int)temp.GetLength(), NULL, 0, NULL, NULL);
string result;
result.resize(nChar);
::WideCharToMultiByte(1252, 0, temp, (int)temp.GetLength(), const_cast<char*>(result.c_str()), nChar, NULL, NULL);
// Парсинг
np_s np;
bool error = false;
size_t pos = result.find_first_of("\t");
np.number = ParseNp(result.substr(0, pos), error);
if (!error)
{
result = result.substr(pos + 1, result.size());
pos = result.find_first_of("\t");
np.name = ParseFrameName(result.substr(0, pos));
AddToNpBase(np);
}
} while (bOper);
file.Close();
}
}

// Добавление в базу наборов параметров
void CWorkExcel::AddToNpBase(const np_s& np)
{
bool find = false;
for (size_t i = 0; i < npBase.size() && !find; i++)
if (npBase[i].name.Compare(np.name) == 0)
find = true;
if (!find)
npBase.push_back(np);
}

// Поиск номера набора в базе  (-1 в случае не найденого номера)
int CWorkExcel::CmpWithNpBase(const vector<CString>& sheetInfo)
{
set <CString> prepos = { L"о", L"об", L"по", L"под", L"от", L"около", L"за", L"над", L"на",  L"с",  L"к",  L"из",  L"в",  L"без" };	// Предлоги
typedef struct {
int numeric = 0;	// Количество совпадающих токенов
int all = 0;		// Количество всех токенов
} words;

CString separate = L" ";
int index = -1, result = -1, idxEmp = -1; // idxEmp - индекс пустого индикационного кадра
words res;

for (size_t i = 0; i < npBase.size(); i++)
{
if (npBase[i].name.CompareNoCase(L"Пустой"))
idxEmp = (int)i;
int pos = 0;
words tmp;
CString token = npBase[i].name.Tokenize(separate, pos);
while (!token.IsEmpty() && prepos.find(token) == prepos.end())
{
tmp.all++;
for (size_t j = 0; j < sheetInfo.size(); j++)
if (sheetInfo[j].Find(token) != -1)
tmp.numeric++;

token = npBase[i].name.Tokenize(separate, pos);
}
if (res.numeric < tmp.numeric || (res.numeric == tmp.numeric && res.all > tmp.all))
{
res = tmp;
index = (int)i;
}
}
if (index != -1)
result = npBase[index].number;

return result;
}

// Просмотр информации над шапкой для поиска имени таблицы
vector<CString> CWorkExcel::GetSheetInfo(const Header& header)
{
vector <CString> result;
Cell res;
FindCell(LINE_FIELD, res);
if (res.row == -1 || res.column == -1)
return result;

for (long i = 1; i < header.adress[header.iRow] - 1; i++)
for (long j = res.column + 2; j < last.column; j++)	// Смещение от определения Линии на два значения
if (!CellValue(i, j).IsEmpty())
result.push_back(CellValue(i, j));
return result;
}

// Парсинг номера набора
int CWorkExcel::ParseNp(string value, bool& flag)
{
int result = -1;
if (!value.empty())
{
char* end;
errno = 0;
result = strtol(value.c_str(), &end, 10);
(*end != '\0' || errno != 0) ? flag = true : flag = flag;
}
else
flag = true;
return result;
}

// Парсинг имени кадра
CString CWorkExcel::ParseFrameName(string value)
{
CString res(value.c_str());
int pos1, pos2;
// Удаление ИК обозначения
do
{
pos1 = res.Find(L"ИК \"");
pos2 = res.Find(L"ИК\"");
if (pos1 != -1)
res.Delete(pos1, 2);
if (pos2 != -1)
res.Delete(pos2, 2);
} while (pos1 != -1 || pos2 != -1);

// Удаление вхождений всех кавычек
DeleteSymbol(res, L"\"");
DeleteSymbol(res, L"«");
DeleteSymbol(res, L"»");
do // Удаление скобок и их содержимых
{
pos1 = res.Find(L'(');
pos2 = res.Find(L')');
int a = res.GetLength();
if (pos1 != -1 && pos2 != -1 && pos1 < pos2)
res.Delete(pos1, pos2 - pos1 + 1);
} while (pos1 < pos2);
res.Trim();
return res;
}

// Удаление символа из строки
void CWorkExcel::DeleteSymbol(CString& field, const CString& sym)
{
int pos;
do
{
pos = field.Find(sym);
if (pos != -1)
field.Delete(pos);
} while (pos != -1);
}
#pragma endregion
*/
#pragma endregion

#pragma region ReadParameters
// Чтение номера слова
intData CReader::GetNumWord(CString numeric)
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
void CReader::GetMinMaxCsr(signalData& signal, const long& row)
{
	long column = header.adress[hMin];
	signal.min.field = CellValue(row, column);
	signal.min.value = GetDouble(signal.min.field, signal.min.flag);

	column = header.adress[hMax];
	signal.max.field = CellValue(row, column);
	signal.max.value = GetDouble(signal.max.field, signal.max.flag);

	column = header.adress[hCSR];
	signal.csr.field = CellValue(row, column);
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
intData CReader::GetBits(CString bits, const int& size)
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
vector <int> CReader::StepBits(CString numeric, bool& flag)
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
CString CReader::GetComment(long row, const int& size)
{
	int merge = GetMerge(row, header.adress[hComment]);
	CString result;

	size > merge ? merge = size : merge = merge;
	for (int i = 0; i < merge; i++)
	{
		CString tmp = CellValue(row + i, header.adress[hComment]);
		if (!tmp.IsEmpty())
			result.IsEmpty() ? result = tmp : result += L'\n' + tmp;
	}
	return result;
}
#pragma endregion

#pragma region Converters
// Конвертер int значения
int CReader::GetInt(CString field, bool& flag)
{
	int result = -1;
	CStringA tmp(field);
	char* buffer = new char[tmp.GetLength() + 1];
	strcpy_s(buffer, tmp.GetLength() + 1, tmp);
	result = GetInt(buffer, flag);
	delete[] buffer;
	return result;
}

// Перегрузка для string (в случае неудачи, вернется значение -1)
int CReader::GetInt(string value, bool& flag)
{
	int result = -1;
	if (!value.empty())
	{
		char* end;
		errno = 0;
		result = strtol(value.c_str(), &end, 10);
		(*end != '\0' || errno != 0) ? flag = true : flag = flag;
	}
	else
		flag = true;
	return result;
}

// Конвертер double значения
double CReader::GetDouble(CString field, bool& flag)
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
bool CReader::IsTitle(const long& row)
{
	bool result = false;
	CString numeric = CellValue(row, 1);
	if (!numeric.IsEmpty())
		GetInt(numeric, result);
	return result;
}

// Проверка строки на пустоту
bool CReader::IsEmpty(const long& row)
{
	bool result = true;
	for (long i = 1; i < hSize; i++)
	{
		long column = header.adress[i];
		if (header.adress[i] != -1)
		{
			CString tmp = CellValue(row, column);
			if (!tmp.IsEmpty())
				result = false;
		}
	}
	return result;
}

// Проверка строки на примечание
bool CReader::IsRemark(const long& row)
{
	bool result = false;
	for (long i = 1; i < hSize; i++)
	{
		long column = header.adress[i];
		if (header.adress[i] != -1)
		{
			CString tmp = CellValue(row, column);
			for (size_t j = 0; j < remark1.size(); j++)
				result = tmp.Find(remark1[j]) > -1 ? true : result;
		}
	}
	return result;
}

// Установка линии передачи
int CReader::SetLine()
{
	CString field = L"Линия";
	int result = NOT_DEFINED;	// Линия передач не определена
	Cell cell;
	if (FindCell(field, cell))
		for (size_t i = 0; i < line.size(); i++)
			if (CellValue(cell.row, cell.column + 1).Find(line[i]) != -1)
				result = (int)i;

	return result;
}

// Проверка расширения файла
bool CReader::ExtensionChecker(const CString& path)
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