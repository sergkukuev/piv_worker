#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>

#define ERROR_DATA _T("�� ������� ��������� �������� %d-�� %s. (���� �������� ������������ ������� ��� ������ �����������)")
#define RESERVE_SIGNAL _T("������")

// ����� ������������� ������
// ��� ������ � ����� "� �����"
CONST CString NumRepite = TEXT("\t ����� � ����� ������� ����������� ����� �� �����");
CONST CString NumMore32 = TEXT("\t ����� ����� ������ 32");

// ��� ������ � ����� "����������� �������"
CONST CString TitleRepBook = TEXT("������ � ����� ������������ ����������� � �����");
CONST CString TitleRepSheet = TEXT("������ � ����� ������������ ����������� �� ���� �����");

// ��� ������ � ������ "����������� ��������", "������������ ��������"
CONST CString MMCNotNote = TEXT("�������� �� ������������� �������� � ����������");
CONST CString MMCNotPkg = TEXT("������ ��������� ������ ��������");
CONST CString MMCNotNegative = TEXT("�� ����� ���� ������������� ������ ��� �� ����� ����� �������� ��� � ���� \"����������\"");
CONST CString MMCNegative = TEXT("������ ���� ������������� ������ ��� �� ����� ����� �������� ��� � ���� \"����������\"");

// ��� ������ � ����� "������������ ����"
CONST CString BitsNotSetSign = TEXT("�� ������� �������� ���");
CONST CString BitsOneInter = TEXT("������ ���� ���� ����������");
CONST CString BitsTwoInter = TEXT("������ ���� ��� ����������");
CONST CString BitsCross = TEXT("���(�) �����������(��)��");

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
	CTest();	// �����������
	~CTest();	// ����������

	list <errorSet> Start(list <bookData>& books);	// �������� �� ��� ������

private:
	void getErrors(sheetData* sheet, vector <errorSignal>& syntax, vector <errorSignal>& simantic); // �������� �� ������
	void getWarning(sheetData* sheet, vector <errorSignal>& warning);		// �������� �� ���������
	
	// �������������� ������
	bool syntaxValue(signalData& signal, vector <CString>& error, fieldError& errValue);
	int checkValue(vector <int> numeric);
	bool syntaxTitle(CString title, vector <CString>& error);

	bool simanticNumWord(vector <int> numeric, vector <CString>& error, bool correct, bool repit[]);		// �������� ������ �����
	bool simanticTitle(sheetData* sheet, CString title, vector <CString>& error, bool err);	// �������� ������������ �������
	bool simanticValue();	// �������� ������������, ������������� � ���
	bool simanticBits(signalData& signal, vector <CString>& error, fieldError errValue, bool repit[][32]);		// �������� ������������ ��������

	bool checkCrossBits(vector <int>& bits, vector <int>& numWord, bool repiter[][32]); // ���������� �����
	bool findRepiteInSheet(CString field, sheetData* sheet)

	/*list <CString> testField(CString field, errorData errStruct);	// �������� ���� �� ������
	errorSignalData getErrSignal(list<signalData>::iterator it, list <CString> error);	// �������� ������ ������ �������
	
	// ��������������
	bool syntaxNumWord(errorSheetData& sheet, list<signalData>::iterator& it);		// �������� ������ �����
	bool syntaxTitleParam(errorSheetData& sheet, list<signalData>::iterator& it);	// �������� ������������ �������
	bool syntaxMinMaxCSR(errorSheetData& sheet, list<signalData>::iterator& it);	// �������� ������������, ������������� � ���
	bool syntaxBits(errorSheetData& sheet, list<signalData>::iterator& it);			// �������� ������������ ��������
	bool syntaxComment(errorSheetData& sheet, list<signalData>::iterator& it, bool begin);		// �������� �����������

	// �������������
	

	bool findRepiteInBook(CString field, bookData book);	// ����� ���������� � �����
	bool findRepiteInSheet(CString field, sheetData sheet);	// ����� ���������� � �����
	bool checkCrossBits(list<signalData>::iterator& it, bool repiter[][32]);	// �������� �� ���������� �����
	void translateNumWord(list<signalData>::iterator& it);	// ������� � ����� �� ������ � �����
	void translateMMC(list<signalData>::iterator& it);		// ������� ���, ���� � ���
	double stepTranslateMMC(CString value);					// �������������� ������� ��� �������� ��������
	void translateBits(list<signalData>::iterator& it);		// ������� ������������ �������� �� ������ � �����
	vector <int> stepTranslateBits(CString bits);			// �������������� ������� ��� �������� ��������
	void translateComment(list<signalData>::iterator& it);	// ������� �������� ����� ��� NP ������ �� ����������
	int stepTranslateComment(CString field, int indx);		// �������������� ������� ��� �������� ����� ��� NP*/
};

/*
// ��������� ���������� ���������
SetCorrect(NumWord, "^[0-9]+(,[ \t]?[0-9]*)?$");

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
AddError(Comment, "��-[^0-9]+", _T("�������� ��������� ������� � ���� ����������."));*/
