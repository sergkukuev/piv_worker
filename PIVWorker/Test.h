#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>

#define ERROR_DATA _T("Не удалось прочитать значение %s. (Поле содержит недопустимые символы или лишние '.' и ',')")
#define RESERVE_SIGNAL _T("Резерв")
#define MAX_NUMWORD 32
#define MAX_BITS 32

class CTest {
public:
	CTest();	// Конструктор
	~CTest();	// Деструктор

	list <errorSet> Start(list <bookData>& books);	// Проверка на все ошибки

private:
	void getErrors(sheetData* sheet, vector <errorSignal>& syntax, vector <errorSignal>& simantic); // Проверка на ошибки
	void getWarnings(sheetData* sheet, vector <errorSignal>& warning);		// Проверка на замечания

	void checkNP(signalData& signal, const int& np, vector <errorSignal>& syntax);	// Проверка ошибок, связанных с номером набора
	void initRepiter(bool* num, bool** bits);	// Инициализация репитеров
	
	// Синтаксический анализ
	void syntaxValue(const convertError& flags, vector <CString>& error);	// Проверка числовых параметров
	bool syntaxTitle(const CString& title, vector <CString>& error);		// Проверка синтаксиса идентификатора

	void checkValueByFlag(const CString& field, const int& indx, const bool& flag, vector <CString>& error); // Проверка числовых параметров по набору флагов

	// Семантический анализ
	void simanticNumWord(const vector <int>& numeric, const bool& flag, bool* repiter, vector <CString>& error);		// Проверка номера слова
	void simanticTitle(sheetData* sheet, const CString& title, const bool& flag, vector <CString>& error);				// Проверка наименований сигнала
	void simanticValue(const signalData& signal, vector <CString>& error);						// Проверка минимального, максимального и цср
	void simanticBits(const signalData& signal, bool** repiter, vector <CString>& error);		// Проверка используемых разрядов
	
	bool checkCrossBits(const vector <int>& bits, const vector <int>& numWord, bool** repiter); // Проверка перекрытия битов
	bool findRepiteInSheet(const CString& field, sheetData* sheet);

	/*list <CString> testField(CString field, errorData errStruct);	// Проверка поля на ошибки
	errorSignalData getErrSignal(list<signalData>::iterator it, list <CString> error);	// Создание записи ошибки сигнала
	
	// Синтаксические
	bool syntaxNumWord(errorSheetData& sheet, list<signalData>::iterator& it);		// Проверка номера слова
	bool syntaxTitleParam(errorSheetData& sheet, list<signalData>::iterator& it);	// Проверка наименований сигнала
	bool syntaxMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it);	// Проверка минимального, максимального и цср
	bool syntaxBits(errorSheetData& sheet, list<signalData>::iterator& it);			// Проверка используемых разрядов
	bool syntaxComment(errorSheetData& sheet, list<signalData>::iterator& it, bool begin);		// Проверка коментариев

	// Семантические
	

	bool findRepiteInBook(CString field, bookData book);	// Поиск повторений в книге
	bool findRepiteInSheet(CString field, sheetData sheet);	// Поиск повторений в листе
	bool checkCrossBits(list<signalData>::iterator& it, bool repiter[][32]);	// Проверка на перекрытия битов
	void translateNumWord(list<signalData>::iterator& it);	// Перевод № слова из строки в числа
	void translateMMC(list<signalData>::iterator& it);		// Перевод мин, макс и цср
	double stepTranslateMMC(CString value);					// Дополнительная функция для перевода разрядов
	void translateBits(list<signalData>::iterator& it);		// Перевод используемых разрядов из строки в числа
	vector <int> stepTranslateBits(CString bits);			// Дополнительная функция для перевода разрядов
	void translateComment(list<signalData>::iterator& it);	// Перевод значения знака или NP набора из примечания
	int stepTranslateComment(CString field, int indx);		// Дополнительная функция для перевода знака или NP*/
};

/*
// Установка корректных регулярок
SetCorrect(NumWord, "^[0-9]+(,[ \t]?[0-9]*)?$");

SetCorrect(MinMaxCSR, "^-?[0-9]+,?[0-9]*$");
SetCorrect(Bits, "^[0-9]+((…|[.]{3})[0-9]+)?(,[ \t]?[0-9]+((…|[.]{3})[0-9]+)?)?$");

// Установка набор ошибок
// Для № слова
AddError(NumWord, "(^$)", _T("Значение в поле отсутствует."));
AddError(NumWord, "[^0-9, \t\n]+", _T("Поле содержит недопустимые символы."));
AddError(NumWord, "[0-9]+[ \t]+[0-9]*", _T("Поле содержит лишние пробелы. Допускается только после запятой."));
AddError(NumWord, "(,[ \t]?)+[0-9]*(,[ \t]?)+", _T("Значение в поле содержит более одной запятой."));
AddError(NumWord, "^,[ \t]?[0-9]+$", _T("Отсутствует значение до запятой."));
AddError(NumWord, "^[0-9]+,[ \t]?$", _T("Отсутствует значение после запятой."));
AddError(NumWord, "^,[ \t]?$", _T("Отсутствуют значения до и после запятой."));
//AddError(NumWord, "^([0-9]*[^0-9, \t\n]+[0-9]*)+,[ \t]?[0-9]+$", _T("Значение в поле содержит недопустимые символы до запятой."));	//??
//AddError(NumWord, "^[0-9]+,[ \t]?([0-9]*[^0-9 \t\n]+[0-9]*)+", _T("Значение в поле содержит недопустимые символы после запятой."));	//??

// Для обозначения сигнала


// Для мин, макс и цср
AddError(MinMaxCSR, "(^$)", _T("Значение в поле отсутствует."));
AddError(MinMaxCSR, "[^0-9, \t\n-]+", _T("Поле содержит недопустимые символы."));
AddError(MinMaxCSR, "[ \t\n]+", _T("Поле содержит пробел."));
AddError(MinMaxCSR, ",+[0-9-]*,+", _T("Значение в поле содержит более одной запятой."));	//??
AddError(MinMaxCSR, "-+[0-9,]*-+", _T("Значение в поле содержит больше одного знака '-'."));	//??
AddError(MinMaxCSR, "^[0-9,]+-", _T("Неправильная установка знака '-'. Он должен быть установлен в начале."));
AddError(MinMaxCSR, "^-?,[0-9]*", _T("Отсутствует значение до запятой."));
AddError(MinMaxCSR, "^-?[0-9]+,$", _T("Отсутствует значение после запятой."));
//AddError(MinMaxCSR, "^-?([0-9]*[^0-9]+[0-9]*),[0-9]+", _T("Значение в поле содержит недопустимые символы до запятой."));		// Нужны ли, если есть проверка на недопустимые символы
//AddError(MinMaxCSR, "^-?[0-9]+,([0-9]*[^0-9]+[0-9]*)", _T("Значение в поле содержит недопустимые символы после запятой."));		//??

// Для используемых битов
AddError(Bits, "(^$)", _T("Значение в поле отсутствует."));
AddError(Bits, "[^0-9., \t…]+", _T("Поле содержит недопустимые символы."));
AddError(Bits, "[0-9.…]*[ \t]+[0-9.…]*", _T("Поле содержит лишние пробелы. Допускается только после запятой."));
AddError(Bits, "[0-9]*[.]{1,2}[0-9]*", _T("Неверное обозначение промежутка."));
AddError(Bits, "^(…|[.]{3})[0-9]+", _T("Отсутствует значение в начале промежутка."));
AddError(Bits, "[0-9]+(…|[.]{3})$", _T("Отсутствует значение в конце промежутка."));

AddError(Bits, "(,[ \t]?)+[0-9.…]*(,[ \t]?)+", _T("Значение в поле содержит более одной запятой."));
//AddError(Bits, "", _T("Значение в поле содержит более двух промежутков."));			// ДОДЕЛАТЬ
AddError(Bits, "^[0-9]*[.]{1,2}[0-9]*,[ \t]?", _T("Неверное обозначение промежутка до запятой."));
AddError(Bits, "^(…|[.]{3})[0-9]+,[ \t]?", _T("Отсутствует значение в начале промежутка до запятой."));
AddError(Bits, "^[0-9]+(…|[.]{3}),[ \t]?", _T("Отсутствует значение в конце промежутка до запятой."));

AddError(Bits, ",[ \t]?([0-9]*[.]{1,2}[0-9]*)$", _T("Неверное обозначение промежутка после запятой."));
AddError(Bits, ",[ \t]?(…|[.]{3})[0-9]+$", _T("Отсутствует значение в начале промежутка после запятой."));
AddError(Bits, ",[ \t]?[0-9]+(…|[.]{3})$", _T("Отсутствует значение в конце промежутка после запятой."));

AddError(Bits, "[0-9]*[.]{1,2}[0-9]*,[ \t]?[0-9]*[.]{1,2}[0-9]*", _T("Неверное обозначение обоих промежутков."));

AddError(Comment, "NP=( )?[^0-9]+", _T("Нет номера набора параметров."));
AddError(Comment, "Зн-[^0-9]+", _T("Значение знакового разряда в поле нечисловое."));*/
