#pragma once

#include "StructPIV.h"
#include "WorkExcel.h"
#include "MyException.h"

#include <cstdlib>
#include <cerrno>

#define SIGN_FIELD _T("��-4")

// ����� ��� ������ ���������� �� excel ������
class CReaderExcel	{
public:
	CReaderExcel();		// �����������
	~CReaderExcel();	// ����������

	bookData getBook(const CString& pathToExcel);	// ������ ����� �����
private:
	vector <CString> extension;	// ���������� ���������� ������
	Header header;				// ���������� � ����������

	void getSheets(vector <sheetData>& book, CWorkExcel& work);			// ������ ������ �� �����
	void getSignals(vector <signalData>& signals, CWorkExcel& work);	// ������ ���������� �� �����

	vector <int> getNumWord(const CString& field, bool& flag);			// �������� ������ ���� �� ������ � �����
	vector <int> getBits(const CString& bits, bool& flag);				// �������� ������������ �������
	vector <int> stepGetBits(const CString& bits, bool& flag);			// ��� ������� ��� ��������
	void getMinMaxCsr(signalData& signal, CWorkExcel& work, const long& row);				// �������� �������� ���, ���� � ���
	CString getComment(CWorkExcel& work, const long& row, const int& size, bool& flag);		// ������ ����������

	double getDouble(const CString& field, bool& flag);	// ��������� �������� double (���� ����)
	int getInt(const CString& field, bool& flag);		// ��������� �������� int (���� ����)


	bool isEmpty(CWorkExcel& work, const long& row);	// �������� ������ �� �������
	bool isRemark(CWorkExcel& work, const long& row);	// �������� ������ �� ������� ����������
	bool checkExtension(const CString& path);			// �������� ���������� ������
};

