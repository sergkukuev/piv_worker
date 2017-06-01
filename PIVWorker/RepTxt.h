#pragma once

#include "StructPIV.h"	// ��������� ���
#include <fstream>

// ������� (��������� � ��������)
#define signal_set(iBook, iSheet) books[iBook].sheets[iSheet].signals

// ���������� �� �������� ���������
struct sheetInfo
{
	int NP;			// ����� ������
	int NumPk;		// ����� ��������
	bool bNumPk;	// ����� �� ������������� ����� ��������
};

// ����� ��������� txt ������
class CRepTxt
{
public:
	CRepTxt() {};	// ����������
	~CRepTxt() {};	// ����������

	void Generate(vector <bookData> books, CString pathToSave, bool bNumPK);	// ������ ��������� txt ������
private:
	void WriteParameter(ofstream& file, list <signalData>::iterator it, sheetInfo info);	// ������ ������� � txt ����
};

