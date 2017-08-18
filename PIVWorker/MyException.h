#pragma once

#include "DLL.h"			// ������ ����������� ������� � �������� DLL

// ������� ����� ����������
class PIV_DECLARE MyException	
{
public:
	MyException() {};
	virtual ~MyException() {};
	virtual CString GetMsg() { return L"����������� ������!"; };
protected:
	CString book = L"";		// ��� �����
	CString sheet = L"";	// ��� ����� � �����
};

// ���������� ������ CReaderExcel
class BadTypeException: public MyException	
{
public:
	BadTypeException(const CString& book) { this->book = book; }
	virtual CString GetMsg() 
	{
		CString result;
		result.Format(L"������: �������� ���������� ����� \"%s\"!", book);
		return result;
	};
};

class ExcelOverflow : public MyException 
{
public:
	ExcelOverflow(const CString& book, const CString& sheet) { this->book = book; this->sheet = sheet; }
	virtual CString GetMsg() 
	{
		CString result;
		result.Format(L"������: � ����� \"%s\" �� ����� \"%s\" �� ������� �������� ������.\n", book, sheet);
		return result;
	}
};
class AccessExcelException : public MyException 
{
public:
	virtual CString GetMsg() { return L"������: ��������� ���������� Excel �� �������!"; };
};

class ReadBookException: public MyException 
{
public:
	ReadBookException(const CString& book) { this->book = book; };
	virtual CString GetMsg() 
	{
		CString result;
		result.Format(L"������: ������� ���� \"%s\" ��� ������ �� �������!", book);
		return result;
	};
};

class NotAllHeaderException : public MyException 
{
public:
	NotAllHeaderException(const CString& book, const CString& sheet) 
	{ 
		this->sheet = sheet;
		this->book = book;
	};
	virtual CString GetMsg() 
	{ 
		CString result;
		result.Format(L"������: �� ������� ����� ��� ��������� �� ����� \"%s\" � ����� \"%s\"!", sheet, book);
		return result;
	};
};

// ���������� ��� ������ Test
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
		CString result = L"������: �������� ������� � ����������� �������!\n";
		result.Format(L"%s\n ����: %s;\n��� �����: %s;\n ��������: %s.", result, book, sheet, param);
		return result; 
	};
private:
	CString param = L"";	// ��������, ������� �������������
};

class BookNotFound : public MyException 
{
public:
	BookNotFound(const CString& book) { this->book = book; }
	virtual CString GetMsg() 
	{
		CString result;
		result.Format(L"������: ���� \"%s\" � ������ �����������!", book);
		return result;
	};
};

// ���������� ��� ����� Report
class EmptyPathException : public MyException 
{
public:
	virtual CString GetMsg() { return L"������: ���� ��� ���������� ������ �� �����!"; };
};
