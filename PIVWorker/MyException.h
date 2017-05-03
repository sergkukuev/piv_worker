#pragma once

// Базовый класс исключений
class MyException	{
public:
	MyException() {};
	virtual ~MyException() {};
	virtual char *GetMessage() { return "Ошибка!"; };
};

// Исключения класса CReaderExcel
class BadTypeException: public MyException	{
public:
	virtual char* GetMessage() { return "Ошибка: Неверное расширение файла(ов)!"; };
};

class NoBooksException: public MyException {
public:
	virtual char* GetMessage() { return "Ошибка: Открыть файл(ы) не удалось!"; };
};

class NotAllHeaderException : public MyException {
public:
	virtual char* GetMessage() { return "Ошибка: Не удалось найти все заголовки!"; };
};
