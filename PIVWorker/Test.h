#pragma once

#include "StructPIV.h"
#include "MyException.h"

#include <regex>

#define ERROR_DATA _T("�� ������� ��������� �������� %s. (���� �������� ������������ ������� ��� ������ '.' � ',')")
#define RESERVE_SIGNAL _T("������")
#define MAX_NUMWORD 32
#define MAX_BITS 32

class CTest {
public:
	CTest();	// �����������
	~CTest();	// ����������

	list <errorSet> Start(list <bookData>& books);	// �������� �� ��� ������

private:
	void getErrors(sheetData* sheet, vector <errorSignal>& syntax, vector <errorSignal>& simantic); // �������� �� ������
	void getWarnings(sheetData* sheet, vector <errorSignal>& warning);		// �������� �� ���������

	void checkNP(signalData& signal, const int& np, vector <errorSignal>& syntax);	// �������� ������, ��������� � ������� ������
	void initRepiter(bool* num, bool** bits);	// ������������� ���������
	
	// �������������� ������
	void syntaxValue(const convertError& flags, vector <CString>& error);	// �������� �������� ����������
	bool syntaxTitle(const CString& title, vector <CString>& error);		// �������� ���������� ��������������

	void checkValueByFlag(const CString& field, const int& indx, const bool& flag, vector <CString>& error); // �������� �������� ���������� �� ������ ������

	// ������������� ������
	void simanticNumWord(const vector <int>& numeric, const bool& flag, bool* repiter, vector <CString>& error);		// �������� ������ �����
	void simanticTitle(sheetData* sheet, const CString& title, const bool& flag, vector <CString>& error);				// �������� ������������ �������
	void simanticValue(const signalData& signal, vector <CString>& error);						// �������� ������������, ������������� � ���
	void simanticBits(const signalData& signal, bool** repiter, vector <CString>& error);		// �������� ������������ ��������
	
	bool checkCrossBits(const vector <int>& bits, const vector <int>& numWord, bool** repiter); // �������� ���������� �����
	bool findRepiteInSheet(const CString& field, sheetData* sheet);

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
