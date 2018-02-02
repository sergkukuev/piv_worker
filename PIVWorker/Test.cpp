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
	exception.arinc.insert(L"ID_BPV1");	// Идентификатор блока 1
	exception.arinc.insert(L"ID_BPV2");	// Идентификатор блока 2
	// Для MKIO 
	// Пусто

	// Создание базы для проверки на ошибки с помощью регулярных выражений
	// TODO: Добавить регулярные выражения для адреса (ARINC)
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
		// TODO: Отладить регулярное выражение
		{ "(…|[.]{2,})[^….]*(…|[.]{2,})[^….]*(…|[.]{2,})", L"Значение <b>используемых разрядов</b> содержит более двух промежутков." },
		{ "(^|,)[ \t\n]*(…|[.])", L"Значение <b>используемых разрядов</b> не содержит значения в начале одного из промежутков." },
		{ "(…|[.])[ \t\n]*(,|$)", L"Значение <b>используемых разрядов</b> не содержит значения в конце одного из промежутков." }
	};
}

// Деструктор
CTest::~CTest() {	}

// Запуск проверки на ошибки
errorData CTest::Start(bookData& book, const int& iMethod) 
{
	errorData result;
	this->iMethod = iMethod;
	result.book = this->book = &book;
	ASSERT(this->book != nullptr);
	for (size_t j = 0; j < book.sheets.size(); j++) 
	{
		errorSheet tmp;
		tmp.data = this->sheet = &book.sheets[j];
		ASSERT(this->sheet != nullptr);
		GetErrors(tmp.syntax, tmp.simantic);
		GetWarnings(tmp.warning);
		result.set.push_back(tmp);
	}
	return result;
}

list <errorData> CTest::Start(list <bookData>& books, const int& iMethod) 
{
	list <errorData> result;
	this->iMethod = iMethod;
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++) 
	{
		errorData error;
		error.book = this->book = &*it;
		ASSERT(this->book != nullptr);
		for (size_t j = 0; j < it->sheets.size(); j++) 
		{
			errorSheet tmp;
			tmp.data = this->sheet = &it->sheets[j];
			ASSERT(this->sheet != nullptr);
			GetErrors(tmp.syntax, tmp.simantic);
			GetWarnings(tmp.warning);
			error.set.push_back(tmp);
		}
		result.push_back(error);
	}
	return result;
}

// Проверка листа на синтаксические и семантические ошибки
void CTest::GetErrors(vector <errorSignal>& syntax, vector <errorSignal>& simantic) 
{
	if (!sheet->arinc)	// Если линия передачи не arinc, то необходимо проверять набор параметра 
		sheet->error = NpTest(syntax);

	InitRepiter();

	for (size_t i = 0; i < sheet->signals.size(); i++) 
	{
		if (sheet->signals[i].title[0].Find(RESERVE_SIGNAL) == 0)
			continue;

		errorSignal signal;
		signal.data = &sheet->signals[i];

		SyntaxChecker(signal, (int)i);
		if (!signal.error.empty())
		{
			syntax.push_back(signal);
			sheet->error = true;
			signal.error.clear();
		}

		SimanticCheker(signal, (int)i, repit);
		if (!signal.error.empty())
		{
			simantic.push_back(signal);
			sheet->error = true;
			signal.error.clear();
		}
	}

	ClearRepiter();
}

// Проверка листа на незначительные ошибки (замечания) 
// TODO: Сделать нормальный поиск по книге
void CTest::GetWarnings(vector <errorSignal>& warning) 
{
	for (size_t i = 0; i < sheet->signals.size(); i++) 
	{
		if (sheet->signals[i].title[0].Find(RESERVE_SIGNAL) == 0) 
		{
			errorSignal tmp;
			tmp.data = &sheet->signals[i];

			FindRepiteTitleInBook(tmp, (int)i);

			if (!tmp.error.empty())
				warning.push_back(tmp);
		}
	}
}

// Запись ошибки
bool CTest::WriteError(errorSignal& signal, CString msg, const int& index) 
{
	if (index == check::min)
		msg.Replace(L"*", L"Минимальное значение");
	else if (index == check::max)
		msg.Replace(L"*", L"Максимальное значение");
	else if (index == check::csr)
		msg.Replace(L"*", L"Значение ЦСР");
	
	signal.error.push_back(msg);
	signal.check[index] = true;
	return true;
}

#pragma region Syntax
// Проверка всех параметров сигнала на синтаксические ошибки
void CTest::SyntaxChecker(errorSignal& signal, const int& i)
{
	// Проход по синтаксическим ошибкам
	if (iMethod == method::patterned)
	{
		if (sheet->arinc && sheet->signals[i].numWord.flag)
			WriteError(signal, L"Поле пустое или содержит недопустимые символы.", check::numword);	// TODO: Добавить регулярные выражения для адреса (ARINC)
		else
			TemplateTest(sheet->signals[i].numWord.field, check::numword, index::numword, signal);

		TemplateTest(sheet->signals[i].title[1], check::title, index::title, signal);
		// TODO: Исправить костыль (три пустые ячейки не считаются ошибкой)
		if (sheet->signals[i].min.flag || sheet->signals[i].max.flag || sheet->signals[i].csr.flag)
		{
			TemplateTest(sheet->signals[i].min.field, check::min, index::value, signal);
			TemplateTest(sheet->signals[i].max.field, check::max, index::value, signal);
			TemplateTest(sheet->signals[i].csr.field, check::csr, index::value, signal);
		}
		TemplateTest(sheet->signals[i].bit.field, check::bits, index::bits, signal);
	}
	else
		SimpleTest(signal);
}

// Проверка номера набора параметров
bool CTest::NpTest(vector <errorSignal>& signals) 
{
	bool result = false;	// true -  есть ошибка, false - нет ошибки
	errorSignal signal;
	signal.data = &sheet->signals[0];
	ASSERT(signal.data != nullptr);

	#define FL(x) signal.data->x.flag
	#define VAL(x) signal.data->x.value

	if (sheet->np == 0)
		result = WriteError(signal, L"Отсутствует значение набора параметров.", check::comment);
	else if ((!FL(max) && !FL(min)) && (VAL(min) > sheet->np || sheet->np > VAL(max)))
		result = WriteError(signal, L"Набор параметров не соответствует указанному интервалу значений.", check::comment);

	if (!signal.error.empty()) 
		signals.push_back(signal);

	return result;
}

// Проверка всех числовых параметров
bool CTest::SimpleTest(errorSignal& signal) 
{
	bool result = false; // true -  есть ошибка, false - нет ошибки
	CString msg = L"Поле пустое или содержит недопустимые символы.";
	
	if (signal.data->numWord.flag)
		result = WriteError(signal, msg, check::numword);
	if (signal.data->min.flag)
		result = WriteError(signal, msg, check::min);
	if (signal.data->max.flag)
		result = WriteError(signal, msg, check::max);
	if (signal.data->csr.flag)
		result = WriteError(signal, msg, check::csr);
	if (signal.data->bit.flag)
		result = WriteError(signal, msg, check::bits);

	return result;
}

// Синтаксическая проверка шаблонным методом
bool CTest::TemplateTest(const CString& field, const int& check, const int& index, errorSignal& signal) 
{
	regex correct(base[index].correct);
	string str = CT2A(field);
	bool result = regex_match(str, correct);

	if (!result)
	{
		bool bFind = false;	// Найдена ли ошибка из набора регулярных выражений
		// Поиск ошибки (обход по набору регулярок)
		for (size_t i = 0; i < base[index].incorrect.size(); i++)
		{
			regex reg(base[index].incorrect[i].reg);
			if (regex_search(str, reg))
			{
				result = WriteError(signal, base[index].incorrect[i].desc, check);
				bFind = true;
			}
		}
		if (!bFind)	// Неопознанная ошибка
			throw UndefinedError(book->name, sheet->name, signal.data->numWord.field + L"; " + signal.data->title[1]);
	}
	return !result;
}
#pragma endregion

#pragma region Simantic
// Проверка всех параметров сигнала на семантические ошибки
void CTest::SimanticCheker(errorSignal& signal, const int& i, vector <repiter>& repit)
{
	// TODO: Сделать проверку в проекте КПРНО35 на отсутствие одной из частей слова (мл. или ст.)
	ValueTest(signal);
	TitleRepitTest(signal, (int)i);
	BitsTest(signal);
}

// Проверка всех числовых параметров
bool CTest::ValueTest(errorSignal& signal) 
{
	bool result = false;
	#define FL(x) signal.data->x.flag
	#define VAL(x) signal.data->x.value
	
	// Проверка номера слова (адреса)
	if (!FL(numWord))
	{
		for (size_t i = 0; i < VAL(numWord).size(); i++)
		{
			if (VAL(numWord)[i] < 0 && !sheet->arinc)
				result = WriteError(signal, L"Значение <b>№ слова</b> не должно быть отрицательным.", check::numword);
			else if (VAL(numWord)[i] > 32 && !sheet->arinc)
				result = WriteError(signal, L"Значение <b>№ слова</b> должно быть меньше 32.", check::numword);
			if (signal.data->numWord.sys != 8 && sheet->arinc)
				result = WriteError(signal, L"<b>Адрес сигнала</b> должен быть записан в 8-ой системе счисления.", check::numword);
		}
	}
	// Проверка мин, макс и цср
	if (!FL(min) && !FL(max) && !signal.data->csr.flag && !FL(bit))
	{
		int nBit = 0;
		(VAL(bit).size() == 4) ? nBit = (VAL(bit)[1] - VAL(bit)[0]) + (VAL(bit)[3] - VAL(bit)[2]) + 2 :
			nBit = (VAL(bit)[1] - VAL(bit)[0]) + 1;

		// Выделение бита под знак, если он присутствует
		if (signal.data->bitSign) 
			nBit--;

		double nMin = signal.data->csr.value;
		for (int i = 1; i <= nBit; i++)
			nMin = nMin / 2;

		double nMax = (signal.data->csr.value * 2) - nMin;

		if (VAL(max) - nMax >= 2)
			result = WriteError(signal, L"Нельзя упаковать <b>максимальное значение</b>", check::max);
		else if (((abs(VAL(min)) > (nMax + nMin)) || (abs(VAL(min)) < nMin)) && VAL(min) != 0)
			result = WriteError(signal, L"Нельзя упаковать <b>минимальное значение</b>", check::min);

		if ((VAL(min) < 0) && !signal.data->bitSign && !sheet->arinc)
			result = WriteError(signal, L"<b>Минимальное значение</b> не может быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\"", check::min);
		else if ((VAL(min) >= 0) && signal.data->bitSign && !sheet->arinc)
			result = WriteError(signal, L"<b>Минимальное значение</b> должно быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\"", check::min);
	}
	return result;
}

// Поиск повторений идентификатора на листе
bool CTest::TitleRepitTest(errorSignal& signal, const int& index) 
{
	bool result = false;
	#define TITLE(i, j) sheet->signals[i].title[j]
	CString title = TITLE(index, 1);
	std::set<CString>::iterator current, end;

	if (title.IsEmpty())
		return result;

	if (sheet->arinc)
	{
		current = exception.arinc.find(title);
		end = exception.arinc.end();
	}
	else
	{
		current = exception.mkio.find(title);
		end = exception.mkio.end();
	}

	// Если идентификатор не найден в множестве исключений
	if (current == end)
		for (size_t i = index + 1; i < sheet->signals.size() && !result; i++)
			if (TITLE(i, 1).Compare(title) == 0 && TITLE(i, 0).CompareNoCase(RESERVE_SIGNAL) != 0)
				result = WriteError(signal, L"Сигнал с таким <b>условным обозначением</b> присутствует на этом листе.", check::title);

	return result;
}

// Проверка используемых разрядов
bool CTest::BitsTest(errorSignal& signal) 
{
	#define FL(x) signal.data->x.flag
	#define VAL(x) signal.data->x.value
	
	bool result = false, bMaxVal = false;
	// Кол-во № слов должно совпадать с кол-вами интервалов исп. разрядов
	if (!FL(numWord) && !signal.data->bit.flag)
	{
		for (size_t i = 0; i < VAL(bit).size(); i += 2)
		{
			if (VAL(bit)[i] > VAL(bit)[i + 1] && VAL(bit)[i + 1] != -1)
				result = WriteError(signal, L"Старший бит меньше младшего в <b>используемых разрядах</b>.", check::bits);
			if (VAL(bit)[i] > MAX_BITS || VAL(bit)[i + 1] > MAX_BITS)
			{
				bMaxVal = true;
				CString msg = L"Значение бита превышает максимально допустимое в <b>используемых разрядах</b> ";
				msg.Format(L"%s(%d).", msg, MAX_BITS);
				result = WriteError(signal, msg, check::bits);
			}
		}

		if ((VAL(numWord).size() * 2 == VAL(bit).size()) && !bMaxVal)
		{
			#define RepSigByIndex(idx) repit[GetIndexRepiter(VAL(numWord)[idx])].signals
			bool bReplase = false;	// Взаимозаменяемые слова, которые кладутся в зависимости от чего-то там (отличаются обозначениями в наименовании после запятой)

			VAL(numWord).size() == 2 ? 
				bReplase = CheckReplace(signal.data->title[0], RepSigByIndex(1)) || CheckReplace(signal.data->title[0], RepSigByIndex(0)) :
				bReplase = CheckReplace(signal.data->title[0], RepSigByIndex(0));
			
			vector<int> bits = CrossBits(VAL(bit), VAL(numWord));
			if (bits.size() != 0 && !bReplase)
			{
				CString msg = L"Следующие <b>используемые разряды</b> перекрываются: ";
				for (size_t i = 0; i < bits.size(); i++)
					msg.Format(L"%s %d", msg, bits[i]);
				result = WriteError(signal, msg, check::bits);
			}
		}
		else 
			result = (signal.data->numWord.value.size() == 1) ? WriteError(signal, L"Должен быть один интервал <b>используемых разрядов</b>", check::bits) :
				WriteError(signal, L"Должно быть два интервала <b>используемых разрядов</b>",  check::bits);
	}
	return result;
}

// Проверка на перекрытие битов
vector<int> CTest::CrossBits(const vector <int>& bits, const vector <int>& numWord) 
{
	vector<int> result;
	for (size_t j = 0, i = 0; i < numWord.size(); j += 2, i++)
	{
		int end, start = bits[j];
		(bits[j + 1] == -1) ? end = start : end = bits[j + 1];

		for (; start <= end; start++) 
		{
			ASSERT(start <= 32);
			int indx = GetIndexRepiter(numWord[i]);
			ASSERT(indx != -1);
			repit[indx].bits[start] ? repit[indx].bits[start] = false :	result.push_back(start);	// отметка в матрице о наличии бита
		}
	}
	return result;
}

// Проверка слов, которые выборочно кладутся в одно слово
bool CTest::CheckReplace(CString first, const vector <signalData*> signals)
{
	bool result = false;
	if (sheet->arinc)
		return result;

	for (size_t i = 0; i < signals.size(); i++)
	{
		CString second = signals[i]->title[0];
		if (first.CompareNoCase(second) == 0)
			continue;

		int indx = first.ReverseFind(L',');

		if (indx == -1)
			continue;
		else
			first.Delete(indx, first.GetLength() - indx);
		
		indx = second.ReverseFind(L',');
		if (indx == -1)
			continue;
		else
			second.Delete(indx, second.GetLength() - indx);

		first.CompareNoCase(second) == 0 ? result = true : result = result;
	}

	return result;
}
#pragma endregion

#pragma region Repiter
// Инициализация репитера для проверки перекрытия
void CTest::InitRepiter()
{
	#define NUM_VAL(idx) sheet->signals[idx].numWord.value

	for (int i = 0; i < sheet->signals.size(); i++)
	{
		for (int j = 0; j < NUM_VAL(i).size(); j++)
		{
			int indx = GetIndexRepiter(NUM_VAL(i)[j]);
			indx == -1 ? AddRepiter(NUM_VAL(i)[j], i) : repit[indx].signals.push_back(&sheet->signals[i]);
		}
	}
}

// Добавление нового номера слова (адреса) в сетку
void CTest::AddRepiter(const int& value, const int& index)
{
	repiter result;
	result.adr = value;
	result.signals.push_back(&sheet->signals[index]);
	result.bits.resize(MAX_BITS);	// Выделение память под биты и номер слова
	for (int k = 0; k < MAX_BITS; k++)	// Инициализация сетки перекрытия
		result.bits[k] = true;

	repit.push_back(result);
}

// Получить индекс сетки битов по номеру слова (в случае неудачи возвр. индекс, иначе -1)
int CTest::GetIndexRepiter(const int& numeric)
{
	int index = -1;
	for (int i = 0; i < repit.size() && index == -1; i++)
		if (repit[i].adr == numeric)
			index = i;

	return index;
}

// Очистка репитера
void CTest::ClearRepiter()
{
	for (size_t i = 0; i < repit.size(); i++)
	{
		repit[i].signals.clear();
		repit[i].signals.shrink_to_fit();
		repit[i].bits.clear();
		repit[i].bits.shrink_to_fit();
	}
	repit.clear();
	repit.shrink_to_fit();
}
#pragma endregion

#pragma region Warning
// Поиск повторений идентификатора в книге
void CTest::FindRepiteTitleInBook(errorSignal& signal, const int& index)
{
	vector <CString> repitSheet;
	CString title = sheet->signals[index].title[1];

	for (size_t i = 0; i < book->sheets.size(); i++)
	{
		int start;
		book->sheets[i].name.Compare(sheet->name) == 0 ? start = index + 1 : start = 0;
		for (size_t j = start; j < book->sheets[i].signals.size(); j++)
		{
			if (book->sheets[i].signals[j].title[1].Compare(title) == 0 && book->sheets[i].signals[j].title[0].CompareNoCase(RESERVE_SIGNAL) == -1 && !title.IsEmpty())
			{
				book->sheets[i].signals[j].repWord = true;
				repitSheet.push_back(book->sheets[i].name);
				continue;
			}
		}
	}
	if (repitSheet.size() != 0)
	{
		CString msg = L"Сигнал с таким <b>условным обозначением</b> встречается на следующих листах в книге:\n";
		for (size_t i = 0; i < repitSheet.size(); i++)
			msg += repitSheet[i] + L", ";
		int posDot = msg.ReverseFind(L',');
		msg.Delete(posDot);
		WriteError(signal, msg, check::title);
	}
}
#pragma endregion