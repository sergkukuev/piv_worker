#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>

#define ERROR_DATA _T("Не удалось прочитать значение %d-го %s. (Поле содержит недопустимые символы или лишний разделитель)")
#define RESERVE_SIGNAL _T("Резерв")

// Набор семантических ошибок
// При работе с полем "№ слова"
CONST CString NumRepite = TEXT("\t Слово с таким номером встречалось ранее на листе");
CONST CString NumMore32 = TEXT("\t Номер слова больше 32");

// При работе с полем "Обозначение сигнала"
CONST CString TitleRepBook = TEXT("Сигнал с таким обозначением присутсвует в книге");
CONST CString TitleRepSheet = TEXT("Сигнал с таким обозначением присутсвует на этом листе");

// При работе с полями "Минимальное значение", "Максимальное значение"
CONST CString MMCNotNote = TEXT("Значение не соответствует значению в примечании");
CONST CString MMCNotPkg = TEXT("Нельзя упаковать данное значение");
CONST CString MMCNotNegative = TEXT("Не может быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\"");
CONST CString MMCNegative = TEXT("Должно быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\"");

// При работе с полем "Используемые биты"
CONST CString BitsNotSetSign = TEXT("Не включен знаковый бит");
CONST CString BitsOneInter = TEXT("Должен быть один промежуток");
CONST CString BitsTwoInter = TEXT("Должно быть два промежутка");
CONST CString BitsCross = TEXT("Бит(ы) перекрывает(ют)ся");

struct fieldError {
	bool numWord = false;
	bool title = false;
	bool min = false;
	bool max = false;
	bool csr = false;
	bool bit = false;
};

class CTest {
public:
	CTest();	// Конструктор
	~CTest();	// Деструктор

	list <errorSet> Start(list <bookData>& books);	// Проверка на все ошибки

private:
	void getErrors(sheetData* sheet, vector <errorSignal>& syntax, vector <errorSignal>& simantic); // Проверка на ошибки
	void getWarning(sheetData* sheet, vector <errorSignal>& warning);		// Проверка на замечания
	
	// Синтаксический анализ
	bool syntaxValue(signalData& signal, vector <CString>& error, fieldError& errValue);
	int checkValue(vector <int> numeric);
	bool syntaxTitle(CString title, vector <CString>& error);

	bool simanticNumWord(vector <int> numeric, vector <CString>& error, bool correct, bool repit[]);		// Проверка номера слова
	bool simanticTitle(sheetData* sheet, CString title, vector <CString>& error, bool err);	// Проверка наименований сигнала
	bool simanticValue();	// Проверка минимального, максимального и цср
	bool simanticBits(signalData& signal, vector <CString>& error, fieldError errValue, bool repit[][32]);		// Проверка используемых разрядов

	bool checkCrossBits(vector <int>& bits, vector <int>& numWord, bool repiter[][32]); // Перекрытие битов
	bool findRepiteInSheet(CString field, sheetData* sheet)

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
