#include "stdafx.h"
#include "Test.h"

// Конструктор
CTest::CTest() 
{	
	// Множество исключений
	// Для ARINC
	exception.insert(L"ID_BPV1");	// Идентификатор блока 1
	exception.insert(L"ID_BPV2");	// Идентификатор блока 2
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
				errorSignal tmp;
				tmp.signal = &sheet->signals[i];

				// Проход по синтаксическим ошибкам
				syntaxValue(sheet->signals[i], tmp.error) ? error = true : error = error;
				syntaxTitle(sheet->signals[i].title, tmp.error) ? error = true : error = error;
				syntaxBits(sheet->signals[i].bit, tmp.error) ? error = true : error = error;

				if (!tmp.error.empty())
					syntax.push_back(tmp);
				tmp.error.clear();

				// Проход по семантическим ошибкам
				simanticValue(sheet->signals[i], tmp.error, repit) ? error = true : error = error;
				findRepiteTitleInSheet((int)i, tmp.error) ? error = true : error = error;
				CString prevTitle;
				(i > 0) ? prevTitle = sheet->signals[i - 1].title[0] : prevTitle = L"";
				simanticBits(sheet->signals[i], prevTitle, repit, tmp.error) ? error = true : error = error;

				if (!tmp.error.empty())
					simantic.push_back(tmp);
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

			findRepiteTitleInBook((int)i, tmp.error);

			if (!tmp.error.empty())
				warning.push_back(tmp);
		}
	}
}

#pragma region Syntax
// Проверка номера набора параметров
bool CTest::checkNP(vector <errorSignal>& error) 
{
	bool result = false;	// true -  есть ошибка, false - нет ошибки
	errorSignal tmp;
	tmp.signal = &sheet->signals[0];
	tmp.error.push_back(errRemarks[6]);

	if (sheet->np == 0)
		tmp.error.push_back(L"Отсутствует значение набора параметров.");
	else
		if (!tmp.signal->min.flag && !tmp.signal->max.flag)
			if (tmp.signal->min.value > sheet->np || sheet->np > tmp.signal->max.value)
				tmp.error.push_back(L"Набор параметров не соответствует указанному интервалу значений.");

	// Добавление ошибки в массив
	if (tmp.error.size() > 1) 
	{
		result = true;
		error.push_back(tmp);
	}
	return result;
}

// Проверка всех числовых параметров
bool CTest::syntaxValue(const signalData& signal, vector <CString>& error) 
{
	bool result; // true -  есть ошибка, false - нет ошибки
	
	stepSyntaxValue(signal.numWord.flag, 0, error);
	stepSyntaxValue(signal.min.flag, 2, error);
	stepSyntaxValue(signal.max.flag, 3, error);
	stepSyntaxValue(signal.csr.flag, 4, error);

	(signal.numWord.flag || signal.min.flag || signal.max.flag || signal.csr.flag) ? result = true : result = false;
	return result;
}

// Доп функция для проверки числовых параметров
void CTest::stepSyntaxValue(const bool& flag, const int& indx, vector <CString>& error) 
{
	if (flag) 
	{
		error.push_back(errRemarks[indx]);
		error.push_back(L"Поле пустое или содержит недопустимые символы.");
	}
}

// Проверка синтаксиса идентификатора
bool CTest::syntaxTitle(const vector <CString>& title, vector <CString>& error) 
{
	regex correct("^[A-Za-z][A-Za-z0-9_]*$");
	string tmp = CT2A(title[1]);
	bool result = regex_match(tmp, correct);

	if (!result) 
	{
		error.push_back(errRemarks[1]);
		bool bFind = false;	// Найдена ли ошибка из набора регулярных выражений
		vector <string> incorrect = { "^[ \t\n]*$", "[ \t\n]+", "^[^A-Za-z][A-Za-z0-9_]*$", "([A-Za-z0-9_]*[А-Яа-я]+[A-Za-z0-9_]*)+", "_$" };
		vector <CString> description = { L"Значение в поле отсутствует. (Возможно сигнал зарезервирован, но \"Резерв\" не написано)",
			L"Значение в поле содержит пробел.",
			L"Значение в поле начинается не с латинской буквы.",
			L"Значение в поле содержит кириллицу.",
			L"Значение в поле заканчивается на '_'." };
		// Поиск ошибки (обход по набору регулярок)
		for (size_t i = 0; i < incorrect.size(); i++)
		{
			regex reg(incorrect[i]);
			if (regex_search(tmp, reg)) 
			{
				error.push_back(description[i]);
				bFind = true;
			}
		}
		if (!bFind)	// Неопознанная ошибка
			throw UndefinedError(book->name, sheet->name, title[0] + L"\n\t" + title[1]);
	}
	return !result;
}

// Проверка используемых разрядов
bool CTest::syntaxBits(const intData& bits, vector <CString>& error) 
{
	regex correct("^[0-9]+((…|[.]{3})[0-9]+)?(,[ \t]?[0-9]+((…|[.]{3})[0-9]+)?)?$");
	string tmp = CT2A(bits.field);
	bool result = regex_match(tmp, correct);

	if (!result)
	{
		error.push_back(errRemarks[5]);
		bool bFind = false;	// Найдена ли ошибка из набора регулярных выражений
		vector <string> incorrect = { "^[ \t\n]*$", "[^0-9., \t…]+", "[0-9.…]*[ \t]+[0-9.…]*", "[0-9]*[.]{1,2}[0-9]*",
			"^(…|[.]{3})[0-9]+" , "[0-9]+(…|[.]{3})$", "(,[ \t]?)+[0-9.…]*(,[ \t]?)+", "^[0-9]*[.]{1,2}[0-9]*,[ \t]?",
			"^(…|[.]{3})[0-9]+,[ \t]?", "^[0-9]+(…|[.]{3}),[ \t]?",  ",[ \t]?([0-9]*[.]{1,2}[0-9]*)$"",[ \t]?([0-9]*[.]{1,2}[0-9]*)$",
			",[ \t]?(…|[.]{3})[0-9]+$", ",[ \t]?[0-9]+(…|[.]{3})$", "[0-9]*[.]{1,2}[0-9]*,[ \t]?[0-9]*[.]{1,2}[0-9]*" };
		vector <CString> description = { L"Значение в поле отсутствует.", L"Поле содержит недопустимые символы.", L"Поле содержит лишние пробелы. Допускается только после запятой.",
			L"Неверное обозначение промежутка.", L"Отсутствует значение в начале промежутка.", L"Отсутствует значение в конце промежутка.",
			L"Значение в поле содержит более одной запятой.", L"Неверное обозначение промежутка до запятой.", L"Отсутствует значение в начале промежутка до запятой.",
			L"Отсутствует значение в конце промежутка до запятой.", L"Неверное обозначение промежутка после запятой.",  L"Отсутствует значение в начале промежутка после запятой.",
			L"Отсутствует значение в конце промежутка после запятой.", L"Неверное обозначение обоих промежутков." };
		// Поиск ошибки (обход по набору регулярок)
		for (size_t i = 0; i < incorrect.size(); i++) 
		{
			regex reg(incorrect[i]);
			if (regex_search(tmp, reg)) 
			{
				if (i == 3 || i == 7 || i == 10 || i == 13)	// Ошибки, связанные с промежутками (чтобы можно было генерировать в таком случае скрипты)
					result = true;
				error.push_back(description[i]);
				bFind = true;
			}
		}
		if (!bFind)	// Неопознанная ошибка
			throw UndefinedError(book->name, sheet->name, bits.field);
	}
	return !result;
}
#pragma endregion

#pragma region Simantic
// Проверка всех числовых параметров
bool CTest::simanticValue(const signalData& signal, vector <CString>& error, vector <repiter>& repit) 
{
	bool result = false;
	// Проверка номера слова (адреса)
	if (!signal.numWord.flag)
	{
		vector <CString> tmp;
		tmp.push_back(errRemarks[0]);
		for (size_t i = 0; i < signal.numWord.value.size(); i++) 
		{
			// Повторение на листе номера слова или адреса!
			/*int indx = isContain(repit, signal.numWord.value[i]);
			CString str;
			sheet->arinc ? str = L"адреса" : str = L"номера слова";
			str.Format(L"Значение %s %d уже содержится на этом листе.", str,signal.numWord.value[i]);
			repit[indx].bits[0] ? repit[indx].bits[0] = false : tmp.push_back(str);
			*/
			if (signal.numWord.value[i] > 32 && !sheet->arinc)	// Слово должно быть не больше 32
				tmp.push_back(L"Значение номера слова должно быть меньше 32.");

			if (signal.numWord.sys != 8 && sheet->arinc)	// Адрес должен быть в 8-ной системе счисления
				tmp.push_back(L"Адрес сигнала должен быть записан в 8-ой системе счисления.");
		}

		if (tmp.size() > 1) 
		{
			result = true;
			for (size_t i = 0; i < tmp.size(); i++)
				error.push_back(tmp[i]);
		}
	}
	// Проверка мин, макс и цср
	if (!signal.min.flag && !signal.max.flag && !signal.csr.flag && !signal.bit.flag) 
	{
		int nBit = 0;
		(signal.bit.value.size() == 4) ? nBit = (signal.bit.value[1] - signal.bit.value[0]) + (signal.bit.value[3] - signal.bit.value[2]) + 2 :
			nBit = (signal.bit.value[1] - signal.bit.value[0]) + 1;

		// Выделение бита под знак, если он присутствует
		if (signal.bitSign) nBit--;

		double nMin = signal.csr.value;
		for (int i = 1; i <= nBit; i++)
			nMin = nMin / 2;

		double nMax = (signal.csr.value * 2) - nMin;

		if (signal.max.value - nMax >= 2) 
		{
			result = true;
			error.push_back(errRemarks[3]);
			error.push_back(L"Нельзя упаковать данное значение");
		}
		else if (((abs(signal.min.value) > (nMax + nMin)) || (abs(signal.min.value) < nMin)) && signal.min.value != 0) 
		{
			result = true;
			error.push_back(errRemarks[2]);
			error.push_back(L"Нельзя упаковать данное значение");
		}
		if ((signal.min.value < 0) && !signal.bitSign && !sheet->arinc) 
		{
			result = true;
			error.push_back(errRemarks[2]);
			error.push_back(L"Не может быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\"");
		}
		else if ((signal.min.value >= 0) && signal.bitSign && !sheet->arinc) 
		{
			result = true;
			error.push_back(errRemarks[2]);
			error.push_back(L"Должно быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\"");
		}
	}
	return result;
}

// Поиск повторений идентификатора на листе
bool CTest::findRepiteTitleInSheet(const int& index, vector <CString>& error) 
{
	bool result = false;
	CString title = sheet->signals[index].title[1];
	set<CString>::iterator it = exception.find(title);
	if (it == exception.end())	// Если идентификатор не найден в множестве исключений
		for (size_t i = index + 1; i < sheet->signals.size(); i++)
			if (sheet->signals[i].title[1].Compare(title) == 0 && sheet->signals[i].title[0].CompareNoCase(RESERVE_SIGNAL) != 0)
				result = true;

	if (result) 
	{
		error.push_back(errRemarks[1]);
		error.push_back(L"Сигнал с таким идентификатором присутствует на этом листе.");
	}
	return result;
}

// Поиск повторений идентификатора в книге
void CTest::findRepiteTitleInBook(const int& index, vector <CString>& error) 
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
		error.push_back(errRemarks[1]);
		for (size_t i = 0; i < repitSheet.size(); i++)
			msg += repitSheet[i] + L", ";
		int posDot = msg.ReverseFind(L',');
		msg.Delete(posDot);
		error.push_back(msg);
	}
}

// Проверка используемых разрядов
bool CTest::simanticBits(const signalData& signal, const CString& prevTitle, vector<repiter>& repit, vector <CString>& error) 
{
	bool result = false;
	// Кол-во № слов должно совпадать с кол-вами интервалов исп. разрядов
	if (!signal.numWord.flag && !signal.bit.flag) 
	{
		for (size_t i = 0; i < signal.bit.value.size(); i+=2)
			if (signal.bit.value[i] > signal.bit.value[i + 1] && signal.bit.value[i+1] != -1) 
			{
				result = true;
				error.push_back(errRemarks[5]);
				error.push_back(L"Старший бит меньше младшего.");
			}
		if (signal.numWord.value.size() * 2 == signal.bit.value.size()) 
		{
			vector<int> bits = checkCrossBits(signal.bit.value, signal.numWord.value, repit);
			if (bits.size() != 0 && !checkSameTitle(signal.title[0], prevTitle)) 
			{
				result = true;
				CString tmp = L"Следующий(ие) бит(ы) перекрывает(ют)ся: ";
				for (size_t i = 0; i < bits.size(); i++)
					tmp.Format(L"%s %d", tmp, bits[i]);
				error.push_back(errRemarks[5]);
				error.push_back(tmp);
			}
		}
		else 
		{
			result = true;
			error.push_back(errRemarks[5]);
			(signal.numWord.value.size() == 1) ? error.push_back(L"Должен быть один интервал.") : error.push_back(L"Должно быть два интервала.");
		}
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