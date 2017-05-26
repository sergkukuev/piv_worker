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

// Класс набора ошибок для всех полей ПИВ
class CErrBase 
{
public:
	CErrBase();		// Конструктор
	~CErrBase();	// Деструктор

	errorData getNumWord();		// Получить набор для № слова
	errorData getTitleParam();	// Получить набор для идентификатора
	errorData getMinMaxCSR();	// Получить набора для мин, макс и цср
	errorData getBits();		// Получить набор для используемых битов

private:
	errorData NumWord;		// Набор ошибок для № слова 
	errorData TitleParam;	// набор ошибок для идентификатора
	errorData MinMaxCSR;	// Набор ошибок для мин, макс, цср
	errorData Bits;			// Набор ошибок для используемых битов

	void SetCorrect(errorData& err, string reg);		// Установка регулярки для корректного поля
	void AddError(errorData& err, string reg, CString msg);	// Добавление ошибки к определенному набору
	void Clear(errorData& err);	// Очистка набор ошибок
};

