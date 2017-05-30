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

struct MyData
{
	CPIVWorker* object;
};

MyData mD;

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

DWORD WINAPI ReaderThread(LPVOID lpParam)
{
	CoInitialize(NULL);
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	MyData* myD = static_cast<MyData*>(lpParam);

	ReadExcel(*(myD->object));

	CoUninitialize();

	return 0;
}

DWORD WINAPI TestThread(LPVOID lpParam)
{
	CoInitialize(NULL);
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	MyData* myD = static_cast<MyData*>(lpParam);

	Test(*(myD->object));

	CoUninitialize();

	return 0;
}

DWORD WINAPI ReportThread(LPVOID lpParam)
{
	CoInitialize(NULL);
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	MyData* myD = static_cast<MyData*>(lpParam);

	Report(*(myD->object));

	CoUninitialize();

	return 0;
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
	errorDB.syntax.clear();
	errorDB.simantic.clear();
	errorDB.warning.clear();
	checkPath.clear();
}

// ��������� ����� ��� ������
void CPIVWorker::ReadExcel(vector <CString> pathToExcel)
{
	checkPath = pathToExcel;
	StartRead();
}

// ��������� ���� ��� ������
void CPIVWorker::ReadExcel(CString pathToExcel)
{
	checkPath.push_back(pathToExcel);
	StartRead();
}

// ������ ������
void CPIVWorker::StartRead()
{
	if (getStatusThread(primary))
	{
		mD.object = this;
		primary = CreateThread(NULL, 0, ReaderThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(_T("����� �����! ��������� ��������� ��������!"));
}

// ������ ���
void CPIVWorker::ReadExcel()
{
	try
	{
		for (size_t i = 0; i < checkPath.size(); i++)
		{
			CReaderExcel reader;	// ����� ������ ����

			if (findIndexBook(checkPath[i]) == -1)		// ���� ����� ����� ��� ���
			{
				books.push_back(reader.getBook(checkPath[i]));
				path.push_back(checkPath[i]);
			}
		}
		AfxMessageBox(_T("������ ��������� �������!"));
	}
	catch (MyException &exc)
	{
		AfxMessageBox(exc.GetMsg());
	}

	checkPath.clear();
	closeThread(primary);
}

// ������ ������������
void CPIVWorker::Test()
{
	if (getStatusThread(primary))
	{
		mD.object = this;
		primary = CreateThread(NULL, 0, TestThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(_T("����� �����! ��������� ��������� ��������!"));
}

// �������� ���� �������� ����
void CPIVWorker::TestAll()
{
	try
	{
		for (size_t i = 0; i < books.size(); i++)
		{
			CTest tester;	// ����� �������� ����
			errorOneSet errBook;

			errBook = tester.Start(books[i]);

			int indx = findReportBook(errBook.syntax.name);	// ����� � ���� ������� ������ �����

			if (indx != -1)	// ���������� ������
			{
				errorDB.syntax[indx] = errBook.syntax;
				errorDB.simantic[indx] = errBook.simantic;
				errorDB.warning[indx] = errBook.warning;
			}
			else // ���������� ������ ������
			{
				errorDB.syntax.push_back(errBook.syntax);
				errorDB.simantic.push_back(errBook.simantic);
				errorDB.warning.push_back(errBook.warning);
			}
				
		}
		AfxMessageBox(_T("�������� ���������� ��������� �������!"));
	}
	catch (MyException &exc)
	{
		AfxMessageBox(exc.GetMsg());
	}
	closeThread(primary);
}

// �������� ������ �� �������
void CPIVWorker::Report(CString path)
{
	if (path.IsEmpty())
		AfxMessageBox(_T("������� ���� ���������� ������!"));
	else
	{
		pathRep = path;
		if (getStatusThread(primary))
		{
			mD.object = this;
			primary = CreateThread(NULL, 0, ReportThread, &mD, 0, 0);
		}
		else
			AfxMessageBox(_T("����� �����! ��������� ��������� ��������!"));
	}
}

// �������� ������
void CPIVWorker::MakeReport()
{
	CReport report;

	report.setPath(pathRep);
	report.Generate(books, errorDB);
	AfxMessageBox(_T("�������� ������ ���������!"));
}

// ������� ��������
void CPIVWorker::CloseExcel(CString pathToExcel)
{
	int iBook = findIndexBook(pathToExcel);
	
	if (iBook != -1)
	{
		int iReport = findReportBook(books[iBook].nameBook);

		if (iReport != -1)	// ������ � ���� ������
		{
			errorDB.syntax.erase(errorDB.syntax.begin() + iReport);
			vector<errorBookData>(errorDB.syntax).swap(errorDB.syntax);

			errorDB.simantic.erase(errorDB.simantic.begin() + iReport);
			vector<errorBookData>(errorDB.simantic).swap(errorDB.simantic);

			errorDB.warning.erase(errorDB.warning.begin() + iReport);
			vector<errorBookData>(errorDB.warning).swap(errorDB.warning);
		}

		// ������ ���� � ������
		books.erase(books.begin() + iBook);
		vector<bookData>(books).swap(books);

		path.erase(path.begin() + iBook);
		vector<CString>(path).swap(path);
	}
}

// ������� ��������
void CPIVWorker::CloseExcel(vector <CString> pathToExcel)
{
	for (size_t i = 0; i < pathToExcel.size(); i++)
	{
		int iBook = findIndexBook(pathToExcel[i]);

		if (iBook != -1)
		{
			int iReport = findReportBook(books[iBook].nameBook);

			if (iReport != -1)	// ������ � ���� ������
			{
				errorDB.syntax.erase(errorDB.syntax.begin() + iReport);
				vector<errorBookData>(errorDB.syntax).swap(errorDB.syntax);

				errorDB.simantic.erase(errorDB.simantic.begin() + iReport);
				vector<errorBookData>(errorDB.simantic).swap(errorDB.simantic);

				errorDB.warning.erase(errorDB.warning.begin() + iReport);
				vector<errorBookData>(errorDB.warning).swap(errorDB.warning);
			}

			// ������ ���� � ������
			books.erase(books.begin() + iBook);
			vector<bookData>(books).swap(books);

			path.erase(path.begin() + iBook);
			vector<CString>(path).swap(path);
		}
	}
}

// ����� ������� ����������� ����� � ���� ������
int CPIVWorker::findReportBook(CString name)
{
	for (size_t i = 0; i < errorDB.syntax.size(); i++)
		if (name.Compare(errorDB.syntax[i].name) == 0)
			return i;

	return -1;
}

// ����� ������� ����������� ����� � ���� ����
int CPIVWorker::findIndexBook(CString pathToExcel)
{
	for (size_t i = 0; i < path.size(); i++)
		if (pathToExcel.Compare(path[i]) == 0)
			return i;

	return -1;
}

// ������������� ������� ��� ������� ������ ����������
void ReadExcel(CPIVWorker& piv)
{
	piv.ReadExcel();
}

// ������������� ������� ��� ������� ������� ����������
void Test(CPIVWorker& piv)
{
	piv.TestAll();
}

// ������������� ������� ��� ������� �������� ������
void Report(CPIVWorker& piv)
{
	piv.MakeReport();
}

// �������� ������ �� �����������
bool CPIVWorker::getStatusThread(HANDLE h)
{
	DWORD ty;
	bool result = true;
	GetExitCodeThread(h, &ty);

	if (ty != STILL_ACTIVE)
		result = true;
	else
		result = false;

	return result;
}

// �������� ������
void CPIVWorker::closeThread(HANDLE& h)
{
	if (h != 0)
	{
		CloseHandle(h);
		h = NULL;
	}
	else
		AfxMessageBox(_T("� �� � �� ����������, ����� �������!"));
}