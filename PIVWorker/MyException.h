#pragma once

// ������� ����� ����������
class MyException	{
public:
	MyException() {};
	virtual ~MyException() {};
	virtual CString GetMsg() { return _T("����������� ������!"); };
};

// ���������� ������ CReaderExcel
class BadTypeException: public MyException	{
public:
	void setName(const CString& name) { this->name = name; }
	virtual CString GetMsg() {
		CString result;
		result.Format(_T("�������� ���������� ����� \"%s\"!"), name);
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
		result.Format(_T("��������� ���������� Excel �� �������!\n\n������: %s"), error);
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
		result.Format(_T("������� ����� \"%s\" �� �������!"), nameBook);
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
		result.Format(_T("�� ������� ����� ��� ��������� �� ����� \"%s\" � ����� \"%s\"!"), nameSheet, nameBook);
		return result;
	};
private:
	CString nameBook = _T("");
	CString nameSheet = _T("");
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

// ���������� ��� ����� Report
class EmptyPathException : public MyException {
public:
	virtual CString GetMsg() { return _T("���� ��� ���������� ������ �� �����!"); };
};
