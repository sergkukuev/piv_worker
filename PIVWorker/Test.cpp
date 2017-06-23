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
		errorSet set;
		// ТУТ НАДО УКАЗАТЬ НА КНИГУ
		for (size_t j = 0; it->sheets.size(); j++) {
			errorSheet err;
			err.sheet = &it->sheets[j];
			getErrors(&it->sheets[j], err.syntax, err.simantic);
			getWarning(&it->sheets[j], err.warning);
			set.yupi.push_back(err);
		}
		errors.push_back(set);
	}

	return errors;
}

void CTest::getErrors(sheetData* sheet, vector <errorSignal>& syntax, vector <errorSignal>& simantic) {
	if (sheet->np == 0) {
		errorSignal tmp;
		tmp.signal = &sheet->signals[0];
		tmp.error.push_back(errRemarks[6]);
		tmp.error.push_back(_T("Отсутствует номер набора параметров"));
		syntax.push_back(tmp);
	}

	bool numRepit[32];
	bool bitRepit[16][32];

	// Доделать проверку на мин, макс и цср
	try {
		for (size_t i = 0; i < sheet->signals.size(); i++) {

			if (sheet->signals[i].title[0].CompareNoCase(RESERVE_SIGNAL) != 0) { // Проверка на резервность
				fieldError errors;
				bool result = true;

				errorSignal tmp;
				tmp.signal = &sheet->signals[i];
				syntaxValue(sheet->signals[i], tmp.error, errors) ? result = result : result = false;
				syntaxTitle(sheet->signals[i].title[0], tmp.error) ? result = result : result = false;
				syntax.push_back(tmp);

				errorSignal tmp2;
				tmp2.signal = &sheet->signals[i];
				simanticNumWord(sheet->signals[i].numWord, tmp2.error, errors.numWord, numRepit) ? result = result : result = false;
				simanticTitle(sheet, sheet->signals[i].title[0], tmp2.error, errors.title) ? result = result : result = false;
				simanticValue() ? result = result : result = false;
				simanticBits(sheet->signals[i], tmp2.error, errors, bitRepit);

				if (!result)
					sheet->error = false;
			}
		}
	}
	catch (UndefinedError& exc) {
		exc.SetName(sheet->name);
		throw exc;
	}
}

#pragma region Syntax

bool CTest::syntaxValue(signalData& signal, vector <CString>& error, fieldError& errValue) {
	int result = checkValue(signal.numWord);
	
	if (result != -1) {
		error.push_back(errRemarks[0]);
		CString message;
		message.Format(ERROR_DATA, result, _T("слова"));
		error.push_back(message);
		errValue.numWord = true;
	}

	result = checkValue(signal.bit);

	if (result != -1) {
		error.push_back(errRemarks[5]);
		CString message;
		message.Format(ERROR_DATA, result, _T("бита"));
		error.push_back(message);
		errValue.bit = true;
	}
}

int CTest::checkValue(vector <int> numeric) {
	int result = -1;
	for (size_t i = 0; i < numeric.size(); i++) {
		if (numeric[i] == 0)
			result = (int)i;
	}
	return result;
}

bool CTest::syntaxTitle(CString title, vector <CString>& error) {
	regex correct("^[A-Za-z][A-Za-z0-9_]*$");
	string tmp = CT2A(title);
	bool result = regex_match(tmp, correct);

	if (!result) {
		error.push_back(errRemarks[1]);
		bool bFind = false;	// Найдена ли ошибка из набора регулярных выражений
		vector <string> incorrect = { "(^$)", "[ \t\n]+", "^[^A-Za-z][A-Za-z0-9_]*$", "([A-Za-z0-9_]*[А-Яа-я]+[A-Za-z0-9_]*)+", "_$" };
		vector <CString> description = { _T("Значение в поле отсутствует. (Возможно сигнал зарезервирован, но \"Резерв\" не написано)"), 
										_T("Значение в поле содержит пробел"),
										_T("Значение в поле начинается не с латинской буквы"),
										_T("Значение в поле содержит кириллицу"), 
										_T("Значение в поле заканчивается на '_'") };
		for (size_t i = 0; i < incorrect.size(); i++) {
			regex reg(incorrect[i]);
			if (regex_search(tmp, reg)) {
				error.push_back(description[i]);
				bFind = true;
			}
		}
		if (bFind) {
			UndefinedError exc;
			exc.SetParam(title);
			throw exc;
		}
	}

	return result;
}

#pragma endregion

#pragma region Simantic

bool CTest::simanticNumWord(vector <int> num, vector <CString>& error, bool correct, bool repit[]) {
	bool result = true;
	if (!correct) {
		vector <CString> tmp;
		tmp.push_back(errRemarks[0]);
		if (num[1] > 0) {
			if (!repit[num[1] - 1])		// Поиск совпадений № слов для второго слова
				tmp.push_back(NumRepite);
			else if (!repit[num[0] - 1])	// Для первого
				tmp.push_back(NumRepite);
		}

		if (num[0] > 32 || num[1] > 32)	// Слово должно быть не больше 32
			tmp.push_back(NumMore32);
		else {
			// Отметка о том, что эти слова имеются на этом листе
			repit[num[0] - 1] = false;
			if (num[1] > 0)
				repit[num[1] - 1] = false;
		}

		if (tmp.size() > 1) {
			result = false;
			for (size_t i = 0; i < tmp.size(); i++)
				error.push_back(tmp[i]);
		}
	}
	return result;
}

bool CTest::simanticTitle(sheetData* sheet, CString title, vector <CString>& error, bool err) {
	bool result = true;
	if (!err) {
		if (findRepiteInSheet(title, sheet)) {
			error.push_back(errRemarks[1]);
			error.push_back(TitleRepSheet);
			result = false;
		}
	}

	return result;
}

bool CTest::simanticValue() {

}

bool CTest::simanticBits(signalData& signal, vector <CString>& error, fieldError errValue, bool repit[][32]) {
	bool result = true;
	// Кол-во № слов должно совпадать с кол-вами интервалов исп. разрядов
	if (!errValue.numWord && !errValue.bit) {
		if ((signal.numWord[1] == -1 && signal.bit[2] == -1) || (signal.numWord[1] != -1 && signal.bit[2] != -1)) {
			if (!checkCrossBits(signal.bit, signal.numWord, repit)) {
				error.push_back(errRemarks[5]);
				error.push_back(BitsCross);
				result = false;
			}
		}
		else {
			result = false;
			error.push_back(errRemarks[5]);
			(signal.numWord[1] == -1) ? error.push_back(BitsOneInter) : error.push_back(BitsTwoInter);
		}
	}
	return result;
}

// Проверка на перекрытие битов
bool CTest::checkCrossBits(vector <int>& bits, vector <int>& numWord, bool repiter[][32]) {
	bool result = true;
	for (size_t j = 0; j < bits.size() / 2; j + 2)
		for (size_t i = 0; i < numWord.size(); i++) {
			int end, start = bits[j];
			(bits[j + 1] == -1) ? end = start : end = bits[j + 1];
			
			if (numWord[i] != -1) {
				for (; start <= end; start++) {
					if (repiter[numWord[i] - 1][start - 1])
						repiter[numWord[i] - 1][start - 1] = false;
				}
			}
		}
	return result;
}

// Поиск повторений идентификатора на листе
bool CTest::findRepiteInSheet(CString field, sheetData* sheet) {
	int result = 0;
	for (size_t i = 0; i < sheet->signals.size(); i++)
		if (sheet->signals[i].title[1].Compare(field) == 0)
			result++;

	if (result > 1)
		return true;

	return false;
}

#pragma endregion

/*
// Замечания
void CTest::Warning(errorBookData& errBook, bookData& book) {
	for (size_t cSheet = 0; cSheet < errBook.sheets.size(); cSheet++) {
		errBook.sheets[cSheet].name = book.sheets[cSheet].name;

		for (list <signalData>::iterator it = book.sheets[cSheet].signals.begin(); it != book.sheets[cSheet].signals.end(); it++) {
			if (it->sTitleParamField->Compare(_T("Резерв")) != 0) {
				list <CString> error;

				if (!it->bTitleParamField)
					if (findRepiteInBook(it->sTitleParamField[1], book))
						error.push_back(ErrorBase.sim.TitleRepBook);

				if (!error.empty()) {
					error.push_front(errRemarks[1]);
					errorSignalData signal = getErrSignal(it, error);
					errBook.sheets[cSheet].signals.push_back(signal);
					it->bTitleParamField = true;
				}
			}
		}
	}
}

// Проверка минимального, максимального и цср
bool CTest::simanticMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it, int currNP, bool begin) {
	list <CString> errMin, errMax;

	if (!it->bMinValField && !it->bMaxValField && !it->bCsrValField && !it->bBitField && !it->bBitSigns) {
		if (begin) {
			if (it->dMinMaxCsrVal[0] != currNP)
				errMin.push_back(ErrorBase.sim.MMCNotNote);
			if (it->dMinMaxCsrVal[1] != currNP)
				errMax.push_back(ErrorBase.sim.MMCNotNote);
		}

		// Подсчет битов
		int nBit = 0;
		if (it->b2BitField)
			nBit = (it->iBit[1] - it->iBit[0]) + (it->iBit[3] - it->iBit[2]) + 2;
		else
			nBit = it->iBit[1] - it->iBit[0] + 1;

		if (it->bCommentField)	// Если есть знаковое описание, то -1 бит	
			nBit--;

		// Подсчет минимума и максимума
		int nMin = it->dMinMaxCsrVal[2];	// мин
		for (int i = 1; i <= nBit; i++)
			nMin = nMin / 2;

		int nMax = (it->dMinMaxCsrVal[2] * 2) - nMin;	// макс

		if (it->dMinMaxCsrVal[1] - nMax > 2)
			errMax.push_back(ErrorBase.sim.MMCNotPkg);
		else if (( (abs(it->dMinMaxCsrVal[0]) > (nMax + nMin)) || (abs(it->dMinMaxCsrVal[0]) < nMin)) && it->dMinMaxCsrVal[0] != 0)
			errMin.push_back(ErrorBase.sim.MMCNotPkg);

		if ((it->dMinMaxCsrVal[0] < 0) && !it->bCommentField)
			errMin.push_back(ErrorBase.sim.MMCNotNegative);
		else if ((it->dMinMaxCsrVal[0] >= 0) && it->bCommentField)
			errMin.push_back(ErrorBase.sim.MMCNegative);
	}

	if (errMin.empty() && errMax.empty())
		return true;
	else if (!errMin.empty()) {
		errMin.push_front(errRemarks[2]);
		errorSignalData signal = getErrSignal(it, errMin);
		sheet.signals.push_back(signal);
	}
	else {
		errMax.push_front(errRemarks[3]);
		errorSignalData signal = getErrSignal(it, errMax);
		sheet.signals.push_back(signal);
	}

	return false;
}

// Поиск повторений в книге
bool CTest::findRepiteInBook(CString field, bookData book) {
	int result = 0;

	for (size_t iSheet = 0; iSheet < book.sheets.size(); iSheet++) {
		list<signalData>::iterator& it = book.sheets[iSheet].signals.begin();	it++;
		for (; it != book.sheets[iSheet].signals.end(); it++)
			if (it->sTitleParamField[1].Compare(field) == 0)
				result++;
	}
	
	if (result > 1)
		return true;

	return false;
}
*/