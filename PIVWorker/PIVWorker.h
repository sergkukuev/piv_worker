// PIVWorker.h: ������� ���� ��������� ��� DLL PIVWorker
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#ifdef PIV_WORKER_EXPORTS
#define PIV_WORKER_API __declspec(dllexport)
#else
#define PIV_WORKER_API __declspec(dllimport)
#endif

#include "StructPIV.h"		// ��������� ����������
#include "ReaderExcel.h"	// ������ ����������
#include "Test.h"			// �������� �� ������ ����������
#include "resource.h"		// �������� �������


// CPIVWorkerApp
// ��� ���������� ������� ������ ��. PIVWorker.cpp
//

class PIV_WORKER_API CPIVWorker	{
public:
	CPIVWorker();	// �����������
	~CPIVWorker();	// ����������

	void ReadExcel(CString pathToExcel);			// ������ ���
	void ReadExcel(vector <CString> pathToExcel);	// ������ ��� (����������)

	void Test();	// �������� ���� �������� ����

// ���������������
private:
	vector <bookData> books;	// ����������� ���
	vector <CString> path;		// ���� ���� ���

	vector <errorBookData> errorDB;	// ���� ������ ������

	bool isExist(CString pathToExcel);		// ������� �� ��� ����� �����
	int findReportBook(CString nameBook);	// ������� �� ����� �� ������� �� ������ �����
};
