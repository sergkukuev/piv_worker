#include "stdafx.h"
#include "Test.h"

// Конструктор
CTest::CTest() 
{	
	// Множество исключений
	// Для ARINC
	exception.insert(L"ID_BPV1");	// Идентификатор блока 1
	exception.insert(L"ID_BPV2");	// Идентификатор блока 2

	// Создание базы для проверки на ошибки с помощью регулярных выражений
	base.resize(SIZE_BASE);
	base[NUMWORD_BASE].correct = "^[0-9]+(, ?[0-9]+)?$";
	base[NUMWORD_BASE].incorrect = {
		{ "^[ \t\n]*$", L"Значение № слова отсутствует." },
		{ "[^0-9 \t\n,]", L"Значение № слова содержит недопустимые символы." },
		{ "[^ \t\n,]+[ \t\n]", L"Значение № слова содержит лишние пробелы. (Допускается только после запятой)" },
		{ ",[^,]*,+", L"Значение № слова содержит больше одной запятой." },
		{ "^[ \t\n]*,", L"Значение № слова до запятой отсутствует." },
		{ ",[ \t\n]*$", L"Значение № слова после запятой отсутствует." }
	};

	base[TITLE_BASE].correct = "^[A-Za-z][A-Za-z0-9_]*$";
	base[TITLE_BASE].incorrect = {
		{ "^$", L"Условное обозначение сигнала отсутствует. (Возможно сигнал зарезервирован, но \"Резерв\" не написано)" },
		{ "[ \t\n]", L"Условное обозначение сигнала содержит пробел." },
		{ "^[^A-Za-z]", L"Условное обозначение сигнала начинается не с латинской буквы." },
		{ "[А-Яа-я]", L"Условное обозначение сигнала содержит кириллицу." },
		{ "_$", L"Условное обозначение сигнала заканчивается на недопустимый символ '_'." }
	};

	base[VALUE_BASE].correct = "^-?[0-9]+(,[0-9]+)?$";
	base[VALUE_BASE].incorrect = {
		{ "^[ \t\n]$", L"Минимальное значение отсутствует." },
		{ "[^0-9,.- \t\n]", L"Минимальное значение содержит недопустимые символы." },
		{ "[ \t\n]", L"Минимальное значение содержит пробелы." },
		{ ",|.[^,.]*(.|,)+", L"Минимальное значение содержит более одного разделителя." },
		{ "-[^-]*-+", L"Минимальное значение содержит больше одного знака '-'." },
		{ "^[^- \t\n]+-", L"Минимальное значение содержит знак '-' в недопустимом месте. (Допускается только перед значением)" },
		{ "^[ \t\n]*.|,", L"Минимальное значение не содержит значения до разделителя" },
		{ ".|,[ \t\n]$", L"Минимальное значение не содержит значения после разделителя" }
	};

	base[BITS_BASE].correct = "^[0-9]+((…|[.]{3})[0-9]+)?(, ?[0-9]+((…|[.]{3})[0-9]+)?)?$";
	base[BITS_BASE].incorrect = {
		{ "^[ \t\n]$", L"Значение используемых разрядов отсутствует." },
		{ "[^0-9,.… \t\n]", L"Значение используемых разрядов содержит недопустимые символы." },
		{ "[^ \t\n,]+[ \t\n]", L"Значение используемых разрядов содержит лишние пробелы. (Допускается только после запятой)" },
		{ "^[^….]*..?[^….,]*,", L"Значение используемых разрядов содержит неверное обозначение промежутка." },
		{ "(…|.{1,3})[^….]*(…|.{1,3})[^….]*(…|.{1,3})", L"Значение используемых разрядов содержит более двух промежутков." },
		//{ "^|,[ \t\n]*(…|[.]+)", L"Значение используемых разрядов не содержит значения в начале одного из промежутков." },
		//{ "(…|[.]+)[ \t\n]*,|$", L"Значение используемых разрядов не содержит значения в конце одного из промежутков." }
	};
}

// Деструктор
CTest::~CTest() {	}

// Проверки на ошибки одного протокола
errorSet CTest::Start(bookData& book) 
{
	errorSet result;
	result.book = &book;
	this->book = &book;
	for (size_t j = 0; j < book.sheets.size(); j++) 
	{
		errorSheet tmp;
		sheet = &book.sheets[j];
		tmp.sheet = &book.sheets[j];
		getErrors(tmp.syntax, tmp.simantic);
		getWarnings(tmp.warning);
		result.set.push_back(tmp);
	}
	return result;
}

// Проверка на все ошибки
list <errorSet> CTest::Start(list <bookData>& books) 
{
	list <errorSet> errors;
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++) 
	{
		errorSet error;
		error.book = &*it;
		book = &*it;
		for (size_t j = 0; j < it->sheets.size(); j++) 
		{
			errorSheet tmp;
			sheet = &it->sheets[j];
			tmp.sheet = &it->sheets[j];
			getErrors(tmp.syntax, tmp.simantic);
			getWarnings(tmp.warning);
			error.set.push_back(tmp);
		}
		errors.push_back(error);
	}
	return errors;
}

// Проверка на синтаксические и семантические ошибки
void CTest::getErrors(vector <errorSignal>& syntax, vector <errorSignal>& simantic) 
{
		bool error = false;	// true -  есть ошибка на листе, false - нет ошибки на листе
		if (!sheet->arinc)	// Если линия передачи не arinc, то необходимо проверять набор параметра 
			checkNP(syntax) ? error = true : error = error;

		vector <repiter> repit;		// Структура для проверки повторений
		initRepiter(repit);	// Ее инициализация

		for (size_t i = 0; i < sheet->signals.size(); i++) 
		{
			if (sheet->signals[i].title[0].Find(RESERVE_SIGNAL) == -1) 
			{
				errorSignal tmpSyntax;
				tmpSyntax.signal = &sheet->signals[i];

				// Проход по синтаксическим ошибкам
				!sheet->arinc ? syntaxTemplate(sheet->signals[i].numWord.field, NUMWORD_CHECK, base[NUMWORD_BASE], tmpSyntax) ? error = true : error = error :
					syntaxValue(tmpSyntax) ? error = true : error = error;
				syntaxTemplate(sheet->signals[i].title[1], TITLE_CHECK, base[TITLE_BASE], tmpSyntax) ? error = true : error = error;
			
				syntaxTemplate(sheet->signals[i].min.field, MIN_CHECK, base[VALUE_BASE], tmpSyntax) ? error = true : error = error;
				syntaxTemplate(sheet->signals[i].max.field, MAX_CHECK, base[VALUE_BASE], tmpSyntax) ? error = true : error = error;
				syntaxTemplate(sheet->signals[i].csr.field, CSR_CHECK, base[VALUE_BASE], tmpSyntax) ? error = true : error = error;
				syntaxTemplate(sheet->signals[i].bit.field, BITS_CHECK, base[BITS_BASE], tmpSyntax) ? error = true : error = error;

				if (!tmpSyntax.error.empty())
					syntax.push_back(tmpSyntax);

				errorSignal tmpSimantic;
				tmpSimantic.signal = &sheet->signals[i];

				// Проход по семантическим ошибкам
				simanticValue(tmpSimantic, repit) ? error = true : error = error;
				findRepiteTitleInSheet(tmpSimantic, (int)i) ? error = true : error = error;
				CString prevTitle;
				(i > 0) ? prevTitle = sheet->signals[i - 1].title[0] : prevTitle = L"";
				simanticBits(tmpSimantic, prevTitle, repit) ? error = true : error = error;

				if (!tmpSimantic.error.empty())
					simantic.push_back(tmpSimantic);
			}
		}

		sheet->error = error;
}

// Проверка на незначительные ошибки (замечания) 
void CTest::getWarnings(vector <errorSignal>& warning) 
{
	for (size_t i = 0; i < sheet->signals.size(); i++) 
	{
		if (sheet->signals[i].title[0].Find(RESERVE_SIGNAL) == -1) 
		{
			errorSignal tmp;
			tmp.signal = &sheet->signals[i];

			findRepiteTitleInBook(tmp, (int)i);

			if (!tmp.error.empty())
				warning.push_back(tmp);
		}
	}
}

// Запись ошибки
bool CTest::writeError(errorSignal& set, const CString& msg, const int& index) {
	set.error.push_back(msg);
	set.check[index] = true;
	return true;
}

#pragma region Syntax
// Проверка номера набора параметров
bool CTest::checkNP(vector <errorSignal>& error) 
{
	bool result = false;	// true -  есть ошибка, false - нет ошибки
	errorSignal tmp;
	tmp.signal = &sheet->signals[0];

	if (sheet->np == 0)
		result = writeError(tmp, L"Отсутствует значение набора параметров.", COMMENT_CHECK);
	else
		if (!tmp.signal->min.flag && !tmp.signal->max.flag)
			if (tmp.signal->min.value > sheet->np || sheet->np > tmp.signal->max.value)
				result = writeError(tmp, L"Набор параметров не соответствует указанному интервалу значений.", COMMENT_CHECK);

	// Добавление ошибки в массив
	if (!tmp.error.empty()) 
		error.push_back(tmp);

	return result;
}

// Проверка всех числовых параметров
bool CTest::syntaxValue(errorSignal& set) 
{
	bool result; // true -  есть ошибка, false - нет ошибки
	
	if (set.signal->numWord.flag)
		writeError(set, L"Поле пустое или содержит недопустимые символы.", NUMWORD_CHECK);
	/*if (set.signal->min.flag)
		writeError(set, L"Поле пустое или содержит недопустимые символы.", MIN_CHECK);
	if (set.signal->max.flag)
		writeError(set, L"Поле пустое или содержит недопустимые символы.", MAX_CHECK);
	if (set.signal->csr.flag)
		writeError(set, L"Поле пустое или содержит недопустимые символы.", CSR_CHECK);
		*/
	(set.signal->numWord.flag || set.signal->min.flag || set.signal->max.flag || set.signal->csr.flag) ? result = true : result = false;
	return result;
}

// Синтаксическая проверка шаблонным методом
bool CTest::syntaxTemplate(const CString& field, const int& check, const regexBase& test, errorSignal& set) 
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
				result = writeError(set, test.incorrect[i].desc, check);
				bFind = true;
			}
		}
		if (!bFind)	// Неопознанная ошибка
			throw UndefinedError(book->name, sheet->name, set.signal->numWord.field + L"\n\t" + set.signal->title[1]);
	}
	return result;
}
#pragma endregion

#pragma region Simantic
// Проверка всех числовых параметров
bool CTest::simanticValue(errorSignal& set, vector <repiter>& repit) 
{
	bool result = false;
	// Проверка номера слова (адреса)
	if (!set.signal->numWord.flag)
	{
		for (size_t i = 0; i < set.signal->numWord.value.size(); i++) 
		{
			// Повторение на листе номера слова или адреса!
			/*int indx = isContain(repit, signal.numWord.value[i]);
			CString str;
			sheet->arinc ? str = L"адреса" : str = L"номера слова";
			str.Format(L"Значение %s %d уже содержится на этом листе.", str,signal.numWord.value[i]);
			repit[indx].bits[0] ? repit[indx].bits[0] = false : set.push_back(str);
			*/
			if (set.signal->numWord.value[i] > 32 && !sheet->arinc)	// Слово должно быть не больше 32
				result = writeError(set, L"Значение номера слова должно быть меньше 32.", NUMWORD_CHECK);

			if (set.signal->numWord.sys != 8 && sheet->arinc)	// Адрес должен быть в 8-ной системе счисления
				result = writeError(set, L"Адрес сигнала должен быть записан в 8-ой системе счисления.", NUMWORD_CHECK);
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
			result = writeError(set, L"Нельзя упаковать данное значение", MAX_CHECK);
		else if (((abs(set.signal->min.value) > (nMax + nMin)) || (abs(set.signal->min.value) < nMin)) && set.signal->min.value != 0) 
			result = writeError(set, L"Нельзя упаковать данное значение", MIN_CHECK);

		if ((set.signal->min.value < 0) && !set.signal->bitSign && !sheet->arinc)
			result = writeError(set, L"Не может быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\"", MIN_CHECK);
		else if ((set.signal->min.value >= 0) && set.signal->bitSign && !sheet->arinc)
			result = writeError(set, L"Должно быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\"", MIN_CHECK);
	}
	return result;
}

// Поиск повторений идентификатора на листе
bool CTest::findRepiteTitleInSheet(errorSignal& set, const int& index) 
{
	bool result = false;
	CString title = sheet->signals[index].title[1];
	std::set<CString>::iterator it = exception.find(title);
	if (it == exception.end())	// Если идентификатор не найден в множестве исключений
		for (size_t i = index + 1; i < sheet->signals.size(); i++)
			if (sheet->signals[i].title[1].Compare(title) == 0 && sheet->signals[i].title[0].CompareNoCase(RESERVE_SIGNAL) != 0)
				result = true;

	if (result)
		writeError(set, L"Сигнал с таким идентификатором присутствует на этом листе.", TITLE_CHECK);

	return result;
}

// Поиск повторений идентификатора в книге
void CTest::findRepiteTitleInBook(errorSignal& set, const int& index) 
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
		writeError(set, msg, TITLE_CHECK);
	}
}

// Проверка используемых разрядов
bool CTest::simanticBits(errorSignal& set, const CString& prevTitle, vector<repiter>& repit) 
{
	bool result = false;
	// Кол-во № слов должно совпадать с кол-вами интервалов исп. разрядов
	if (!set.signal->numWord.flag && !set.signal->bit.flag) 
	{
		for (size_t i = 0; i < set.signal->bit.value.size(); i+=2)
			if (set.signal->bit.value[i] > set.signal->bit.value[i + 1] && set.signal->bit.value[i+1] != -1) 
				result = writeError(set, L"Старший бит меньше младшего.", BITS_CHECK);
		if (set.signal->numWord.value.size() * 2 == set.signal->bit.value.size()) 
		{
			vector<int> bits = checkCrossBits(set.signal->bit.value, set.signal->numWord.value, repit);
			if (bits.size() != 0 && !checkSameTitle(set.signal->title[0], prevTitle)) 
			{
				CString str = L"Следующий(ие) бит(ы) перекрывает(ют)ся: ";
				for (size_t i = 0; i < bits.size(); i++)
					str.Format(L"%s %d", str, bits[i]);
				result = writeError(set, str, BITS_CHECK);
			}
		}
		else 
			result = (set.signal->numWord.value.size() == 1) ? writeError(set, L"Должен быть один интервал.", BITS_CHECK) : writeError(set, L"Должно быть два интервала.",  BITS_CHECK);
	}
	return result;
}

// Проверка на перекрытие битов
vector<int> CTest::checkCrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit) 
{
	vector<int> result;
	for (size_t j = 0; j < 2; j += 2)
		for (size_t i = 0; i < numWord.size(); i++) 
		{
			int end, start = bits[j];
			(bits[j + 1] == -1) ? end = start : end = bits[j + 1];

			for (; start <= end; start++) 
			{
				int indx = isContain(repit, numWord[i]);
				// отметка в матрице о наличии бита
				repit[indx].bits[start] ? repit[indx].bits[start] = false :	result.push_back(start);
			}
		}
	return result;
}

// Проверка двух соседних наименований на совпадение
bool CTest::checkSameTitle(const CString& next, const CString& prev)
{
	CString first = next, second = prev;

	int indx = first.ReverseFind(L',');
	if (indx != -1)
		first.Delete(indx, first.GetLength() - indx);
	indx = second.ReverseFind(L',');
	if (indx != -1)
		second.Delete(indx, second.GetLength() - indx);

	bool result;
	first.CompareNoCase(second) == 0 ? result = true : result = false;
	return result;
}
#pragma endregion

// Инициализация репитера для проверки перекрытия
void CTest::initRepiter(vector <repiter>& repit)
{
	for (int i = 0; i < sheet->signals.size(); i++) 
	{
		for (int j = 0; j < sheet->signals[i].numWord.value.size(); j++) 
		{
			if (isContain(repit, sheet->signals[i].numWord.value[j]) == -1) 
			{
				repiter tmp;
				tmp.adr = sheet->signals[i].numWord.value[j];
				tmp.bits = new bool[MAX_BITS + 1];	// Выделение память под биты и номер слова
				for (int k = 0; k < MAX_BITS + 1; k++)	// Инициализация массива перекрытия
					tmp.bits[k] = true;
				repit.push_back(tmp);
			}
		}
	}
}

// Имеется ли уже такой номер слова (адрес) (в случае удачи возвр. индекс, иначе -1)
int CTest::isContain(const vector<repiter>& repit, const int& numeric) 
{
	int result = -1;
	for (int i = 0; i < repit.size(); i++)
		if (repit[i].adr == numeric)
			result = i;
	return result;
}