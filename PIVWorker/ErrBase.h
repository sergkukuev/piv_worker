#pragma once

#include <vector>
#include <regex>
#include <cstring>

using namespace std;

// Структура набора ошибок
struct errorData
{
	regex correct;					// Корректная регулярка
	vector <regex> error;			// Набор ошибочных регулярок
	vector <CString> description;	// Описание ошибок
};

// Набор семантических ошибок
struct simantic
{
	// При работе с полем "№ слова"
	CONST CString NumRepite = TEXT("\t Слово с таким номером встречалось ранее на листе.");
	CONST CString NumMore32 = TEXT("\t Номер слова больше 32.");

	// При работе с полем "Обозначение сигнала"
	CONST CString TitleRepBook = TEXT("Сигнал с таким обозначением присутсвует в книге.");
	CONST CString TitleRepSheet = TEXT("Сигнал с таким обозначением присутсвует на этом листе.");

	// При работе с полями "Минимальное значение", "Максимальное значение"
	CONST CString MMCNotNote = TEXT("Значение не соответствует значению в примечании.");
	CONST CString MMCNotPkg = TEXT("Нельзя упаковать данное значение.");
	CONST CString MMCNotNegative = TEXT("Не может быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\".");
	CONST CString MMCNegative = TEXT("Должно быть отрицательным числом или не верно задан знаковый бит в поле \"Примечание\".");

	// При работе с полем "Используемые биты"
	CONST CString BitsNotSetSign = TEXT("Не включен знаковый бит.");
	CONST CString BitsOneInter = TEXT("Должен быть один промежуток.");
	CONST CString BitsTwoInter = TEXT("Должно быть два промежутка.");
};

// Класс набора ошибок для всех полей ПИВ
class CErrBase 
{
public:
	CErrBase();		// Конструктор
	~CErrBase();	// Деструктор

	simantic sim;			// Набор семантических ошибок

	// Синтаксические наборы регулярок
	errorData getNumWord();		// Получить набор для № слова
	errorData getTitleParam();	// Получить набор для идентификатора
	errorData getMinMaxCSR();	// Получить набора для мин, макс и цср
	errorData getBits();		// Получить набор для используемых битов
	errorData getComment();		// Получить набор для комментариев
private:
	// Синтаксические
	errorData NumWord;		// Набор ошибок для № слова 
	errorData TitleParam;	// набор ошибок для идентификатора
	errorData MinMaxCSR;	// Набор ошибок для мин, макс, цср
	errorData Bits;			// Набор ошибок для используемых битов
	errorData Comment;		// Набор ошибок для комментариев

	void SetCorrect(errorData& err, string reg);		// Установка регулярки для корректного поля
	void AddError(errorData& err, string reg, CString msg);	// Добавление ошибки к определенному набору
	void Clear(errorData& err);	// Очистка набор ошибок
};

