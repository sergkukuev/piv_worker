#pragma once

// ������� ����� ����������
class MyException	{
public:
	MyException() {};
	virtual ~MyException() {};
	virtual char *GetMessage() { return "������!"; };
};

// ���������� ������ CReaderExcel
class BadTypeException: public MyException	{
public:
	virtual char* GetMessage() { return "������: �������� ���������� �����(��)!"; };
};

class NoBooksException: public MyException {
public:
	virtual char* GetMessage() { return "������: ������� ����(�) �� �������!"; };
};

class NotAllHeaderException : public MyException {
public:
	virtual char* GetMessage() { return "������: �� ������� ����� ��� ���������!"; };
};
