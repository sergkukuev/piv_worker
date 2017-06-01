#include "stdafx.h"
#include "ErrBase.h"

// �����������
CErrBase::CErrBase()
{
	// ��������� ���������� ���������
	SetCorrect(NumWord, "^[0-9]+(,[ \t]?[0-9]*)?$");
	SetCorrect(TitleParam, "^[A-Za-z][A-Za-z0-9_]*$");
	SetCorrect(MinMaxCSR, "^-?[0-9]+,?[0-9]*$");
	SetCorrect(Bits, "^[0-9]+((�|[.]{3})[0-9]+)?(,[ \t]?[0-9]+((�|[.]{3})[0-9]+)?)?$");

	// ��������� ����� ������
	// ��� � �����
	AddError(NumWord, "(^$)", _T("�������� � ���� �����������."));
	AddError(NumWord, "[^0-9, \t\n]+", _T("���� �������� ������������ �������."));
	AddError(NumWord, "[0-9]+[ \t]+[0-9]*", _T("���� �������� ������ �������. ����������� ������ ����� �������."));
	AddError(NumWord, "(,[ \t]?)+[0-9]*(,[ \t]?)+", _T("�������� � ���� �������� ����� ����� �������."));
	AddError(NumWord, "^,[ \t]?[0-9]+$", _T("����������� �������� �� �������."));
	AddError(NumWord, "^[0-9]+,[ \t]?$", _T("����������� �������� ����� �������."));
	AddError(NumWord, "^,[ \t]?$", _T("����������� �������� �� � ����� �������."));
	//AddError(NumWord, "^([0-9]*[^0-9, \t\n]+[0-9]*)+,[ \t]?[0-9]+$", _T("�������� � ���� �������� ������������ ������� �� �������."));	//??
	//AddError(NumWord, "^[0-9]+,[ \t]?([0-9]*[^0-9 \t\n]+[0-9]*)+", _T("�������� � ���� �������� ������������ ������� ����� �������."));	//??

	// ��� ����������� �������
	AddError(TitleParam, "(^$)", _T("�������� � ���� �����������. (�������� ������ ��������������, �� \"������\" �� ��������)"));
	AddError(TitleParam, "[ \t\n]+", _T("�������� � ���� �������� ������."));
	AddError(TitleParam, "^[^A-Za-z][A-Za-z0-9_]*$", _T("�������� � ���� ���������� �� � ��������� �����."));
	AddError(TitleParam, "([A-Za-z0-9_]*[�-��-�]+[A-Za-z0-9_]*)+", _T("�������� � ���� �������� ���������."));
	AddError(TitleParam, "_$", _T("�������� � ���� ������������� �� '_'"));

	// ��� ���, ���� � ���
	AddError(MinMaxCSR, "(^$)", _T("�������� � ���� �����������."));
	AddError(MinMaxCSR, "[^0-9, \t\n-]+", _T("���� �������� ������������ �������."));
	AddError(MinMaxCSR, "[ \t\n]+", _T("���� �������� ������."));
	AddError(MinMaxCSR, ",+[0-9-]*,+", _T("�������� � ���� �������� ����� ����� �������."));	//??
	AddError(MinMaxCSR, "-+[0-9,]*-+", _T("�������� � ���� �������� ������ ������ ����� '-'."));	//??
	AddError(MinMaxCSR, "^[0-9,]+-", _T("������������ ��������� ����� '-'. �� ������ ���� ���������� � ������."));
	AddError(MinMaxCSR, "^-?,[0-9]*", _T("����������� �������� �� �������."));
	AddError(MinMaxCSR, "^-?[0-9]+,$", _T("����������� �������� ����� �������."));		
	//AddError(MinMaxCSR, "^-?([0-9]*[^0-9]+[0-9]*),[0-9]+", _T("�������� � ���� �������� ������������ ������� �� �������."));		// ����� ��, ���� ���� �������� �� ������������ �������
	//AddError(MinMaxCSR, "^-?[0-9]+,([0-9]*[^0-9]+[0-9]*)", _T("�������� � ���� �������� ������������ ������� ����� �������."));		//??

	// ��� ������������ �����
	AddError(Bits, "(^$)", _T("�������� � ���� �����������."));
	AddError(Bits, "[^0-9., \t�]+", _T("���� �������� ������������ �������."));
	AddError(Bits, "[0-9.�]*[ \t]+[0-9.�]*", _T("���� �������� ������ �������. ����������� ������ ����� �������."));
	AddError(Bits, "[0-9]*[.]{1,2}[0-9]*", _T("�������� ����������� ����������."));
	AddError(Bits, "^(�|[.]{3})[0-9]+", _T("����������� �������� � ������ ����������."));
	AddError(Bits, "[0-9]+(�|[.]{3})$", _T("����������� �������� � ����� ����������."));

	AddError(Bits, "(,[ \t]?)+[0-9.�]*(,[ \t]?)+", _T("�������� � ���� �������� ����� ����� �������."));
	//AddError(Bits, "", _T("�������� � ���� �������� ����� ���� �����������."));			// �������� 
	AddError(Bits, "^[0-9]*[.]{1,2}[0-9]*,[ \t]?", _T("�������� ����������� ���������� �� �������."));
	AddError(Bits, "^(�|[.]{3})[0-9]+,[ \t]?", _T("����������� �������� � ������ ���������� �� �������."));
	AddError(Bits, "^[0-9]+(�|[.]{3}),[ \t]?", _T("����������� �������� � ����� ���������� �� �������."));

	AddError(Bits, ",[ \t]?([0-9]*[.]{1,2}[0-9]*)$", _T("�������� ����������� ���������� ����� �������."));
	AddError(Bits, ",[ \t]?(�|[.]{3})[0-9]+$", _T("����������� �������� � ������ ���������� ����� �������."));
	AddError(Bits, ",[ \t]?[0-9]+(�|[.]{3})$", _T("����������� �������� � ����� ���������� ����� �������."));

	AddError(Bits, "[0-9]*[.]{1,2}[0-9]*,[ \t]?[0-9]*[.]{1,2}[0-9]*", _T("�������� ����������� ����� �����������."));

	AddError(Comment, "NP=( )?[^0-9]+", _T("��� ������ ������ ����������."));
	AddError(Comment, "��-[^0-9]+", _T("�������� ��������� ������� � ���� ����������."));
}

// ����������
CErrBase::~CErrBase()
{
	Clear(NumWord);
	Clear(TitleParam);
	Clear(MinMaxCSR);
	Clear(Bits);
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