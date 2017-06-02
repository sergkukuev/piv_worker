#pragma once

// Базовый класс исключений
class MyException	{
public:
	MyException() {};
	virtual ~MyException() {};
	virtual CString GetMsg() { return _T("Ошибка!"); };
};

// Исключения класса CReaderExcel
class BadTypeException: public MyException	{
public:
	virtual CString GetMsg() { return _T("Неверное расширение файла(ов)!"); };
};

class NoBooksException: public MyException {
public:
	virtual CString GetMsg() { return _T("Открыть файл(ы) не удалось!"); };
};

class NotAllHeaderException : public MyException {
public:
	virtual CString GetMsg() { return _T("Не удалось найти все заголовки на одном из листов!"); };
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
