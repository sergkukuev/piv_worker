#include "stdafx.h"
#include "Test.h"

// Конструктор
CTest::CTest() {	}

// Деструктор
CTest::~CTest()	{	}

// Проверка на все ошибки
list <errorSet> CTest::Start(list <bookData>& books) {	
	list <errorSet> errors;
	list <bookData>::iterator it = books.begin();

	for (it = books.begin(); it != books.end(); it++) {
		errorSet error;
		error.book = it;
		for (size_t j = 0; j < it->sheets.size(); j++) {
			errorSheet tmp;
			tmp.sheet = &it->sheets[j];
			getErrors(&it->sheets[j], tmp.syntax, tmp.simantic);
			getWarnings(&it->sheets[j], tmp.warning);
			error.set.push_back(tmp);
		}
		errors.push_back(error);
	}

	return errors;
}

// Проверка на ошибки
void CTest::getErrors(sheetData* sheet, vector <errorSignal>& syntax, vector <errorSignal>& simantic) {
	checkNP(sheet->signals[0], sheet->np, syntax);
	try {
		bool *numRepit = new bool[MAX_NUMWORD];		// Повторения слов
		bool **bitRepit = new bool*[MAX_NUMWORD];	// Повторения битов
		for (int i = 0; i < MAX_NUMWORD; i++)
			bitRepit[i] = new bool[MAX_BITS];

		initRepiter(numRepit, bitRepit);	// Инициализация

		for (size_t i = 0; i < sheet->signals.size(); i++) {

			if (sheet->signals[i].title[0].CompareNoCase(RESERVE_SIGNAL) != 0) { // Проверка на резервность
				bool correctTitle = true;

				// Проход по синтаксическим ошибкам
				errorSignal tmp;
				tmp.signal = &sheet->signals[i];
				syntaxValue(sheet->signals[i].flags, tmp.error);
				sheet->signals[i].title[1].Trim();
				correctTitle = syntaxTitle(sheet->signals[i].title, tmp.error);
				
				if (!tmp.error.empty()) {
					sheet->error = true;
					syntax.push_back(tmp);
					tmp.error.clear();
				}

				// Проход по семантическми ошибкам
				simanticNumWord(sheet->signals[i].numWord, sheet->signals[i].flags.num, numRepit, tmp.error);
				simanticTitle(sheet, sheet->signals[i].title[1], correctTitle, tmp.error);
				simanticValue(sheet->signals[i], tmp.error);
				simanticBits(sheet->signals[i], bitRepit, tmp.error);

				if (!tmp.error.empty()) {
					sheet->error = true;
					simantic.push_back(tmp);
				}
			}
		}

		// Высвобождение памяти
		delete[] numRepit;
		for (int i = 0; i < MAX_BITS; i++)
			delete[] bitRepit[i];
		delete[] bitRepit;
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
			// Проверка на присутствие параметров в книге
			warning.push_back(tmp);
		}
	}
}

// Проверка ошибок, связанных с номером набора
void CTest::checkNP(signalData& signal, const int& np, vector <errorSignal>& syntax) {
	errorSignal tmp;
	tmp.signal = &signal;

	if (np == 0) {
		tmp.error.push_back(errRemarks[6]);
		tmp.error.push_back(L"Отсутствует номер набора параметров");
		syntax.push_back(tmp);
	}

	// Минимально и максимальное значение должно соответствовать номер набора
	if (!signal.flags.min && !signal.flags.max)
		if (signal.min != np || signal.min != np)
			tmp.error.push_back(L"Значение не соответствует значению в примечании");
}

// Инициализация репитеров
void CTest::initRepiter(bool* num, bool** bits) {
	for (int i = 0; i < MAX_NUMWORD; i++) {
		num[i] = true;
		for (int j = 0; j < MAX_BITS; j++)
			bits[i][j] = true;
	}
}

#pragma region Syntax

// Проверка числовых параметров 
void CTest::syntaxValue(const convertError& flags, vector <CString>& error) {
	checkValueByFlag(L"№ слова", 0, flags.num, error);
	checkValueByFlag(L"минимума", 2, flags.min, error);
	checkValueByFlag(L"максимума", 3, flags.max, error);
	checkValueByFlag(L"цср", 4, flags.csr, error);
	checkValueByFlag(L"битов", 5, flags.bit, error);
}

// Проверка числовых параметров по набору флагов
void CTest::checkValueByFlag(const CString& field, const int& indx, const bool& flag, vector <CString>& error) {
	if (flag) {
		error.push_back(errRemarks[indx]);
		CString message;
		message.Format(ERROR_DATA, field);
		error.push_back(message);
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

#pragma endregion

#pragma region Simantic

// Проверка номера слова
void CTest::simanticNumWord(const vector <int>& num, const bool& flag, bool* repiter, vector <CString>& error) {
	if (!flag) {
		vector <CString> tmp;
		tmp.push_back(errRemarks[0]);
		for (size_t i = 0; i < num.size(); i++) {
			//if (!repiter[num[i] - 1])		// Поиск совпадений 
				//tmp.push_back(_T("Слово с таким номером встречалось ранее на листе"));

			if (num[i] > 32)	// Слово должно быть не больше 32
				tmp.push_back(L"Значение номера слова должно быть меньше 32");
			else
				*(repiter + num[i] - 1) = false; // Отметка о том, что эти слово имеется на этом листе
		}
		if (tmp.size() > 1)
			for (size_t i = 0; i < tmp.size(); i++)
				error.push_back(tmp[i]);
	}
}

// Проверка наименований сигнала
void CTest::simanticTitle(sheetData* sheet, const CString& title, const bool& flag, vector <CString>& error) {
	if (flag) {
		if (findRepiteInSheet(title, sheet)) {
			error.push_back(errRemarks[1]);
			error.push_back(L"Сигнал с таким обозначением присутсвует на этом листе");
		}
	}
}

// Проверка минимального, максимального и цср
void CTest::simanticValue(const signalData& signal, vector <CString>& error) {
	if (!signal.flags.min && !signal.flags.max && !signal.flags.csr && !signal.flags.bit) {
		int nBit = 0;
		if (signal.bit.size() == 4)
			nBit = (signal.bit[1] - signal.bit[0]) + (signal.bit[3] - signal.bit[2]) + 2;
		else
			nBit = (signal.bit[1] - signal.bit[0]) + 1;

		// Выделение бита под знак, если он присутствует
		if (signal.bitSign) nBit--;

		double nMin = signal.csr;
		for (int i = 1; i <= nBit; i++)
			nMin = nMin / 2;

		double nMax = (signal.csr * 2) - nMin;	

		if (signal.max - nMax > 2) {
			error.push_back(errRemarks[3]);
			error.push_back(L"Нельзя упаковать данное значение");
		}
		else if (((abs(signal.min) > (nMax + nMin)) || (abs(signal.min) < nMin)) && signal.min != 0) {
			error.push_back(errRemarks[3]);
			error.push_back(L"Нельзя упаковать данное значение");
		}

		if ((signal.min < 0) && !signal.bitSign) {
			error.push_back(errRemarks[2]);
			error.push_back(L"Не может быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\"");
		}
		else if ((signal.min >= 0) && signal.bitSign) {
			error.push_back(errRemarks[2]);
			error.push_back(L"Должно быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\"");
		}
	}
}

// Проверка используемых разрядов
void CTest::simanticBits(const signalData& signal, bool** repiter, vector <CString>& error) {
	// Кол-во № слов должно совпадать с кол-вами интервалов исп. разрядов
	if (!signal.flags.num && !signal.flags.bit) {
		if (signal.numWord.size() * 2 == signal.bit.size()) {
			if (!checkCrossBits(signal.bit, signal.numWord, repiter)) {
				error.push_back(errRemarks[5]);
				error.push_back(L"Бит(ы) перекрывает(ют)ся");
			}
		}
		else {
			error.push_back(errRemarks[5]);
			(signal.numWord.size() == 1) ? error.push_back(L"Должен быть один интервал") : error.push_back(L"Должно быть два интервала");
		}
	}
}

// Проверка на перекрытие битов
bool CTest::checkCrossBits(const vector <int>& bits, const vector <int>& numWord, bool** repiter) {
	bool result = true;
	for (size_t j = 0; j < 2; j += 2)
		for (size_t i = 0; i < numWord.size(); i++) {
			int end, start = bits[j];
			(bits[j + 1] == -1) ? end = start : end = bits[j + 1];
			
			if (numWord[i] != -1) {
				for (; start <= end; start++) {
					if (repiter[numWord[i] - 1][start - 1])
						repiter[numWord[i] - 1][start - 1] = false;
					else
						result = false;
				}
			}
		}
	return result;
}

// Поиск повторений идентификатора на листе
bool CTest::findRepiteInSheet(const CString& field, sheetData* sheet) {
	int result = 0;
	for (size_t i = 0; i < sheet->signals.size(); i++)
		if (sheet->signals[i].title[1].Compare(field) == 0)
			result++;

	if (result > 1)
		return true;
	else
		return false;
}

#pragma endregion