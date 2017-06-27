#pragma once

// ������� ����� ����������
class MyException	{
public:
	MyException() {};
	virtual ~MyException() {};
	virtual CString GetMsg() { return L"����������� ������!"; };
};

// ���������� ������ CReaderExcel
class BadTypeException: public MyException	{
public:
	void setName(const CString& name) { this->name = name; }
	virtual CString GetMsg() {
		CString result;
		result.Format(L"�������� ���������� ����� \"%s\"!", name);
		return result;
	};
private:
	CString name = L"";
};

class AccessExcelException : public MyException {
public:
	virtual CString GetMsg() { return L"��������� ���������� Excel �� �������!"; };
};

class ReadBookException: public MyException {
public:
	void setParam(const CString& nameBook) { this->nameBook = nameBook; };
	virtual CString GetMsg() {
		CString result;
		result.Format(L"������� ����� \"%s\" �� �������!", nameBook);
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
		result.Format(L"�� ������� ����� ��� ��������� �� ����� \"%s\" � ����� \"%s\"!", nameSheet, nameBook);
		return result;
	};
private:
	CString nameBook = L"";
	CString nameSheet = L"";
};

// ���������� ��� ������ Test
class UndefinedError : public MyException {
public:
	void SetParam(CString param) { this->param = param; }
	void SetName(CString name) { this->name = name; }
	virtual CString GetMsg() 
	{
		CString result = L"�������� ������� � ����������� �������!";
		result.Format(L"%s\n (��� �����: %s, ��������: %s)", result, name, param);
		return result; 
	};
private:
	CString name = L"";	// ��� �����, �� ������� ������� ����������� ������
	CString param = L"";	// ��������, ������� �������������
};

// ���������� ��� ����� Report
class EmptyPathException : public MyException {
public:
	virtual CString GetMsg() { return L"���� ��� ���������� ������ �� �����!"; };
};
