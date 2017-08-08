#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"
#include "MyException.h"

#include <cstdlib>
#include <cerrno>

#define SIGN_FIELD L"��-4"		// ����������� �������������� ������� � ����

// ���������� � ���������� � arinc ����������
struct arincData {
	CString symbol;		// ������ ������ � ��������������
	int current = 0;	// ������� ����������
	int amount = 0;		// ����� ����������
	int startRow;		// ������ ����� ����������
	bool flag = true;	// ���� ���������� ����������
};

// ����� ��� ������ ���������� �� excel ������
class PIV_DECLARE CReaderExcel	{
public:
	CReaderExcel();		// �����������
	~CReaderExcel();	// ����������

	bookData getBook(const CString& pathToExcel);	// ������ ����� �����
private:
	vector <CString> extension;	// ���������� ���������� ������
	Header header;				// ���������� � ����������

	void getSheets(vector <sheetData>& book, CWorkExcel& work);								// ������ ������ �� �����
	void getSignals(vector <signalData>& signals, CWorkExcel& work, const bool& isArinc);	// ������ ���������� �� �����
	void getArinc(const CString& field, const long& row, arincData& arinc);		// ������ ������ ���������� � arinc ���������(���������� ����� � �����)

	intData getNumWord(const CString& field);					// �������� ������ ���� �� ������ � �����
	intData getBits(const CString& bits);						// �������� ������������ �������
	intData getAdress(const CString& field, const int& num);	// �������� ����� �� ������ � �����
	void getMinMaxCsr(signalData& signal, CWorkExcel& work, const long& row);				// �������� �������� ���, ���� � ���
	CString getComment(CWorkExcel& work, const long& row, const int& size, bool& flag);		// ������ ����������
	
	vector <int> stepGetAdress(const CString& adress, bool& flag);	// ��� ������� ��� ������
	vector <int> stepGetBits(const CString& bits, bool& flag);		// ��� ������� ��� ��������
	
	double getDouble(const CString& field, bool& flag);	// ��������� �������� double (���� ����)
	int getInt(const CString& field, bool& flag);		// ��������� �������� int (���� ����)

	int getSubIndex(CString& numeric);					// ��������� ������������ ������� � �������� ��� �� ������
	bool isTitle(CWorkExcel& work, const long& row);	// �������� ������ �� ���������
	bool isEmpty(CWorkExcel& work, const long& row);	// �������� ������ �� �������
	bool isRemark(CWorkExcel& work, const long& row);	// �������� ������ �� ������� ����������
	bool checkExtension(const CString& path);			// �������� ���������� ������
};

