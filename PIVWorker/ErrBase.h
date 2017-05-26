#pragma once

#include <vector>
#include <regex>
#include <cstring>

using namespace std;

// ��������� ������ ������
struct errorData
{
	regex correct;					// ���������� ���������
	vector <regex> error;			// ����� ��������� ���������
	vector <CString> description;	// �������� ������
};

// ����� ������ ������ ��� ���� ����� ���
class CErrBase 
{
public:
	CErrBase();		// �����������
	~CErrBase();	// ����������

	errorData getNumWord();		// �������� ����� ��� � �����
	errorData getTitleParam();	// �������� ����� ��� ��������������
	errorData getMinMaxCSR();	// �������� ������ ��� ���, ���� � ���
	errorData getBits();		// �������� ����� ��� ������������ �����

private:
	errorData NumWord;		// ����� ������ ��� � ����� 
	errorData TitleParam;	// ����� ������ ��� ��������������
	errorData MinMaxCSR;	// ����� ������ ��� ���, ����, ���
	errorData Bits;			// ����� ������ ��� ������������ �����

	void SetCorrect(errorData& err, string reg);		// ��������� ��������� ��� ����������� ����
	void AddError(errorData& err, string reg, CString msg);	// ���������� ������ � ������������� ������
	void Clear(errorData& err);	// ������� ����� ������
};

