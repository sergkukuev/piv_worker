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
