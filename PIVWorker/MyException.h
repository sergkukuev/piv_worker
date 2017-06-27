#pragma once

// Базовый класс исключений
class MyException	{
public:
	MyException() {};
	virtual ~MyException() {};
	virtual CString GetMsg() { return L"Неизвестная ошибка!"; };
};

// Исключения класса CReaderExcel
class BadTypeException: public MyException	{
public:
	void setName(const CString& name) { this->name = name; }
	virtual CString GetMsg() {
		CString result;
		result.Format(L"Неверное расширение файла \"%s\"!", name);
		return result;
	};
private:
	CString name = L"";
};

class AccessExcelException : public MyException {
public:
	virtual CString GetMsg() { return L"Запустить приложение Excel не удалось!"; };
};

class ReadBookException: public MyException {
public:
	void setParam(const CString& nameBook) { this->nameBook = nameBook; };
	virtual CString GetMsg() {
		CString result;
		result.Format(L"Открыть книгу \"%s\" не удалось!", nameBook);
		return result;
	};
private:
	CString nameBook = L"";
};

class NotAllHeaderException : public MyException {
public:
	void setParam(const CString& nameSheet, const CString& nameBook) { 
		this->nameSheet = nameSheet;
		this->nameBook = nameBook;
	};
	virtual CString GetMsg() { 
		CString result;
		result.Format(L"Не удалось найти все заголовки на листе \"%s\" в файле \"%s\"!", nameSheet, nameBook);
		return result;
	};
private:
	CString nameBook = L"";
	CString nameSheet = L"";
};

// Исключения для класса Test
class UndefinedError : public MyException {
public:
	void SetParam(CString param) { this->param = param; }
	void SetName(CString name) { this->name = name; }
	virtual CString GetMsg() 
	{
		CString result = L"Параметр сигнала с неизвестной ошибкой!";
		result.Format(L"%s\n (Имя листа: %s, Параметр: %s)", result, name, param);
		return result; 
	};
private:
	CString name = L"";	// Имя листа, на котором найдена неизвестная ошибка
	CString param = L"";	// Параметр, который обрабатывался
};

// Исключения для класс Report
class EmptyPathException : public MyException {
public:
	virtual CString GetMsg() { return L"Путь для сохранения отчета не задан!"; };
};
