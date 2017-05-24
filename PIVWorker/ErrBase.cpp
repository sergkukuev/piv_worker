#include "stdafx.h"
#include "ErrBase.h"

// �����������
CErrBase::CErrBase()
{
	// ��������� ���������� ���������
	SetCorrect(NumWord, "[0-9]+[,]?[0-9]*$");
	SetCorrect(TitleParam, "^_*[A-Za-z]{1}[A-Za-z0-9_]*$");
	SetCorrect(MinMaxCSR, "^-?[0-9]+[,]?[0-9]*$");
	SetCorrect(Bits, "[0-9]+[�]?[0-9]*$");
	SetCorrect(Comment, "^_+");

	// ��������� ����� ������
}

// ����������
CErrBase::~CErrBase()
{
	Clear(NumWord);
	Clear(TitleParam);
	Clear(MinMaxCSR);
	Clear(Bits);
	Clear(Comment);
}

// ��������� ��������� ��� ����������� ����
void CErrBase::SetCorrect(errorData& err, string reg)
{
	regex temp(reg);

	err.correct = temp;
}

// ���������� ������ � ������������� ������
void CErrBase::AddError(errorData& err, string reg, CString msg)
{
	regex temp(reg);

	err.error.push_back(temp);
	err.description.push_back(msg);
}

// ������� ����� ������
void CErrBase::Clear(errorData& err)
{
	err.error.clear();
	err.description.clear();
}

// �������� ����� ��� � �����
errorData CErrBase::getNumWord()
{
	return NumWord;
}

// �������� ����� ��� ��������������
errorData CErrBase::getTitleParam()
{
	return TitleParam;
}

// �������� ������ ��� ���, ���� � ���
errorData CErrBase::getMinMaxCSR()
{
	return MinMaxCSR;
}

// �������� ����� ��� ������������ �����
errorData CErrBase::getBits()
{
	return Bits;
}

// �������� ����� ��� ������������
errorData CErrBase::getComment()
{
	return Comment;
}