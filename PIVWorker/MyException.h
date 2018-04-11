#pragma once

#include "StructPIV.h"
#include "DllDeclare.h"

// ������� ����� ����������
class PIV_DECLARE MyException	
{
public:
	MyException() {};
	virtual ~MyException() {};
	virtual CString GetMsg() { return L"����������� ������!"; };
protected:
	CString book = L"";		// ��� ���������
	CString sheet = L"";	// ��� ����� � �����
};

// ���������� ������ CReaderExcel
#pragma region ReaderExcel
class BadTypeException: public MyException	
{
public:
	BadTypeException(const CString& book) { this->book = book; }
	virtual CString GetMsg() 
	{
		CString result;
		result.Format(L"�������� ���������� ����� \"%s\"", book);
		return result;
	};
};

// ������ � ��������� ������
class AccessExcelException : public MyException 
{
public:
	virtual CString GetMsg() { return L"��������� Excel-���������� �� �������"; };
};

class ReadBookException: public MyException 
{
public:
	ReadBookException(const CString& book) { this->book = book; };
	virtual CString GetMsg() 
	{
		CString result;
		result.Format(L"�� ������� ������� �������� \"%s\" ��� ������", book);
		return result;
	};
};

// ������ � ������ ������
class NotAllHeaderException : public MyException 
{
public:
	NotAllHeaderException(const CString& sheet) { this->sheet = sheet; };
	virtual CString GetMsg() 
	{ 
		CString result;
		result.Format(L"�� ����� \"%s\" �� ������� ����� ��� ���������", sheet);
		return result;
	};
};

class ExcelOverflow : public MyException
{
public:
	ExcelOverflow(const CString& sheet) { this->sheet = sheet; }
	virtual CString GetMsg()
	{
		CString result;
		result.Format(L"�� ����� \"%s\" ��������� ����� �� ������� �������", sheet);
		return result;
	}
};

class ReadSheetException : public MyException
{
public:
	ReadSheetException(const CString& sheet) { this->sheet = sheet; }
	virtual CString GetMsg()
	{
		CString result;
		result.Format(L"�� ����� \"%s\ ����������� ������", sheet);
		return result;
	}
};
#pragma endregion

#pragma region OTHER
// ���������� ��� ������ CTest
class UndefinedError : public MyException 
{
public:
	UndefinedError(const CString& book, const CString& sheet, const CString& param) 
	{
		this->book = book;
		this->sheet = sheet;
		this->param = param;
	}

	virtual CString GetMsg() 
	{
		CString result = L"����������� ������ � ���������!";
		result.Format(L"%s (��������: %s; ��� �����: %s; ��������: %s)", result, book, sheet, param);
		return result; 
	};
private:
	CString param = L"";	// �������� � �������
};

class BookNotFound : public MyException 
{
public:
	BookNotFound(const CString& book) { this->book = book; }
	virtual CString GetMsg() 
	{
		CString result;
		result.Format(L"�������� \"%s\" � ������ �����������!", book);
		return result;
	};
};

// ���������� ��� ������ CReport
class EmptyPathException : public MyException 
{
public:
	virtual CString GetMsg() { return L"���� ��� �������� ������ � txt �� �����!"; };
};
#pragma endregion