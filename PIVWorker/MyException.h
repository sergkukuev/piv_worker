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
#pragma region ReaderExcel
class BadTypeException: public MyException	
{
public:
	BadTypeException(const CString& book) { this->book = book; }
	virtual CString GetMsg() 
	{
		CString result;
		result.Format(L"Неверное расширение файла \"%s\"", book);
		return result;
	};
};

// Работа с открытием екселя
class AccessExcelException : public MyException 
{
public:
	virtual CString GetMsg() { return L"Запустить Excel-приложение не удалось"; };
};

class ReadBookException: public MyException 
{
public:
	ReadBookException(const CString& book) { this->book = book; };
	virtual CString GetMsg() 
	{
		CString result;
		result.Format(L"Не удалось открыть протокол \"%s\" для чтения", book);
		return result;
	};
};

// Работа с листом екселя
class NotAllHeaderException : public MyException 
{
public:
	NotAllHeaderException(const CString& sheet) { this->sheet = sheet; };
	virtual CString GetMsg() 
	{ 
		CString result;
		result.Format(L"На листе \"%s\" не удалось найти все заголовки", sheet);
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
		result.Format(L"На листе \"%s\" произошел выход за границы таблицы", sheet);
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
		result.Format(L"На листе \"%s\ отсутствуют данные", sheet);
		return result;
	}
};
#pragma endregion

#pragma region OTHER
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
#pragma endregion