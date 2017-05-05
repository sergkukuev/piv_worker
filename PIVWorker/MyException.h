#pragma once

// ������� ����� ����������
class MyException	{
public:
	MyException() {};
	virtual ~MyException() {};
	virtual CString GetMsg() { return _T("������!"); };
};

// ���������� ������ CReaderExcel
class BadTypeException: public MyException	{
public:
	virtual CString GetMsg() { return _T("�������� ���������� �����(��)!"); };
};

class NoBooksException: public MyException {
public:
	virtual CString GetMsg() { return _T("������� ����(�) �� �������!"); };
};

class NotAllHeaderException : public MyException {
public:
	virtual CString GetMsg() { return _T("�� ������� ����� ��� ��������� �� ����� �� ������!"); };
};
