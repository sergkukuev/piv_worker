#pragma once

#include <cstdlib>
#include <cerrno>

#include "StructPIV.h"
#include "WorkExcel.h"
#include "MyException.h"

// ���������� � ���������� � ARINC ����������
struct arincData 
{
	CString symbol;		// ������ ������ � ��������������
	int current = 0;	// ������� ����������
	int amount = 0;		// ����� ����������
	int startRow;		// ������ ����� ����������
	bool flag = true;	// ���� ���������� ����������
};

// ����� ��� ������ ���������� �� Excel
class PIV_DECLARE CReaderExcel	
{
public:
	CReaderExcel();		// �����������
	~CReaderExcel();	// ����������

	bookData GetBook(const CString& pathToExcel);	// ������ ���������
private:
	vector <CString> extension;	// ���������� ���������� ������
	Header header;				// ���������� � ����������

	void GetSheets(vector <sheetData>& book, CWorkExcel& work);								// ������ ������ ���������
	void GetSignals(vector <signalData>& signals, CWorkExcel& work, const bool& isArinc);	// ������ ���������� �� �����
	void GetArinc(const CString& field, const long& row, arincData& arinc);					// ������ ������ ���������� � ARINC ��������� (���������� ����� � �����)

	// ������ ����������:
	intData GetNumWord(const CString& field);					// ����� �����
	intData GetBits(const CString& bits, const int& size);		// ������������ �������
	intData GetAdress(const CString& field, const int& num);	// �����
	void GetMinMaxCsr(signalData& signal, CWorkExcel& work, const long& row);				// ���, ���� � ���
	CString GetComment(CWorkExcel& work, const long& row, const int& size, bool& flag);		// ����������
	
	int GetSubIndex(CString& numeric);							// ��������� ������������ ������� ������ � �������� ��� �� ������
	vector <int> StepAdress(const CString& adress, bool& flag);	// ��� ������� ��� ������
	vector <int> StepBits(const CString& bits, bool& flag);		// ��� ������� ��� ������������ ��������
	
	double GetDouble(const CString& field, bool& flag);	// ��������� �������� double (����� = 0)
	int GetInt(const CString& field, bool& flag);		// ��������� �������� int (����� = 0)

	bool IsTitle(CWorkExcel& work, const long& row);	// �������� ������ �� ���������
	bool IsEmpty(CWorkExcel& work, const long& row);	// �������� ������ �� �������
	bool IsRemark(CWorkExcel& work, const long& row);	// �������� ������ �� ������� ����������
	bool CheckExtension(const CString& path);			// �������� ���������� ������
};

