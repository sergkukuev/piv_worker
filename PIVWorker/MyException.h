#pragma once

#include "StructPIV.h"
#include "DllDeclare.h"

// Базовый класс исключений
class PIV_DECLARE MyException	
{
public:
	MyException() {};
	virtual ~MyException() {};
	virtual CString GetMsg() { return L"Неизвестная ошибка!"; };
protected:
	CString book = L"";		// Имя протокола
	CString sheet = L"";	// Имя листа в файле
};

// Исключения класса CReaderExcel
class BadTypeException: public MyException	
{
public:
	BadTypeException(const CString& book) { this->book = book; }
	virtual CString GetMsg() 
	{
		CString result;
		result.Format(L"Неверное расширение файла \"%s\"!", book);
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
		result.Format(L"В протоколе \"%s\" на листе \"%s\" не удалось прочесть данные.", book, sheet);
		return result;
	}
};

class AccessExcelException : public MyException 
{
public:
	virtual CString GetMsg() { return L"Запустить Excel-приложение не удалось!"; };
};

class ReadBookException: public MyException 
{
public:
	ReadBookException(const CString& book) { this->book = book; };
	virtual CString GetMsg() 
	{
		CString result;
		result.Format(L"Не удалось открыть протокол \"%s\" для чтения!", book);
		return result;
	};
};

class NotAllHeaderException : public MyException 
{
public:
	NotAllHeaderException(const CString& book, const CString& sheet) {	this->sheet = sheet;	this->book = book;	};
	virtual CString GetMsg() 
	{ 
		CString result;
		result.Format(L"Не удалось найти все заголовки на листе \"%s\" в протоколе \"%s\"!", sheet, book);
		return result;
	};
};

// Исключения для класса CTest
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
		CString result = L"Неизвестная ошибка в параметре!";
		result.Format(L"%s (Протокол: %s; Имя листа: %s; Параметр: %s)", result, book, sheet, param);
		return result; 
	};
private:
	CString param = L"";	// Параметр с ошибкой
};

class BookNotFound : public MyException 
{
public:
	BookNotFound(const CString& book) { this->book = book; }
	virtual CString GetMsg() 
	{
		CString result;
		result.Format(L"Протокол \"%s\" в памяти отсутствует!", book);
		return result;
	};
};

// Исключения для класса CReport
class EmptyPathException : public MyException 
{
public:
	virtual CString GetMsg() { return L"Путь для хранения отчета и txt не задан!"; };
};
