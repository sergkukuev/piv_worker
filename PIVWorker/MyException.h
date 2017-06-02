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

// ���������� ��� ������ Test
class UndefinedError : public MyException {
public:
	void SetParam(CString param) { this->param = param; }
	void SetName(CString name) { this->name = name; }
	virtual CString GetMsg() 
	{
		CString result = _T("�������� ������� � ����������� �������!");
		result.Format(_T("%s\n (��� �����: %s, ��������: %s)"), result, name, param);
		return result; 
	};
private:
	CString name = _T("");	// ��� �����, �� ������� ������� ����������� ������
	CString param = _T("");	// ��������, ������� �������������
};
