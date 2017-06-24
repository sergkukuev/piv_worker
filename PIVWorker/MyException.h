#pragma once

// Базовый класс исключений
class MyException	{
public:
	MyException() {};
	virtual ~MyException() {};
	virtual CString GetMsg() { return _T("Неизвестная ошибка!"); };
};

// Исключения класса CReaderExcel
class BadTypeException: public MyException	{
public:
	void setName(const CString& name) { this->name = name; }
	virtual CString GetMsg() {
		CString result;
		result.Format(_T("Неверное расширение файла \"%s\"!"), name);
		return result;
	};
private:
	CString name = (_T(""));
};

class AccessExcelException : public MyException {
public:
	void setOleError(const CString& error) { this->error = error; };
	virtual CString GetMsg() {
		CString result;
		result.Format(_T("Запустить приложение Excel не удалось!\n\nОшибка: %s"), error);
		return result;
	};
private:
	CString error = _T("");
};

class ReadBookException: public MyException {
public:
	void setParam(const CString& nameBook) { this->nameBook = nameBook; };
	virtual CString GetMsg() {
		CString result;
		result.Format(_T("Открыть книгу \"%s\" не удалось!"), nameBook);
		return result;
	};
private:
	CString nameBook = _T("");
};

class NotAllHeaderException : public MyException {
public:
	void setParam(const CString& nameSheet, const CString& nameBook) { 
		this->nameSheet = nameSheet;
		this->nameBook = nameBook;
	};
	virtual CString GetMsg() { 
		CString result;
		result.Format(_T("Не удалось найти все заголовки на листе \"%s\" в файле \"%s\"!"), nameSheet, nameBook);
		return result;
	};
private:
	CString nameBook = _T("");
	CString nameSheet = _T("");
};

// Исключения для класса Test
class UndefinedError : public MyException {
public:
	void SetParam(CString param) { this->param = param; }
	void SetName(CString name) { this->name = name; }
	virtual CString GetMsg() 
	{
		CString result = _T("Параметр сигнала с неизвестной ошибкой!");
		result.Format(_T("%s\n (Имя листа: %s, Параметр: %s)"), result, name, param);
		return result; 
	};
private:
	CString name = _T("");	// Имя листа, на котором найдена неизвестная ошибка
	CString param = _T("");	// Параметр, который обрабатывался
};

// Исключения для класс Report
class EmptyPathException : public MyException {
public:
	virtual CString GetMsg() { return _T("Путь для сохранения отчета не задан!"); };
};
