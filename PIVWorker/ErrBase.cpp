#include "stdafx.h"
#include "ErrBase.h"

// Конструктор
CErrBase::CErrBase()
{
	// Установка корректных регулярок
	SetCorrect(NumWord, "[0-9]+[,]?[0-9]*$");
	SetCorrect(TitleParam, "^_*[A-Za-z]{1}[A-Za-z0-9_]*$");
	SetCorrect(MinMaxCSR, "^-?[0-9]+[,]?[0-9]*$");
	SetCorrect(Bits, "[0-9]+[…]?[0-9]*$");
	SetCorrect(Comment, "^_+");

	// Установка набор ошибок
}

// Деструктор
CErrBase::~CErrBase()
{
	Clear(NumWord);
	Clear(TitleParam);
	Clear(MinMaxCSR);
	Clear(Bits);
	Clear(Comment);
}

// Установка регулярки для корректного поля
void CErrBase::SetCorrect(errorData& err, string reg)
{
	regex temp(reg);

	err.correct = temp;
}

// Добавление ошибки к определенному набору
void CErrBase::AddError(errorData& err, string reg, CString msg)
{
	regex temp(reg);

	err.error.push_back(temp);
	err.description.push_back(msg);
}

// Очистка набор ошибок
void CErrBase::Clear(errorData& err)
{
	err.error.clear();
	err.description.clear();
}

// Получить набор для № слова
errorData CErrBase::getNumWord()
{
	return NumWord;
}

// Получить набор для идентификатора
errorData CErrBase::getTitleParam()
{
	return TitleParam;
}

// Получить набора для мин, макс и цср
errorData CErrBase::getMinMaxCSR()
{
	return MinMaxCSR;
}

// Получить набор для используемых битов
errorData CErrBase::getBits()
{
	return Bits;
}

// Получить набор для комментариев
errorData CErrBase::getComment()
{
	return Comment;
}