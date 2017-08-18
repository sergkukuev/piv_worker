#pragma once

#include "DLL.h"			// Макрос определения импорта и экспорта DLL

// Базовый класс исключений
class PIV_DECLARE MyException	
{
public:
	MyException() {};
	virtual ~MyException() {};
	virtual CString GetMsg() { return L"Неизвестная ошибка!"; };
protected:
	CString book = L"";		// Имя файла
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
		result.Format(L"Ошибка: Неверное расширение файла \"%s\"!", book);
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
		result.Format(L"Ошибка: В файле \"%s\" на листе \"%s\" не удалось прочесть данные.\n", book, sheet);
		return result;
	}
};
class AccessExcelException : public MyException 
{
public:
	virtual CString GetMsg() { return L"Ошибка: Запустить приложение Excel не удалось!"; };
};

class ReadBookException: public MyException 
{
public:
	ReadBookException(const CString& book) { this->book = book; };
	virtual CString GetMsg() 
	{
		CString result;
		result.Format(L"Ошибка: Открыть файл \"%s\" для чтения не удалось!", book);
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
		result.Format(L"Ошибка: Не удалось найти все заголовки на листе \"%s\" в файле \"%s\"!", sheet, book);
		return result;
	};
};

// Исключения для класса Test
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
		CString result = L"Ошибка: Параметр сигнала с неизвестной ошибкой!\n";
		result.Format(L"%s\n Файл: %s;\nИмя листа: %s;\n Параметр: %s.", result, book, sheet, param);
		return result; 
	};
private:
	CString param = L"";	// Параметр, который обрабатывался
};

class BookNotFound : public MyException 
{
public:
	BookNotFound(const CString& book) { this->book = book; }
	virtual CString GetMsg() 
	{
		CString result;
		result.Format(L"Ошибка: Файл \"%s\" в памяти отсутствует!", book);
		return result;
	};
};

// Исключения для класс Report
class EmptyPathException : public MyException 
{
public:
	virtual CString GetMsg() { return L"Ошибка: Путь для сохранения отчета не задан!"; };
};
