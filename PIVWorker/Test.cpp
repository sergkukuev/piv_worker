#include "stdafx.h"
#include "Test.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Конструктор
CTest::CTest() 
{	
	// Множество исключений
	// Для ARINC
	exception.insert(L"ID_BPV1");	// Идентификатор блока 1
	exception.insert(L"ID_BPV2");	// Идентификатор блока 2

	// Создание базы для проверки на ошибки с помощью регулярных выражений
	base.resize(index::size);
	base[numword].correct = "^[0-9]+(, ?[0-9]+)?$";
	base[numword].incorrect = 
	{
		{ "^[ \t\n]*$", L"Значение <b>№ слова</b> отсутствует." },
		{ "[^0-9 \t\n,]", L"Значение <b>№ слова</b> содержит недопустимые символы." },
		{ "[^,][ \t\n]", L"Значение <b>№ слова</b> содержит лишние пробелы. (Допускается только после запятой)" },
		{ ",[^,]*,+", L"Значение <b>№ слова</b> содержит больше одной запятой." },
		{ "^[ \t\n]*,", L"Значение <b>№ слова</b> до запятой отсутствует." },
		{ ",[ \t\n]*$", L"Значение <b>№ слова</b> после запятой отсутствует." }
	};

	base[title].correct = "^[A-Za-z]([A-za-z_0-9]*[A-Za-z0-9])?$";
	base[title].incorrect = 
	{
		{ "^[ \t\n]*$", L"<b>Обозначение сигнала</b> отсутствует. (Возможно сигнал зарезервирован, но \"Резерв\" не написано)" },
		{ "[ \t\n]", L"<b>Обозначение сигнала</b> содержит пробел." },
		{ "[^A-Za-z_ \t\n0-9]", L"<b>Обозначение сигнала</b> содержит недопустимые символы" },
		{ "^[^A-Za-z]", L"<b>Обозначение сигнала</b> начинается не с латинской буквы." },
		{ "_[ \t\n]*$", L"<b>Обозначение сигнала</b> заканчивается на недопустимый символ '_'." },
		{ "[А-Яа-я]", L"<b>Обозначение сигнала</b> содержит кириллицу." }
	};

	base[value].correct = "^-?[0-9]+(,[0-9]+)?$";
	base[value].incorrect = 
	{	// Значение '*' подставляется в методе WriteError в зависимости от проверенной ячейки (min, max, csr)
		{ "^[ \t\n]*$", L"<b>*</b> отсутствует." },
		{ "[^-,. \t\n0-9]", L"<b>*</b> содержит недопустимые символы." },
		{ "[ \t\n]", L"<b>*</b> содержит пробелы." },
		{ "(,|[.])[^,.]*([.]|,)", L"<b>*</b> содержит более одного разделителя." },
		{ "-[^-]*-", L"<b>*</b> содержит больше одного знака '-'." },
		{ "^[^- \t\n]+-", L"<b>*</b> содержит знак '-' в недопустимом месте. (Допускается только перед значением)" },
		{ "^[ \t\n]*([.]|,)", L"<b>*</b> не содержит значения до разделителя" },
		{ "([.]|,)[ \t\n]*$", L"<b>*</b> не содержит значения после разделителя" }
	};

	base[bits].correct = "^[0-9]+((…|[.]{3})[0-9]+)?(, ?[0-9]+((…|[.]{3})[0-9]+)?)?$";
	base[bits].incorrect = 
	{
		{ "^[ \t\n]*$", L"Значение <b>используемых разрядов</b> отсутствует." },
		{ "[^0-9,.… \t\n]", L"Значение <b>используемых разрядов</b> содержит недопустимые символы." },
		{ "[^ \t\n,]+[ \t\n]", L"Значение <b>используемых разрядов</b> содержит лишние пробелы. (Допускается только после запятой)" },
		{ "[.]{1,2}|[.]{4,}[^.…]*", L"Значение <b>используемых разрядов</b> содержит неверное обозначение промежутка." },
		{ "(…|[.]{2,})[^….]*(…|[.]{2,})[^….]*(…|[.]{2,})", L"Значение <b>используемых разрядов</b> содержит более двух промежутков." },	// TODO: Отладить регулярку получше
		{ "(^|,)[ \t\n]*(…|[.])", L"Значение <b>используемых разрядов</b> не содержит значения в начале одного из промежутков." },
		{ "(…|[.])[ \t\n]*(,|$)", L"Значение <b>используемых разрядов</b> не содержит значения в конце одного из промежутков." }
	};
}

// Деструктор
CTest::~CTest() {	}

// Запуск проверки на ошибки
errorSet CTest::Start(bookData& book, const bool& fast) 
{
	errorSet result;
	result.book = this->book = &book;
	ASSERT(this->book != nullptr);
	for (size_t j = 0; j < book.sheets.size(); j++) 
	{
		errorSheet tmp;
		tmp.sheet = this->sheet = &book.sheets[j];
		ASSERT(this->sheet != nullptr);
		GetErrors(tmp.syntax, tmp.simantic, fast);
		GetWarnings(tmp.warning);
		result.set.push_back(tmp);
	}
	return result;
}

list <errorSet> CTest::Start(list <bookData>& books, const bool& fast) 
{
	list <errorSet> result;
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++) 
	{
		errorSet error;
		error.book = this->book = &*it;
		ASSERT(this->book != nullptr);
		for (size_t j = 0; j < it->sheets.size(); j++) 
		{
			errorSheet tmp;
			tmp.sheet = this->sheet = &it->sheets[j];
			ASSERT(this->sheet != nullptr);
			GetErrors(tmp.syntax, tmp.simantic, fast);
			GetWarnings(tmp.warning);
			error.set.push_back(tmp);
		}
		result.push_back(error);
	}
	return result;
}

// Проверка листа на синтаксические и семантические ошибки
void CTest::GetErrors(vector <errorSignal>& syntax, vector <errorSignal>& simantic, const bool& fast) 
{
		if (!sheet->arinc)	// Если линия передачи не arinc, то необходимо проверять набор параметра 
			sheet->error = NpTest(syntax);

		vector <repiter> repit;
		InitRepiter(repit);

		for (size_t i = 0; i < sheet->signals.size(); i++) 
		{
			if (sheet->signals[i].title[0].Find(RESERVE_SIGNAL) == -1) 
			{
				errorSignal tmp;
				tmp.signal = &sheet->signals[i];

				// Проход по синтаксическим ошибкам
				if (fast)
					SimpleTest(tmp) ? sheet->error = true : sheet->error = sheet->error;
				else
				{
					// TODO: Сделать регулярные выражения для ячейки адреса (arinc)
					if (sheet->arinc)
					{
						if (sheet->signals[i].numWord.flag)
							sheet->error = WriteError(tmp, L"Поле пустое или содержит недопустимые символы.", check::numword);
					}
					else
						TemplateTest(sheet->signals[i].numWord.field, check::numword, base[numword], tmp) ? sheet->error = true : sheet->error = sheet->error;

					TemplateTest(sheet->signals[i].title[1], check::title, base[title], tmp) ? sheet->error = true : sheet->error = sheet->error;
					// TODO: Три пустые ячейки не считаются ошибкой (исправить костыль)
					if (sheet->signals[i].min.flag || sheet->signals[i].max.flag || sheet->signals[i].csr.flag)
					{
						TemplateTest(sheet->signals[i].min.field, check::min, base[value], tmp) ? sheet->error = true : sheet->error = sheet->error;
						TemplateTest(sheet->signals[i].max.field, check::max, base[value], tmp) ? sheet->error = true : sheet->error = sheet->error;
						TemplateTest(sheet->signals[i].csr.field, check::csr, base[value], tmp) ? sheet->error = true : sheet->error = sheet->error;
					}
					TemplateTest(sheet->signals[i].bit.field, check::bits, base[bits], tmp) ? sheet->error = true : sheet->error = sheet->error;
				}
				if (!tmp.error.empty())
				{
					syntax.push_back(tmp);
					tmp.error.clear();
				}
				
				// Проход по семантическим ошибкам
				ValueTest(tmp) ? sheet->error = true : sheet->error = sheet->error;
				RepiterTest(tmp, (int)i) ? sheet->error = true : sheet->error = sheet->error;
				BitsTest(tmp, repit) ? sheet->error = true : sheet->error = sheet->error;

				if (!tmp.error.empty())
					simantic.push_back(tmp);
			}
		}
}

// Проверка листа на незначительные ошибки (замечания) 
// TODO: Сделать нормальный поиск по книге
void CTest::GetWarnings(vector <errorSignal>& warning) 
{
	for (size_t i = 0; i < sheet->signals.size(); i++) 
	{
		if (sheet->signals[i].title[0].Find(RESERVE_SIGNAL) == -1) 
		{
			errorSignal tmp;
			tmp.signal = &sheet->signals[i];

			FindRepiteTitleInBook(tmp, (int)i);

			if (!tmp.error.empty())
				warning.push_back(tmp);
		}
	}
}

// Запись ошибки
bool CTest::WriteError(errorSignal& res, const CString& msg, const int& index) 
{
	CString message = msg;
	if (index == check::min)
		message.Replace(L"*", L"Минимальное значение");
	else if (index == check::max)
		message.Replace(L"*", L"Максимальное значение");
	else if (index == check::csr)
		message.Replace(L"*", L"Значение ЦСР");
	
	res.error.push_back(message);
	res.check[index] = true;
	return true;
}

#pragma region Syntax
// Проверка номера набора параметров
bool CTest::NpTest(vector <errorSignal>& error) 
{
	bool result = false;	// true -  есть ошибка, false - нет ошибки
	errorSignal tmp;
	tmp.signal = &sheet->signals[0];
	ASSERT(tmp.signal != nullptr);

	if (sheet->np == 0)
		result = WriteError(tmp, L"Отсутствует значение набора параметров.", check::comment);
	else
		if (!tmp.signal->min.flag && !tmp.signal->max.flag)
			if (tmp.signal->min.value > sheet->np || sheet->np > tmp.signal->max.value)
				result = WriteError(tmp, L"Набор параметров не соответствует указанному интервалу значений.", check::comment);

	if (!tmp.error.empty()) 
		error.push_back(tmp);

	return result;
}

// Проверка всех числовых параметров
bool CTest::SimpleTest(errorSignal& set) 
{
	bool result = false; // true -  есть ошибка, false - нет ошибки
	
	if (set.signal->numWord.flag)
		result = WriteError(set, L"Поле пустое или содержит недопустимые символы.", check::numword);
	if (set.signal->min.flag)
		result = WriteError(set, L"Поле пустое или содержит недопустимые символы.", check::min);
	if (set.signal->max.flag)
		result = WriteError(set, L"Поле пустое или содержит недопустимые символы.", check::max);
	if (set.signal->csr.flag)
		result = WriteError(set, L"Поле пустое или содержит недопустимые символы.", check::csr);
	if (set.signal->bit.flag)
		result = WriteError(set, L"Поле пустое или содержит недопустимые символы.", check::bits);

	return result;
}

// Синтаксическая проверка шаблонным методом
bool CTest::TemplateTest(const CString& field, const int& check, const regBase& test, errorSignal& set) 
{
	regex correct(test.correct);
	string str = CT2A(field);
	bool result = regex_match(str, correct);

	if (!result)
	{
		bool bFind = false;	// Найдена ли ошибка из набора регулярных выражений
		// Поиск ошибки (обход по набору регулярок)
		for (size_t i = 0; i < test.incorrect.size(); i++)
		{
			regex reg(test.incorrect[i].reg);
			if (regex_search(str, reg))
			{
				result = WriteError(set, test.incorrect[i].desc, check);
				bFind = true;
			}
		}
		if (!bFind)	// Неопознанная ошибка
			throw UndefinedError(book->name, sheet->name, set.signal->numWord.field + L"\n\t" + set.signal->title[1]);
	}
	return !result;
}
#pragma endregion

#pragma region Simantic
// Проверка всех числовых параметров
bool CTest::ValueTest(errorSignal& set) 
{
	bool result = false;
	// Проверка номера слова (адреса)
	if (!set.signal->numWord.flag)
	{
		for (size_t i = 0; i < set.signal->numWord.value.size(); i++) 
		{
			// Повторение на листе номера слова или адреса
			/*int indx = IsRepitContain(repit, signal.numWord.value[i]);
			CString str;
			sheet->arinc ? str = L"адреса" : str = L"номера слова";
			str.Format(L"Значение %s %d уже содержится на этом листе.", str,signal.numWord.value[i]);
			repit[indx].bits[0] ? repit[indx].bits[0] = false : set.push_back(str);
			*/
			if (set.signal->numWord.value[i] > 32 && !sheet->arinc)
				result = WriteError(set, L"Значение номера слова должно быть меньше 32.", check::numword);
			if (set.signal->numWord.sys != 8 && sheet->arinc)
				result = WriteError(set, L"Адрес сигнала должен быть записан в 8-ой системе счисления.", check::numword);
		}
	}
	// Проверка мин, макс и цср
	if (!set.signal->min.flag && !set.signal->max.flag && !set.signal->csr.flag && !set.signal->bit.flag) 
	{
		int nBit = 0;
		(set.signal->bit.value.size() == 4) ? nBit = (set.signal->bit.value[1] - set.signal->bit.value[0]) + (set.signal->bit.value[3] - set.signal->bit.value[2]) + 2 :
			nBit = (set.signal->bit.value[1] - set.signal->bit.value[0]) + 1;

		// Выделение бита под знак, если он присутствует
		if (set.signal->bitSign) nBit--;

		double nMin = set.signal->csr.value;
		for (int i = 1; i <= nBit; i++)
			nMin = nMin / 2;

		double nMax = (set.signal->csr.value * 2) - nMin;

		if (set.signal->max.value - nMax >= 2) 
			result = WriteError(set, L"Нельзя упаковать данное значение", check::max);
		else if (((abs(set.signal->min.value) > (nMax + nMin)) || (abs(set.signal->min.value) < nMin)) && set.signal->min.value != 0) 
			result = WriteError(set, L"Нельзя упаковать данное значение", check::min);

		if ((set.signal->min.value < 0) && !set.signal->bitSign && !sheet->arinc)
			result = WriteError(set, L"Не может быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\"", check::min);
		else if ((set.signal->min.value >= 0) && set.signal->bitSign && !sheet->arinc)
			result = WriteError(set, L"Должно быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\"", check::min);
	}
	return result;
}

// Поиск повторений идентификатора на листе
bool CTest::RepiterTest(errorSignal& set, const int& index) 
{
	bool result = false;
	CString title = sheet->signals[index].title[1];
	std::set<CString>::iterator it = exception.find(title);
	// TODO: Реализовать исключения под разные линии передачи
	if (it == exception.end())	// Если идентификатор не найден в множестве исключений
		for (size_t i = index + 1; i < sheet->signals.size(); i++)
			if (sheet->signals[i].title[1].Compare(title) == 0 && sheet->signals[i].title[0].CompareNoCase(RESERVE_SIGNAL) != 0)
				result = true;

	if (result)
		WriteError(set, L"Сигнал с таким идентификатором присутствует на этом листе.", check::title);

	return result;
}

// Инициализация репитера для проверки перекрытия
void CTest::InitRepiter(vector <repiter>& repit)
{
	for (int i = 0; i < sheet->signals.size(); i++)
	{
		for (int j = 0; j < sheet->signals[i].numWord.value.size(); j++)
		{
			int indx = IsRepitContain(repit, sheet->signals[i].numWord.value[j]);
			if (indx == -1)
			{
				repiter tmp;
				tmp.adr = sheet->signals[i].numWord.value[j];
				tmp.signals.push_back(&sheet->signals[i]);
				tmp.bits.resize(MAX_BITS);	// Выделение память под биты и номер слова
				for (int k = 0; k < MAX_BITS; k++)
					tmp.bits[k] = true;
				repit.push_back(tmp);
			}
			else
			{
				repit[indx].signals.push_back(&sheet->signals[i]);
			}
		}
	}
}

// Имеется ли уже такой номер слова (адрес) (в случае удачи возвр. индекс, иначе -1)
int CTest::IsRepitContain(const vector<repiter>& repit, const int& numeric)
{
	int result = -1;
	for (int i = 0; i < repit.size(); i++)
		if (repit[i].adr == numeric)
			result = i;
	return result;
}

// Проверка используемых разрядов
bool CTest::BitsTest(errorSignal& set, vector<repiter>& repit) 
{
	bool result = false;
	// Кол-во № слов должно совпадать с кол-вами интервалов исп. разрядов
	if (!set.signal->numWord.flag && !set.signal->bit.flag) 
	{
		for (size_t i = 0; i < set.signal->bit.value.size(); i+=2)
			if (set.signal->bit.value[i] > set.signal->bit.value[i + 1] && set.signal->bit.value[i+1] != -1) 
				result = WriteError(set, L"Старший бит меньше младшего.", check::bits);
		if (set.signal->numWord.value.size() * 2 == set.signal->bit.value.size()) 
		{
			bool replaced;
			(set.signal->numWord.value.size() == 2) ? replaced = IsReplaceable(set.signal->title[0], repit[IsRepitContain(repit, set.signal->numWord.value[1])].signals) || 
				IsReplaceable(set.signal->title[0], repit[IsRepitContain(repit, set.signal->numWord.value[0])].signals) :
				replaced = IsReplaceable(set.signal->title[0], repit[IsRepitContain(repit, set.signal->numWord.value[0])].signals);
			
			vector<int> bits = CrossBits(set.signal->bit.value, set.signal->numWord.value, repit);
			if (bits.size() != 0 && !replaced)
			{
				CString str = L"Следующие биты перекрываются: ";
				for (size_t i = 0; i < bits.size(); i++)
					str.Format(L"%s %d", str, bits[i]);
				result = WriteError(set, str, check::bits);
			}
		}
		else 
			result = (set.signal->numWord.value.size() == 1) ? WriteError(set, L"Должен быть один интервал.", check::bits) : WriteError(set, L"Должно быть два интервала.",  check::bits);
	}
	return result;
}

// Проверка на перекрытие битов
vector<int> CTest::CrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit) 
{
	vector<int> result;
	for (size_t j = 0; j < 2; j += 2)
		for (size_t i = 0; i < numWord.size(); i++) 
		{
			int end, start = bits[j];
			(bits[j + 1] == -1) ? end = start : end = bits[j + 1];

			for (; start <= end; start++) 
			{
				int indx = IsRepitContain(repit, numWord[i]);
				ASSERT(start <= 32);
				ASSERT(indx != -1);
				repit[indx].bits[start] ? repit[indx].bits[start] = false :	result.push_back(start);	// отметка в матрице о наличии бита
			}
		}
	return result;
}

// Проверка на заменяемость
bool CTest::IsReplaceable(const CString& title, const vector <signalData*> signals)
{
	bool result = false;
	CString first = title;
	for (size_t i = 0; i < signals.size(); i++)
	{
		CString second = signals[i]->title[0];
		if (first.CompareNoCase(second) == 0)
			continue;

		int indx = first.ReverseFind(L',');
		if (indx != -1)
			first.Delete(indx, first.GetLength() - indx);
		indx = second.ReverseFind(L',');
		if (indx != -1)
			second.Delete(indx, second.GetLength() - indx);
		first.CompareNoCase(second) == 0 ? result = true : result = result;
	}

	return result;
}
#pragma endregion

#pragma region Warning
// Поиск повторений идентификатора в книге
void CTest::FindRepiteTitleInBook(errorSignal& set, const int& index)
{
	vector <CString> repitSheet;
	CString title = sheet->signals[index].title[1];

	for (size_t i = 0; i < book->sheets.size(); i++)
	{
		int start;
		book->sheets[i].name.Compare(sheet->name) == 0 ? start = index + 1 : start = 0;
		for (size_t j = start; j < book->sheets[i].signals.size(); j++)
		{
			if (book->sheets[i].signals[j].title[1].Compare(title) == 0 && book->sheets[i].signals[j].title[0].CompareNoCase(RESERVE_SIGNAL) == -1)
			{
				repitSheet.push_back(book->sheets[i].name);
				continue;
			}
		}
	}
	if (repitSheet.size() != 0)
	{
		CString msg = L"Сигнал с таким идентификатором встречается на следующих листах в книге:\n";
		for (size_t i = 0; i < repitSheet.size(); i++)
			msg += repitSheet[i] + L", ";
		int posDot = msg.ReverseFind(L',');
		msg.Delete(posDot);
		WriteError(set, msg, check::title);
	}
}
#pragma endregion