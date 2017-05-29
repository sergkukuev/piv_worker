#include "stdafx.h"
#include "Test.h"

// Конструктор
CTest::CTest()
{
}

// Деструктор
CTest::~CTest()
{
}

// Проверка на все ошибки
errorOneSet CTest::Start(bookData& book) 
{	
	// Синтаксические ошибки
	error.syntax.sheets.resize(book.sheets.size());
	error.syntax.name = book.nameBook;
	Syntax(error.syntax, book);

	// Семантические ошибки
	/*error.simantic.sheets.resize(book.sheets.size());
	error.simantic.name = book.nameBook;
	error.warning.sheets.resize(book.sheets.size());
	error.warning.name = book.nameBook;
	Simantic(error.simantic, book);
	*/
	return error;
}

#pragma region Syntax

// Проверка на синтаксические ошибки
void CTest::Syntax(errorBookData& errBook, bookData& book)
{
	for (size_t cSheet = 0; cSheet < errBook.sheets.size(); cSheet++)
	{
		errBook.sheets[cSheet].name = book.sheets[cSheet].nameSheet;

		for (list <signalData>::iterator it = book.sheets[cSheet].signals.begin(); it != book.sheets[cSheet].signals.end(); it++)
		{
			try 
			{
				bool begin = true;
				if (it != book.sheets[cSheet].signals.begin())	begin = false;
				bool result = syntaxNumWord(errBook.sheets[cSheet], it);
				(!syntaxTitleParam(errBook.sheets[cSheet], it))? result = false : result = result;
				(!syntaxMinMaxCSR(errBook.sheets[cSheet], it)) ? result = false : result = result;
				(!syntaxBits(errBook.sheets[cSheet], it))? result = false : result = result;
				(!syntaxComment(errBook.sheets[cSheet], it, begin)) ? result = false : result = result;

				if (!result)	// Установка флага, что на листе есть ошибка
					book.sheets[cSheet].bErrorSheet = false;
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
	
	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
	it->bNumWordField = false;
	return false;
}

// Проверка наименований сигнала
bool CTest::syntaxTitleParam(errorSheetData& sheet, list<signalData>::iterator& it)
{
	list <CString> error = testField(it->sTitleParamField[1], ErrorBase.getTitleParam());

	if (error.empty())
		return true;

	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
	it->bTitleParamField = false;
	return false;
}

// Проверка минимального, максимального и цср
bool CTest::syntaxMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it)
{
	list <CString> errMin = testField(it->sMinMaxCsrValField[0], ErrorBase.getMinMaxCSR());
	list <CString> errMax = testField(it->sMinMaxCsrValField[1], ErrorBase.getMinMaxCSR());
	list <CString> errCSR = testField(it->sMinMaxCsrValField[2], ErrorBase.getMinMaxCSR());
	bool result = true;

	if (!errMin.empty())
	{
		errorSignalData signal = getErrSignal(it, errMin);
		sheet.signals.push_back(signal);
		it->bMinValField = false;
		result = false;
	}
	else
	{
		CString buffer = it->sMinMaxCsrValField[0];
		it->iMinMaxCsrVal[0] = _wtoi(buffer);
	}

	if (!errMax.empty())
	{
		errorSignalData signal = getErrSignal(it, errMax);
		sheet.signals.push_back(signal);
		it->bMaxValField = false;
		result = false;
	}
	else
	{
		CString buffer = it->sMinMaxCsrValField[1];
		it->iMinMaxCsrVal[1] = _wtoi(buffer);
	}

	if (!errCSR.empty())
	{
		errorSignalData signal = getErrSignal(it, errCSR);
		sheet.signals.push_back(signal);
		it->bCsrValField = false;
		result = false;
	}
	else
	{
		CString buffer = it->sMinMaxCsrValField[2];
		it->iMinMaxCsrVal[2] = _wtoi(buffer);
	}

	return result;
}

// Проверка используемых разрядов
bool CTest::syntaxBits(errorSheetData& sheet, list<signalData>::iterator& it)
{
	list <CString> error = testField(it->sBitField, ErrorBase.getBits());

	if (error.empty())
		return true;

	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
	it->bBitField = false;

	return false;
}

// Проверка комментариев
bool CTest::syntaxComment(errorSheetData& sheet, list<signalData>::iterator& it, bool begin)
{
	list <CString> error;
	string field = convertString(it->sCommentField);
	bool bNP = false;
	bool zn = regex_search(field, ErrorBase.getComment().error[1]);	// Проверка на знак

	if (begin)
		bNP = regex_search(field, ErrorBase.getComment().error[0]);	// Проверка на набор параметров

	if (bNP)
		error.push_back(ErrorBase.getComment().description[0]);

	if (zn)
		error.push_back(ErrorBase.getComment().description[1]);

	if (error.empty())
	{
		size_t iBit = field.find("Зн-");
		// Достать значение знака
		return true;
	}
}

// Проверка поля на ошибки
list <CString> CTest::testField(CString field, errorData errStruct)
{
	string temp = convertString(field);
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

#pragma once region Simantic

// Проверка на семантические ошибки
void CTest::Simantic(errorBookData& errBook, bookData& book)
{
	for (size_t cSheet = 0; cSheet < errBook.sheets.size(); cSheet++)
	{
		errBook.sheets[cSheet].name = book.sheets[cSheet].nameSheet;

		for (list <signalData>::iterator it = book.sheets[cSheet].signals.begin(); it != book.sheets[cSheet].signals.end(); it++)
		{
			bool wRepite[32];
			bool result = simanticNumWord(errBook.sheets[cSheet], it, wRepite);
			(!simanticTitleParam(errBook.sheets[cSheet], it)) ? result = false : result = result;
			(!simanticMinMaxCSR(errBook.sheets[cSheet], it)) ? result = false : result = result;
			(!simanticBits(errBook.sheets[cSheet], it)) ? result = false : result = result;
			
			if (!result)	// Установка флага, что на листе есть ошибка
				book.sheets[cSheet].bErrorSheet = false;
		}
	}
}

// Проверка номера слова
bool CTest::simanticNumWord(errorSheetData& sheet, list<signalData>::iterator& it, bool wRep[])
{
	list <CString> error;

	if (it->bNumWordField)
	{
		if (it->iNumWord[0] > 32 || it->iNumWord[1] > 32)	// Слово должно быть не больше 32
			error.push_back(ErrorBase.sim.NumMore32);

		if (it->iNumWord[1] > 0)
			if (!wRep[it->iNumWord[1] - 1])	// Поиск совпадений № слов для второго слова
				error.push_back(ErrorBase.sim.NumRepite);
		
		if (!wRep[it->iNumWord[0] - 1])	// Для первого
			error.push_back(ErrorBase.sim.NumRepite);

		// Отметка о том, что эти слова на этом листе
		wRep[it->iNumWord[0] - 1] = false;
		if (it->iNumWord[1] > 0)
			wRep[it->iNumWord[1] - 1] = false;
	}

	if (error.empty())
		return true;

	errorSignalData signal = getErrSignal(it, error);
	sheet.signals.push_back(signal);
	it->bNumWordField = false;

	return false;
}

bool CTest::simanticTitleParam(errorSheetData& errSheet, list<signalData>::iterator& it)	// Прокинуть книгу и индекс листа
{
	list <CString> error;

	if (it->bTitleParamField)
	{
		if (findRepiteInSheet())
			error.push_back(ErrorBase.sim.TitleRepSheet);
		else if (findRepiteInBook())
			error.push_back(ErrorBase.sim.TitleRepBook);	// Замечание, а не ошибка 
	}

	if (error.empty())
		return true;

	errorSignalData signal = getErrSignal(it, error);
	errSheet.signals.push_back(signal);
	it->bTitleParamField = false;

	return false;
}

// Проверка минимального, максимального и цср
bool CTest::simanticMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it)
{

}

// Проверка используемых разрядов
bool CTest::simanticBits(errorSheetData& sheet, list<signalData>::iterator& it)
{

}

// Перевод из строки в числа
void CTest::translateNumWord(list<signalData>::iterator& it)
{
	CString num = it->sNumWordField;
	int indxDot = num.Find(_T(","));

	if (indxDot == -1)
	{
		num.Trim();	// Удаление пробелов
		it->iNumWord[0] = _wtoi(num);
	}
	else
	{
		CString num2 = num;	// Второе число
		num2.Delete(indxDot, num.GetLength() - indxDot);
		num2.Trim();
		it->iNumWord[1] = _wtoi(num2);

		num.Delete(0, indxDot);	// Первое число
		num.Trim();
		it->iNumWord[0] = _wtoi(num);
	}
}

#pragma endregion

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

// Функция преобразования CString в string
string CTest::convertString(CString cStr)
{
	int bytes = WideCharToMultiByte(CP_ACP, 0, cStr.GetBuffer(), -1, 0, 0, NULL, NULL);
	char *buffer = new char[bytes];

	WideCharToMultiByte(CP_ACP, 0, cStr.GetBuffer(), -1, buffer, bytes, NULL, NULL);

	string result(buffer);

	delete buffer;
	buffer = nullptr;

	return result;
}