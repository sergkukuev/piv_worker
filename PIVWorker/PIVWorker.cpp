// PIVWorker.cpp: ���������� ��������� ������������� ��� DLL.
//

#include "stdafx.h"
#include "PIVWorker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: ���� ��� ���������� DLL ����������� ������� � ������������ DLL MFC,
//		��� �������, ���������������� �� ������ DLL-����������, ������� ��������� ������ �
//		MFC, ������ ��������� ������ AFX_MANAGE_STATE �
//		����� ������ �������.
//
//		��������:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// ���� ���������� �������
//		}
//
//		�����, ����� ������ ������ ��� ����������� � ������
//		������� �� ������ MFC.  ��� ��������, ���
//		�� ������ ���� ������ ���������� 
//		������� � �������������� ���� ����� ����������� ���������� �������,
//		��������� �� ������������ ����� ��������� ������ � MFC
//		DLL.
//
//		� ����������� ��������� MFC 33 � 58 ���������� �����
//		��������� ��������.
//


// ������������ ������ CPIVWorkerApp
CWinApp theApp;


// ������������� CPIVWorkerApp
int main()
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// ���������������� MFC, � ����� ������ � ��������� �� ������� ��� ����
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: �������� ��� ������ �������������� ����� ������������
			wprintf(L"����������� ������: ���� ��� ������������� MFC\n");
			nRetCode = 1;
		}
		else
		{
			// TODO: �������� ���� ��� ��� ����������.
		}
	}
	else
	{
		// TODO: �������� ��� ������ �������������� ����� ������������
		wprintf(L"����������� ������: ��������� ���������� GetModuleHandle\n");
		nRetCode = 1;
	}

	return nRetCode;
}

// �����������
CPIVWorker::CPIVWorker()
{

}

// ����������
CPIVWorker::~CPIVWorker()
{
	path.clear();
	books.clear();
	errorDB.clear();
}

// ������ ���
void CPIVWorker::ReadExcel(CString pathToExcel)
{
	try
	{
		CReaderExcel reader;

		if (!isExist(pathToExcel))
		{
			books.push_back(reader.getBook(pathToExcel));
			path.push_back(pathToExcel);
		}
	}
	catch (MyException& exc)
	{
		AfxMessageBox(exc.GetMsg());
	}
}

// ������ ���
void CPIVWorker::ReadExcel(vector<CString> pathToExcel)
{
	try
	{
		for (size_t i = 0; i < pathToExcel.size(); i++)
		{
			CReaderExcel reader;	// ����� ������ ����

			if (!isExist(pathToExcel[i]))
			{
				books.push_back(reader.getBook(pathToExcel[i]));
				path.push_back(pathToExcel[i]);
			}
		}
	}
	catch (MyException &exc)
	{
		AfxMessageBox(exc.GetMsg());
	}
}

// �������� ���� �������� ����
void CPIVWorker::Test()
{
	try
	{
		for (size_t i = 0; i < books.size(); i++)
		{
			CTest tester;	// ����� �������� ����
			errorBookData errBook;

			errBook = tester.Start(books[i]);

			int indx = findReportBook(errBook.name);	// ����� � ���� ������� ������ �����

			if (indx != -1)
				errorDB[indx] = errBook;	// ���������� ������
			else
				errorDB.push_back(errBook); // ���������� ������ ������
		}
	}
	catch (MyException &exc)
	{
		AfxMessageBox(exc.GetMsg());
	}
}

// ������� �� ��� ����� �����
bool CPIVWorker::isExist(CString pathToExcel)
{
	for (size_t i = 0; i < path.size(); i++)
		if (pathToExcel.Compare(path[i]) == 0)
			return true;

	return false;
}

// ������� �� ����� �� ������� �� ������ �����
int CPIVWorker::findReportBook(CString nameBook)
{
	int result = -1;

	for (size_t i = 0; i < errorDB.size(); i++)
		if (errorDB[i].name.Compare(nameBook) == 0)
			result = i;

	return result;
}
