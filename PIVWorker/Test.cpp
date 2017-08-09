#include "stdafx.h"
#include "Test.h"

// Конструктор
CTest::CTest() {	}

// Деструктор
CTest::~CTest() {	}

// Проверки на ошибки одного протокола
errorSet CTest::Start(bookData& book) {
	errorSet result;
	result.book = &book;
	tmpBook = &book;
	for (size_t j = 0; j < book.sheets.size(); j++) {
		errorSheet tmp;
		tmpSheet = &book.sheets[j];
		tmp.sheet = &book.sheets[j];
		getErrors(&book.sheets[j], tmp.syntax, tmp.simantic);
		//getWarnings(&book.sheets[j], tmp.warning);
		result.set.push_back(tmp);
	}
	return result;
}

// Проверка на все ошибки
list <errorSet> CTest::Start(list <bookData>& books) {
	list <errorSet> errors;
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++) {
		errorSet error;
		error.book = &*it;
		tmpBook = &*it;
		for (size_t j = 0; j < it->sheets.size(); j++) {
			errorSheet tmp;
			tmpSheet = &it->sheets[j];
			tmp.sheet = &it->sheets[j];
			getErrors(&it->sheets[j], tmp.syntax, tmp.simantic);
			//getWarnings(&it->sheets[j], tmp.warning);
			error.set.push_back(tmp);
		}
		errors.push_back(error);
	}
	return errors;
}

// Проверка на ошибки
void CTest::getErrors(sheetData* sheet, vector <errorSignal>& syntax, vector <errorSignal>& simantic) {
	try {
		if (!sheet->arinc)
			checkNP(sheet->signals[0], sheet->np, syntax);	// Проверка номера набора для мкио

		vector <repiter> repit;		// Структура для проверки повторений
		initRepiter(repit);	// Ее инициализация

		for (size_t i = 0; i < sheet->signals.size(); i++) {
			// Проверка на резервность
			if (sheet->signals[i].title[0].CompareNoCase(RESERVE_SIGNAL) != 0) {
				errorSignal tmp;
				bool bTitle = true, result = true;
				tmp.signal = &sheet->signals[i];

				// Проход по синтаксическим ошибкам
				syntaxValue(sheet->signals[i], sheet->arinc, tmp.error) ? result = result : result = false;
				bTitle = syntaxTitle(sheet->signals[i].title, tmp.error) ? result = result : result = false;
				syntaxBits(sheet->signals[i].bit, tmp.error) ? result = result : result = false;
				
				if (!tmp.error.empty())
					syntax.push_back(tmp);
				tmp.error.clear();

				// Проход по семантическим ошибкам
				simanticNumWord(sheet->signals[i].numWord, sheet->arinc, tmp.error) ? result = result : result = false;
				simanticValue(sheet->signals[i], tmp.error) ? result = result : result = false;
				simanticTitle((int)i, sheet->signals[i].title[1], bTitle, tmp.error) ? result = result : result = false;
				CString prevTitle;
				(i > 0) ? prevTitle = sheet->signals[i - 1].title[0] : prevTitle = L"";
				simanticBits(sheet->signals[i], prevTitle, repit, tmp.error) ? result = result : result = false;

				if (!tmp.error.empty())
					simantic.push_back(tmp);

				!result ? sheet->error = true : sheet->error = sheet->error;
			}
		}
	}
	catch (UndefinedError& exc) {
		exc.SetName(sheet->name);
		throw exc;
	}
}

// Проверка на замечания
void CTest::getWarnings(sheetData* sheet, vector <errorSignal>& warning) {
	for (size_t i = 1; i < sheet->signals.size(); i++) {
		if (sheet->signals[i].title[0].CompareNoCase(RESERVE_SIGNAL) != 0) { // Проверка на резервность
			errorSignal tmp;
			tmp.signal = &sheet->signals[i];
			//titleInBook();
			warning.push_back(tmp);
		}
	}
}

#pragma region Syntax
// Проверка номера набора
void CTest::checkNP(signalData& signal, const int& np, vector <errorSignal>& syntax) {
	errorSignal tmp;
	tmp.signal = &signal;
	tmp.error.push_back(errRemarks[6]);

	if (np == 0)
		tmp.error.push_back(L"Отсутствует номер набора параметров");
	else {
		// Минимально и максимальное значение должно соответствовать номер набора
		if (!signal.min.flag && !signal.max.flag)
			if (signal.min.value > np || np > signal.max.value)
				tmp.error.push_back(L"Набор параметров не соответствует значению в примечании");
	}

	if (tmp.error.size() > 1)
		syntax.push_back(tmp);
}

// Проверка числовых параметров 
bool CTest::syntaxValue(const signalData& signal, const bool& arinc, vector <CString>& error) {
	bool result;
	arinc ? checkValueByFlag(0, signal.numWord.flag, error) : checkValueByFlag(0, signal.numWord.flag, error);
	checkValueByFlag(2, signal.min.flag, error);
	checkValueByFlag(3, signal.max.flag, error);
	checkValueByFlag(4, signal.csr.flag, error);
	//checkValueByFlag(L"битов", 5, signal.bit.flag, error);
	(signal.numWord.flag || signal.min.flag || signal.max.flag || signal.csr.flag) ? result = false : result = true;
	return result;
}

// Проверка числовых параметров по набору флагов
void CTest::checkValueByFlag(const int& indx, const bool& flag, vector <CString>& error) {
	if (flag) {
		error.push_back(errRemarks[indx]);
		error.push_back(L"Поле пустое или содержит недопустимые символы");
	}
}

// Проверка синтаксиса идентификатора
bool CTest::syntaxTitle(const vector <CString>& title, vector <CString>& error) {
	regex correct("^[A-Za-z][A-Za-z0-9_]*$");
	string tmp = CT2A(title[1]);
	bool result = regex_match(tmp, correct);

	if (!result) {
		error.push_back(errRemarks[1]);
		bool bFind = false;	// Найдена ли ошибка из набора регулярных выражений
		vector <string> incorrect = { "^[ \t\n]*$", "[ \t\n]+", "^[^A-Za-z][A-Za-z0-9_]*$", "([A-Za-z0-9_]*[А-Яа-я]+[A-Za-z0-9_]*)+", "_$" };
		vector <CString> description = { L"Значение в поле отсутствует. (Возможно сигнал зарезервирован, но \"Резерв\" не написано)",
			L"Значение в поле содержит пробел",
			L"Значение в поле начинается не с латинской буквы",
			L"Значение в поле содержит кириллицу",
			L"Значение в поле заканчивается на '_'" };
		// Поиск ошибки (обход по набору регулярок)
		for (size_t i = 0; i < incorrect.size(); i++) {
			regex reg(incorrect[i]);
			if (regex_search(tmp, reg)) {
				error.push_back(description[i]);
				bFind = true;
			}
		}
		if (!bFind) {	// Неопознанная ошибка
			UndefinedError exc;
			exc.SetParam(title[0] + L';' + title[1]);
			throw exc;
		}
	}
	return result;
}

// Проверка битов
bool CTest::syntaxBits(const intData& bits, vector <CString>& error) {
	regex correct("^[0-9]+((…|[.]{3})[0-9]+)?(,[ \t]?[0-9]+((…|[.]{3})[0-9]+)?)?$");
	string tmp = CT2A(bits.field);
	bool result = regex_match(tmp, correct);

	if (!result) {
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
		for (size_t i = 0; i < incorrect.size(); i++) {
			regex reg(incorrect[i]);
			if (regex_search(tmp, reg)) {
				if (i == 3 || i == 7 || i == 10 || i == 13)	// Ошибки, связанные с промежутками (чтобы можно было генерировать в таком случае скрипты)
					result = true;
				error.push_back(description[i]);
				bFind = true;
			}
		}
		if (!bFind) {	// Неопознанная ошибка
			UndefinedError exc;
			exc.SetParam(bits.field);
			throw exc;
		}
	}
	return result;
}
#pragma endregion

#pragma region Simantic
// Проверка номера слова
bool CTest::simanticNumWord(const intData& num, const bool& arinc, vector <CString>& error) {
	bool result = true;
	if (!num.flag) {
		vector <CString> tmp;
		tmp.push_back(errRemarks[0]);
		for (size_t i = 0; i < num.value.size(); i++) {
			if (num.value[i] > 32 && !arinc) {	// Слово должно быть не больше 32
				result = false;
				tmp.push_back(L"Значение номера слова должно быть меньше 32");
			}
		}

		if (tmp.size() > 1)
			for (size_t i = 0; i < tmp.size(); i++)
				error.push_back(tmp[i]);
	}
	return result;
}

// Проверка наименований сигнала
bool CTest::simanticTitle(const int& indx, const CString& title, const bool& flag, vector <CString>& error) {
	bool result = true;
	if (flag) {
		if (findRepiteInSheet(title, indx)) {
			result = false;
			error.push_back(errRemarks[1]);
			error.push_back(L"Сигнал с таким обозначением присутсвует на этом листе");
		}
	}
	return result;
}

// Проверка минимального, максимального и цср
bool CTest::simanticValue(const signalData& signal, vector <CString>& error) {
	bool result = true;
	if (!signal.min.flag && !signal.max.flag && !signal.csr.flag && !signal.bit.flag) {
		int nBit = 0;
		(signal.bit.value.size() == 4) ? nBit = (signal.bit.value[1] - signal.bit.value[0]) + (signal.bit.value[3] - signal.bit.value[2]) + 2 :
			nBit = (signal.bit.value[1] - signal.bit.value[0]) + 1;

		// Выделение бита под знак, если он присутствует
		if (signal.bitSign) nBit--;

		double nMin = signal.csr.value;
		for (int i = 1; i <= nBit; i++)
			nMin = nMin / 2;

		double nMax = (signal.csr.value * 2) - nMin;

		if (signal.max.value - nMax >= 2) {
			result = false;
			error.push_back(errRemarks[3]);
			error.push_back(L"Нельзя упаковать данное значение");
		}
		else if (((abs(signal.min.value) > (nMax + nMin)) || (abs(signal.min.value) < nMin)) && signal.min.value != 0) {
			result = false;
			error.push_back(errRemarks[2]);
			error.push_back(L"Нельзя упаковать данное значение");
		}
		if ((signal.min.value < 0) && !signal.bitSign) {
			result = false;
			error.push_back(errRemarks[2]);
			error.push_back(L"Не может быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\"");
		}
		else if ((signal.min.value >= 0) && signal.bitSign) {
			result = false;
			error.push_back(errRemarks[2]);
			error.push_back(L"Должно быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\"");
		}
	}
	return result;
}

// Проверка используемых разрядов
bool CTest::simanticBits(const signalData& signal, const CString& prevTitle, vector<repiter>& repit, vector <CString>& error) {
	bool result = true;
	// Кол-во № слов должно совпадать с кол-вами интервалов исп. разрядов
	if (!signal.numWord.flag && !signal.bit.flag) {
		if (signal.numWord.value.size() * 2 == signal.bit.value.size()) {
			if (!checkCrossBits(signal.bit.value, signal.numWord.value, repit) && !checkSameTitle(signal.title[0], prevTitle)) {
				result = false;
				error.push_back(errRemarks[5]);
				error.push_back(L"Бит(ы) перекрывает(ют)ся");
			}
		}
		else {
			result = false;
			error.push_back(errRemarks[5]);
			(signal.numWord.value.size() == 1) ? error.push_back(L"Должен быть один интервал") : error.push_back(L"Должно быть два интервала");
		}
	}
	return result;
}
#pragma endregion

#pragma region subFunc
// Проверка на перекрытие битов
bool CTest::checkCrossBits(const vector <int>& bits, const vector <int>& numWord, vector<repiter>& repit) {
	bool result = true;
	for (size_t j = 0; j < 2; j += 2)
		for (size_t i = 0; i < numWord.size(); i++) {
			int end, start = bits[j];
			(bits[j + 1] == -1) ? end = start : end = bits[j + 1];

			for (; start <= end; start++) {
				int indx = isContain(repit, numWord[i]);
				if (repit[indx].bits[start-1])	// отметка в матрице о наличии бита
					repit[indx].bits[start-1] = false;
				else
					result = false;
			}
		}
	return result;
}

// Проверка двух соседних наименований на совпадение
bool CTest::checkSameTitle(const CString& next, const CString& prev) {
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

// Инициализация репитера для проверки перекрытия
void CTest::initRepiter(vector <repiter>& repit) {
	for (int i = 0; i < tmpSheet->signals.size(); i++) {
		for (int j = 0; j < tmpSheet->signals[i].numWord.value.size(); j++) {
			if (isContain(repit, tmpSheet->signals[i].numWord.value[j]) == -1) {
				repiter tmp;
				tmp.adr = tmpSheet->signals[i].numWord.value[j];
				tmp.bits = new bool[MAX_BITS];	// Выделение память под биты
				for (int k = 0; k < MAX_BITS; k++)	// Инициализация массива перекрытия
					tmp.bits[k] = true;
				repit.push_back(tmp);
			}
		}
	}
}

// Имеется ли уже такой номер слова (адрес) (в случае удачи возвр. индекс, иначе -1)
int CTest::isContain(const vector<repiter>& repit, const int& numeric) {
	int result = -1;
	for (int i = 0; i < repit.size(); i++)
		if (repit[i].adr == numeric)
			result = i;
	return result;
}

// Поиск повторений на листе
bool CTest::findRepiteInSheet(const CString& field, const int& indx) {
	bool result = false;
	for (size_t i = indx + 1; i < tmpSheet->signals.size(); i++)
		if (tmpSheet->signals[i].title[1].Compare(field) == 0)
			result = true;
	return result;
}

// Поиск повторений в книге
bool CTest::findRepiteInBook(const CString& field, const int& indx) {
	bool result = false;
	for (size_t i = 0; i < tmpBook->sheets.size(); i++)
		for (size_t j = 0; j < tmpBook->sheets[i].signals.size(); j++)
			if (tmpBook->sheets[i].signals[j].title[1].Compare(field) == 0) {

			}
	return result;
}
#pragma endregion