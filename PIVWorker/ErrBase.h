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

// ����� ������������� ������
struct simantic
{
	// ��� ������ � ����� "� �����"
	CONST CString NumRepite = TEXT("\t ����� � ����� ������� ����������� ����� �� �����.");
	CONST CString NumMore32 = TEXT("\t ����� ����� ������ 32.");

	// ��� ������ � ����� "����������� �������"
	CONST CString TitleRepBook = TEXT("������ � ����� ������������ ����������� � �����.");
	CONST CString TitleRepSheet = TEXT("������ � ����� ������������ ����������� �� ���� �����.");

	// ��� ������ � ������ "����������� ��������", "������������ ��������"
	CONST CString MMCNotNote = TEXT("�������� �� ������������� �������� � ����������.");
	CONST CString MMCNotPkg = TEXT("������ ��������� ������ ��������.");
	CONST CString MMCNotNegative = TEXT("�� ����� ���� ������������� ������ ��� �� ����� ����� �������� ��� � ���� \"����������\".");
	CONST CString MMCNegative = TEXT("������ ���� ������������� ������ ��� �� ����� ����� �������� ��� � ���� \"����������\".");

	// ��� ������ � ����� "������������ ����"
	CONST CString BitsNotSetSign = TEXT("�� ������� �������� ���.");
	CONST CString BitsOneInter = TEXT("������ ���� ���� ����������.");
	CONST CString BitsTwoInter = TEXT("������ ���� ��� ����������.");
};

// ����� ������ ������ ��� ���� ����� ���
class CErrBase 
{
public:
	CErrBase();		// �����������
	~CErrBase();	// ����������

	simantic sim;			// ����� ������������� ������

	// �������������� ������ ���������
	errorData getNumWord();		// �������� ����� ��� � �����
	errorData getTitleParam();	// �������� ����� ��� ��������������
	errorData getMinMaxCSR();	// �������� ������ ��� ���, ���� � ���
	errorData getBits();		// �������� ����� ��� ������������ �����
	errorData getComment();		// �������� ����� ��� ������������
private:
	// ��������������
	errorData NumWord;		// ����� ������ ��� � ����� 
	errorData TitleParam;	// ����� ������ ��� ��������������
	errorData MinMaxCSR;	// ����� ������ ��� ���, ����, ���
	errorData Bits;			// ����� ������ ��� ������������ �����
	errorData Comment;		// ����� ������ ��� ������������

	void SetCorrect(errorData& err, string reg);		// ��������� ��������� ��� ����������� ����
	void AddError(errorData& err, string reg, CString msg);	// ���������� ������ � ������������� ������
	void Clear(errorData& err);	// ������� ����� ������
};

