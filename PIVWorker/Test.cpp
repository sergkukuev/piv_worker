#include "stdafx.h"
#include "Test.h"

// Конструктор
CTest::CTest()	{	}

// Деструктор
CTest::~CTest()	{	}

// Проверка на все ошибки
errorOneSet CTest::Start(bookData& book) 
{	
	// Синтаксические ошибки
	error.syntax.sheets.resize(book.sheets.size());
	error.syntax.name = book.name;
	Syntax(error.syntax, book);

	// Семантические ошибки
	error.simantic.sheets.resize(book.sheets.size());
	error.simantic.name = book.name;
	Simantic(error.simantic, book);

	// Замечания
	error.warning.sheets.resize(book.sheets.size());
	error.warning.name = book.name;
	Warning(error.warning, book);
	
	return error;
}

#pragma region Syntax

// Проверка на синтаксические ошибки
void CTest::Syntax(errorBookData& errBook, bookData& book)
{
	for (size_t cSheet = 0; cSheet < errBook.sheets.size(); cSheet++)
	{
		errBook.sheets[cSheet].name = book.sheets[cSheet].name;

		for (list <signalData>::iterator it = book.sheets[cSheet].signals.begin(); it != book.sheets[cSheet].signals.end(); it++)
		{
			try 
			{
				bool begin = true, result = true;
				
				if (it != book.sheets[cSheet].signals.begin())	
					begin = false;

				if (it->sTitleParamField->Compare(_T("Резерв")) != 0)
				{
					result = syntaxNumWord(errBook.sheets[cSheet], it);
					(!syntaxTitleParam(errBook.sheets[cSheet], it)) ? result = false : result = result;
					(!syntaxMinMaxCSR(errBook.sheets[cSheet], it)) ? result = false : result = result;
					(!syntaxBits(errBook.sheets[cSheet], it)) ? result = false : result = result;
					(!syntaxComment(errBook.sheets[cSheet], it, begin)) ? result = false : result = result;
				}
				
				if (NP != 0)
				{
					book.sheets[cSheet].iFieldNP = NP;	// Установка номера набора (translateComment)
					NP = 0;
				}

				if (!result)	// Установка флага, что на листе есть ошибка
					book.sheets[cSheet].bError = true;
			}
			catch (UndefinedError& exc)
			{
				exc.SetName(errBook.sheets[cSheet].name);
				throw exc;
			}
		}
	}
}

// Проверка номера слова
bool CTest::syntaxNumWord(errorSheetData& sheet, list<signalData>::iterator& it)
{
	list <CString> error = testField(it->sNumWordField, ErrorBase.getNumWord());

	if (error.empty())
	{
		translateNumWord(it);	// Преобразование слов в числа
		return true;
	}
	
	error.push_front(errRemarks[0]);
	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
	it->bNumWordField = true;

	return false;
}

// Проверка идентификатора
bool CTest::syntaxTitleParam(errorSheetData& sheet, list<signalData>::iterator& it)
{
	list <CString> error = testField(it->sTitleParamField[1], ErrorBase.getTitleParam());

	if (error.empty())
		return true;

	error.push_front(errRemarks[1]);
	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
	it->bTitleParamField = true;

	return false;
}

// Проверка минимального, максимального и цср
bool CTest::syntaxMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it)
{
	list <CString> errMin, errMax, errCSR;
	bool result = true;

	if (!it->sMinMaxCsrValField[0].IsEmpty() || !it->sMinMaxCsrValField[0].IsEmpty() || !it->sMinMaxCsrValField[0].IsEmpty())
	{
		errMin = testField(it->sMinMaxCsrValField[0], ErrorBase.getMinMaxCSR());
		errMax = testField(it->sMinMaxCsrValField[1], ErrorBase.getMinMaxCSR());
		errCSR = testField(it->sMinMaxCsrValField[2], ErrorBase.getMinMaxCSR());
	}

	if (!errMin.empty())
	{
		errMin.push_front(errRemarks[2]);
		errorSignalData signal = getErrSignal(it, errMin);
		sheet.signals.push_back(signal);
		it->bMinValField = true;
		result = false;
	}

	if (!errMax.empty())
	{
		errMax.push_front(errRemarks[3]);
		errorSignalData signal = getErrSignal(it, errMax);
		sheet.signals.push_back(signal);
		it->bMaxValField = true;
		result = false;
	}

	if (!errCSR.empty())
	{
		errCSR.push_front(errRemarks[4]);
		errorSignalData signal = getErrSignal(it, errCSR);
		sheet.signals.push_back(signal);
		it->bCsrValField = true;
		result = false;
	}

	if (result)
		translateMMC(it);

	return result;
}

// Проверка используемых разрядов
bool CTest::syntaxBits(errorSheetData& sheet, list<signalData>::iterator& it)
{
	list <CString> error = testField(it->sBitField, ErrorBase.getBits());

	if (error.empty())
	{
		translateBits(it);	// Считывание битов
		return true;
	}

	error.push_front(errRemarks[5]);
	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
	it->bBitField = true;

	return false;
}

// Проверка комментариев
bool CTest::syntaxComment(errorSheetData& sheet, list<signalData>::iterator& it, bool begin)
{
	list <CString> error;
	string field = CT2A(it->sCommentField);

	if (begin)
	{
		bool bNP = regex_search(field, ErrorBase.getComment().error[0]);	// Проверка на набор параметров
		if (bNP)
			error.push_back(ErrorBase.getComment().description[0]);
	}
	else
	{
		bool zn = regex_search(field, ErrorBase.getComment().error[1]);	// Проверка на знак
		if (zn)
		{
			error.push_back(ErrorBase.getComment().description[1]);
			it->bBitSigns = true;
		}		
	}

	if (error.empty())
	{
		translateComment(it);	// Достать значение знакового бита или NP набора, если они есть
		return true;
	}

	error.push_front(errRemarks[6]);
	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);

	return false;
}

// Проверка поля на ошибки
list <CString> CTest::testField(CString field, errorData errStruct)
{
	string temp = CT2A(field);
	list <CString> error;	// Набор найденных ошибок в поле
	bool result = false;

	result = regex_match(temp, errStruct.correct);	// Проверка на корректную регулярку

	if (result)	return error;

	for (size_t i = 0; i < errStruct.error.size(); i++)	// Перебор базы ошибок
		if (regex_search(temp, errStruct.error[i]))
		{
			error.push_back(errStruct.description[i]);
			result = true;
		}

	if (!result)	// исключение, если в базе ошибок такая ошибка не предусмотрена
	{
		UndefinedError exc;
		exc.SetParam(field);
		throw exc;
	}

	return error;
}

#pragma endregion 

// Замечания
void CTest::Warning(errorBookData& errBook, bookData& book)
{
	for (size_t cSheet = 0; cSheet < errBook.sheets.size(); cSheet++)
	{
		errBook.sheets[cSheet].name = book.sheets[cSheet].name;

		for (list <signalData>::iterator it = book.sheets[cSheet].signals.begin(); it != book.sheets[cSheet].signals.end(); it++)
		{
			if (it->sTitleParamField->Compare(_T("Резерв")) != 0)
			{
				list <CString> error;

				if (!it->bTitleParamField)
					if (findRepiteInBook(it->sTitleParamField[1], book))
						error.push_back(ErrorBase.sim.TitleRepBook);

				if (!error.empty())
				{
					error.push_front(errRemarks[1]);
					errorSignalData signal = getErrSignal(it, error);
					errBook.sheets[cSheet].signals.push_back(signal);
					it->bTitleParamField = true;
				}
			}
		}
	}
}

#pragma region Simantic

// Проверка на семантические ошибки
void CTest::Simantic(errorBookData& errBook, bookData& book)
{
	for (size_t cSheet = 0; cSheet < errBook.sheets.size(); cSheet++)
	{
		errBook.sheets[cSheet].name = book.sheets[cSheet].name;
		bool wRepiter[32];		// Для отслеживания повторений слов
		bool tRepiter[32][32];	// Для отслеживания бито перекрытия

		// Очистка массивов
		for (int i = 0; i < 32; i++)
		{
			wRepiter[i] = true;
			for (int j = 0; j < 32; j++)
				tRepiter[i][j] = true;
		}

		for (list <signalData>::iterator it = book.sheets[cSheet].signals.begin(); it != book.sheets[cSheet].signals.end(); it++)
		{
			bool begin = true, result = true;
			
			if (it != book.sheets[cSheet].signals.begin())	
				begin = false;
			
			if (it->sTitleParamField->Compare(_T("Резерв")) != 0)
			{
				result = simanticNumWord(errBook.sheets[cSheet], it, wRepiter);
				(!simanticTitleParam(errBook.sheets[cSheet], it, book, cSheet)) ? result = false : result = result;
				(!simanticMinMaxCSR(errBook.sheets[cSheet], it, book.sheets[cSheet].iFieldNP, begin)) ? result = false : result = result;
				(!simanticBits(errBook.sheets[cSheet], it, tRepiter)) ? result = false : result = result;
			}

			if (!result)	// Установка флага, что на листе есть ошибка
				book.sheets[cSheet].bError = true;
		}
	}
}

// Проверка номера слова
bool CTest::simanticNumWord(errorSheetData& sheet, list<signalData>::iterator& it, bool wRep[])
{
	list <CString> error;

	if (!it->bNumWordField)
	{
		if (it->iNumWord[1] > 0)
			if (!wRep[it->iNumWord[1] - 1])	// Поиск совпадений № слов для второго слова
				error.push_back(ErrorBase.sim.NumRepite);
		else if (!wRep[it->iNumWord[0] - 1])	// Для первого
			error.push_back(ErrorBase.sim.NumRepite);

		if (it->iNumWord[0] > 32 || it->iNumWord[1] > 32)	// Слово должно быть не больше 32
			error.push_back(ErrorBase.sim.NumMore32);
		else
		{
			// Отметка о том, что эти слова на этом листе
			wRep[it->iNumWord[0] - 1] = false;
			if (it->iNumWord[1] > 0)
				wRep[it->iNumWord[1] - 1] = false;
		}
	}

	if (error.empty())
		return true;

	error.push_front(errRemarks[0]);
	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);

	return false;
}

// Проверка идентификатора
bool CTest::simanticTitleParam(errorSheetData& errSheet, list<signalData>::iterator& it, bookData book, int iSheet)	// Прокинуть книгу и индекс листа
{
	list <CString> error;

	if (!it->bTitleParamField)
		if (findRepiteInSheet(it->sTitleParamField[1], book.sheets[iSheet]))
			error.push_back(ErrorBase.sim.TitleRepSheet);

	if (error.empty())
		return true;

	error.push_front(errRemarks[1]);
	errorSignalData signal = getErrSignal(it, error);
	errSheet.signals.push_back(signal);

	return false;
}

// Проверка минимального, максимального и цср
bool CTest::simanticMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it, int currNP, bool begin)
{
	list <CString> errMin, errMax;

	if (!it->bMinValField && !it->bMaxValField && !it->bCsrValField && !it->bBitField && !it->bBitSigns)
	{
		if (begin)
		{
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
	else if (!errMin.empty())
	{
		errMin.push_front(errRemarks[2]);
		errorSignalData signal = getErrSignal(it, errMin);
		sheet.signals.push_back(signal);
	}
	else
	{
		errMax.push_front(errRemarks[3]);
		errorSignalData signal = getErrSignal(it, errMax);
		sheet.signals.push_back(signal);
	}

	return false;
}

// Проверка используемых разрядов
bool CTest::simanticBits(errorSheetData& sheet, list<signalData>::iterator& it, bool tRep[][32])
{
	list <CString> error;
	
	if (!it->bNumWordField && !it->bBitField)
	{
		if (it->b2NumWordField == it->b2BitField)	// Кол-во № слов должно совпадать с кол-вами интервалов исп. разрядов
		{
			if (!checkCrossBits(it, tRep))	// Проверка на перекрытие
				error.push_back(ErrorBase.sim.BitsCross);	

			if (it->bCommentField && !it->bBitSigns)
				if (it->iBitSigns != it->iBit[0])
					error.push_back(ErrorBase.sim.BitsNotSetSign);
		}
		else
		{
			if (it->b2NumWordField)
				error.push_back(ErrorBase.sim.BitsOneInter);
			if (it->b2BitField)
				error.push_back(ErrorBase.sim.BitsTwoInter);
		}
	}

	if (error.empty())
		return true;

	error.push_front(errRemarks[5]);
	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);

	return false;
}

// Проверка на перекрытие битов
bool CTest::checkCrossBits(list<signalData>::iterator& it, bool repiter[][32])
{
	bool result = true;
	int nInterval;
	(it->b2BitField) ? nInterval = 2 : nInterval = 1;

	for (int i = 0; i < nInterval; i++)
	{
		int start, end;

		if (i == 0)
		{
			start = it->iBit[0];
			end = it->iBit[1];
		}
		else
		{
			start = it->iBit[2];
			end = it->iBit[3];
		}

		if (end == 0)
			end = start;

		for (; start <= end; start++)
		{
			if (repiter[it->iNumWord[i] - 1][start - 1])
				repiter[it->iNumWord[i] - 1][start - 1] = false;
			else
				result = false;
		}
	}
	return result;
}

#pragma endregion

// Поиск повторений в книге
bool CTest::findRepiteInBook(CString field, bookData book)
{
	int result = 0;

	for (size_t iSheet = 0; iSheet < book.sheets.size(); iSheet++)
	{
		list<signalData>::iterator& it = book.sheets[iSheet].signals.begin();	it++;
		for (; it != book.sheets[iSheet].signals.end(); it++)
			if (it->sTitleParamField[1].Compare(field) == 0)
				result++;
	}
	
	if (result > 1)
		return true;

	return false;
}

// Поиск повторений на листе
bool CTest::findRepiteInSheet(CString field, sheetData sheet)
{
	int result = 0;
	list<signalData>::iterator& it = sheet.signals.begin(); it++;
	for (; it != sheet.signals.end(); it++)
		if (it->sTitleParamField[1].Compare(field) == 0)
			result++;

	if (result > 1)
		return true;

	return false;
}

// Создание записи ошибки сигнала
errorSignalData CTest::getErrSignal(list<signalData>::iterator it, list <CString> error)
{
	errorSignalData signal;

	signal.sNumWordField = it->sNumWordField;
	signal.sTitleParamField[0] = it->sTitleParamField[0];
	signal.sTitleParamField[1] = it->sTitleParamField[1];
	signal.sDimensionField = it->sDimensionField;
	signal.sMinMaxCsrValField[0] = it->sMinMaxCsrValField[0];
	signal.sMinMaxCsrValField[1] = it->sMinMaxCsrValField[1];
	signal.sMinMaxCsrValField[2] = it->sMinMaxCsrValField[2];
	signal.sBitField = it->sBitField;
	signal.sCommentField = it->sCommentField;
	signal.sErrorField = error;
	
	return signal;
}

// Перевод номеров слов из строки в числа
void CTest::translateNumWord(list<signalData>::iterator& it)
{
	CString num = it->sNumWordField;
	int indxDot = num.Find(_T(','));

	if (indxDot == -1)
	{
		num.Trim();	// Удаление пробелов
		it->iNumWord[0] = _wtoi(num);
	}
	else
	{
		CString num2 = num;	// Второе число
		num2.Delete(0, indxDot + 1);
		num2.Trim();
		it->iNumWord[1] = _wtoi(num2);

		num.Delete(indxDot, num.GetLength());	// Первое число
		num.Trim();
		it->iNumWord[0] = _wtoi(num);

		it->b2NumWordField = true; // Установка флага присутствия двух слов
	}
}

// Перевод из мин, макс и цср
void CTest::translateMMC(list<signalData>::iterator& it)
{
	it->dMinMaxCsrVal[0] = stepTranslateMMC(it->sMinMaxCsrValField[0]);
	it->dMinMaxCsrVal[1] = stepTranslateMMC(it->sMinMaxCsrValField[1]);
	it->dMinMaxCsrVal[2] = stepTranslateMMC(it->sMinMaxCsrValField[2]);
}

// Дополнительная функция перевода мин, макс, цср
double CTest::stepTranslateMMC(CString value)
{
	value.Replace(_T(','), _T('.'));
	return _wtof(value);
}

// Перевод из используемых разрядов из строки в числа
void CTest::translateBits(list<signalData>::iterator& it)
{
	CString bits = it->sBitField;
	int indxDot = bits.Find(_T(','));

	if (indxDot == -1)	// Для одного промежутка
	{
		vector <int> tmp = stepTranslateBits(bits);

		it->iBit[0] = tmp[0];
		it->iBit[1] = tmp[1];
	}
	else  // Для двух промежутков
	{
		CString bits2 = bits;
		bits2.Delete(0, indxDot + 1);
		bits.Delete(indxDot, bits.GetLength());
		bits.Trim();	bits2.Trim();

		vector <int> tmp = stepTranslateBits(bits);
		it->iBit[0] = tmp[0];
		it->iBit[1] = tmp[1];

		tmp.clear();
		tmp = stepTranslateBits(bits2);
		it->iBit[2] = tmp[0];
		it->iBit[3] = tmp[1];

		it->b2BitField = true; // Установка флага для двух промежутков
	}
}

// Дополнительная функция для перевода разрядов
vector <int> CTest::stepTranslateBits(CString bits)
{
	vector <int> result = { 0, 0 };
	bits.Trim();

	// Поиск индекса разделителей
	int indxDel = bits.Find(_T('.'));
	if (indxDel == -1)
		indxDel = bits.Find(_T('…'));
	else
		bits.Delete(indxDel, 2);	// Удалям две последующие точки

	if (indxDel == -1)	// Разделителей нет, используется один разряд
		result[0] = _wtoi(bits);
	else
	{
		CString num = bits;
		CString num2 = num;	// Второе число

		num.Delete(indxDel, num.GetLength());	// Первое число
		num.Trim();
		result[0] = _wtoi(num);

		num2.Delete(0, indxDel + 1);
		num2.Trim();
		result[1] = _wtoi(num2);
	}

	return result;
}

// Перевод знакового бита или NP набора, если он есть
void CTest::translateComment(list<signalData>::iterator& it)
{
	int indx = it->sCommentField.Find(_T("Зн-"));
	CString tmp = it->sCommentField;

	if (indx != -1)	// Чтение знакового бита
	{
		it->iBitSigns = stepTranslateComment(tmp, indx);

		it->bCommentField = true;	// Установка флага присутствия знака в примечании
	}

	indx = tmp.Find(_T("NP="));

	if (indx != -1)	// Чтение NP
		NP = stepTranslateComment(tmp, indx);
}

// Дополнительная функция для перевода знака или NP
int CTest::stepTranslateComment(CString field, int indx)
{
	field.Delete(0, indx + 3);

	int iSpace = field.Find(_T(' '));
	if (iSpace == -1)
		iSpace = field.Find(_T('\n'));
	if (iSpace == -1)
		iSpace = field.Find(_T('\0'));

	field.Delete(iSpace, field.GetLength());

	return _wtoi(field);
}