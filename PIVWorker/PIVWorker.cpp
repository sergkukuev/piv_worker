// PIVWorker.cpp: ���������� ��������� ������������� ��� DLL.
//

#include "stdafx.h"
#include "PIVWorker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// TODO: �����! ��������� �� ������, ����� ������� �����������.
//	 	 ���� ��� ���������� DLL ����������� ������� � ������������ DLL MFC,
//		 ��� �������, ���������������� �� ������ DLL-����������, ������� ��������� ������ �
//		 MFC, ������ ��������� ������ AFX_MANAGE_STATE �
//		 ����� ������ �������.
//
//		 ��������:
//
//		 extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		 {
//			 AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			 // ���� ���������� �������
//		 }
//
//		 �����, ����� ������ ������ ��� ����������� � ������
//		 ������� �� ������ MFC.  ��� ��������, ���
//		 �� ������ ���� ������ ���������� 
//		 ������� � �������������� ���� ����� ����������� ���������� �������,
//		 ��������� �� ������������ ����� ��������� ������ � MFC
//		 DLL.
//
//		 � ����������� ��������� MFC 33 � 58 ���������� �����
//		 ��������� ��������.
//

using namespace stgdll;	// ������������ ���� ��������
struct ThreadData
{
	CPIV* object;
};

// ������������ ������ CPIVWorkerApp
CWinApp theApp;
ThreadData mData;

// �������� �����
DWORD WINAPI PrimaryThread(LPVOID lpParam) 
{
	CoInitialize(NULL);
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	ThreadData* data = static_cast<ThreadData*>(lpParam);
	Thread(*data->object);
	CoUninitialize();
	return 0;
}

// �����������
CPIV::CPIV() {	}

// ����������
CPIV::~CPIV() 
{
	Close();
	CloseProject();
	logger.Write(LOG_SLASH);
}

bool CPIV::IsUpdate() { return logger.IsRead(); }

CString CPIV::GetStatus() { return logger.GetStatus(); }

void CPIV::WriteLog(const CString& msg) { logger << msg; }

// ��������� ����� ����������
CString CPIV::GetPath() { return settings.GetPath(); }

CString CPIV::GetOtherPath()
{
	CString result;
	result.Format(L"%s%s", settings.GetPath(), settings.folders[folders::other]);
	return result;
}

CString CPIV::GetProjectPath()
{
	CString result;
	result.Format(L"%s%s", settings.GetPath(), settings.folders[folders::project]);
	return result;
}
#pragma region SET_PARAMETERS
// ��������� ���� �������� ����������
void CPIV::SetPathToSave(const CString& pathToReport)
{
	settings.SetStgPath(pathToReport);
	logger << L"�������� ������������ ����� �������";
}

void CPIV::SetSettings(const stgParams& parameters) 
{ 
	while (!GetStatusThread(primary))
		Sleep(100);
	settings.parameters = parameters;
	logger << L"�������� ��������� ����������";
}

stgParams CPIV::GetSettings() { return settings.GetParameters(); }
#pragma endregion

#pragma region OPEN_PROJECT
// �������� ������� (������ ���) � ���������� ���� �������� ����������
void CPIV::Open(const vector<CString> pathToExcel, const CString pathToReport) 
{
	buffer = pathToExcel;
	settings.SetStgPath(pathToReport);
	StartOpen();
}

// ������������ ������ ���� ��������
void CPIV::Open(const vector<CString> pathToExcel)
{
	buffer = pathToExcel;
	StartOpen();
}

// ������ ������ ��� �������� �������� �������
void CPIV::StartOpen() 
{
	if (GetStatusThread(primary))
	{
		CloseProject();	// ������� ������ ������ �������
		hCmd = open;
		mData.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mData, 0, 0);
	}
	else
		AfxMessageBox(LOG_THREAD_BUSY, MB_ICONINFORMATION);
}

void CPIV::OpenExcel() 
{
	try 
	{
		logger << L"���� �������� ����������...";
		CReaderExcel reader;
		for (size_t i = 0; i < buffer.size(); i++)
			project.books.push_back(reader.GetBook(buffer[i]));

		CTest tester;
		project.db = tester.Start(project.books);

		// ��������� ����������
		CReport report;
		report.GetReport(project, true);		// true -  ������, false - ��������� ���������
		report.GetTxt(project.books);
		CloseThread(primary);
		logger << L"�������� ���������� ���������";	// �����������
	}
	catch (MyException& exc)
	{
		logger.WriteError(exc.GetMsg());
	}
}
#pragma endregion

#pragma region OPEN_PIV
// ������� ��������� ���������:
// ����
void CPIV::Add(const CString pathToExcel) 
{
	buffer.push_back(pathToExcel);
	StartAdd();
}

// ���������
void CPIV::Add(const vector<CString> pathToExcel) 
{
	buffer = pathToExcel;
	StartAdd();
}

// ������ ������ ��� �������� ��������� ����������
void CPIV::StartAdd() 
{
	if (GetStatusThread(primary))
	{
		hCmd = add;
		mData.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mData, 0, 0);
	}
	else
		AfxMessageBox(LOG_THREAD_BUSY, MB_ICONINFORMATION);
}

void CPIV::AddExcel() 
{
	try 
	{
		logger >> L"���� ���������� ����������...";
		CReport report;
		for (size_t i = 0; i < buffer.size(); i++) 
		{
			bool contain = IsContain(other, buffer[i]);
			CReaderExcel reader;
			bookData book = reader.GetBook(buffer[i]);
			contain ? Refresh(other, book) : other.books.push_back(book);
			
			CTest tester;
			list <bookData>::iterator pBook = GetBook(other, buffer[i]);
			errorData error = tester.Start(*pBook);
			contain ? Refresh(other, error) : other.db.push_back(error);

			report.GetTxt(*pBook);
		}
		
		report.GetReport(other, false);	// true -  ������, false - ��������� ���������
		CloseThread(primary);
		
		logger << L"���������� ���������� ���������";	// �����������
	}
	catch (MyException& exc) 
	{
		logger.WriteError(exc.GetMsg());
	}
}
#pragma endregion

#pragma region REFRESH_PIV
// ���������� ����������
void CPIV::Refresh(const vector<CString> pathToExcel)
{
	buffer = pathToExcel;
	StartRefresh();
}

void CPIV::Refresh(const CString pathToExcel) 
{
	buffer.push_back(pathToExcel);
	StartRefresh();
}

// ������ �������� ���������� ����������
void CPIV::StartRefresh() 
{
	if (GetStatusThread(primary))
	{
		hCmd = refresh;
		mData.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mData, 0, 0);
	}
	else
		AfxMessageBox(LOG_THREAD_BUSY, MB_ICONINFORMATION);
}

void CPIV::RefreshExcel() 
{
	try 
	{
		logger >> L"���� ���������� ��������� ����������...";
		CReport report;
		bool flag = true;
		for (size_t i = 0; i < buffer.size(); i++)
		{
			CReaderExcel reader;
			bookData book = reader.GetBook(buffer[i]);
			list <bookData>::iterator pBook;

			CTest tester;
			if (IsContain(project, buffer[i])) 
			{
				flag = true;
				Refresh(project, book);
				pBook = GetBook(project, buffer[i]);
				errorData error = tester.Start(*pBook);
				Refresh(project, error);
			}
			else if (IsContain(other, buffer[i])) 
			{
				flag = false;
				Refresh(other, book);
				pBook = GetBook(other, buffer[i]);
				errorData error = tester.Start(*pBook);
				Refresh(other, error);
			}
			else
				throw BookNotFound(NameFromPath(buffer[i]));

			report.GetTxt(*pBook);
		}
		
		flag ?	report.GetReport(project, true) : report.GetReport(other, false);
	
		CloseThread(primary);
		
		logger << L"���������� ���������� ���������";	// �����������
	}
	catch (MyException& exc) 
	{
		logger.WriteError(exc.GetMsg());
	}
}
#pragma endregion

#pragma region CLOSE_PIV
// �������� ���� ����������
void CPIV::Close() 
{
	if (!other.books.empty() || !other.db.empty())
	{
		logger >> L"���� �������� ���� ����������...";
		other.books.clear();
		other.db.clear();
		logger << L"�������� ���������� ���������";	// �����������
	}
}

// ������
void CPIV::Close(const CString path)
{
	buffer.push_back(path);
	StartClose();
}

// ����������
void CPIV::Close(const vector<CString> path) 
{
	buffer = path;
	StartClose();
}

// ������ �������� ��������
void CPIV::StartClose() 
{
	if (GetStatusThread(primary))
	{
		hCmd = close;
		mData.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mData, 0, 0);
	}
	else
		AfxMessageBox(LOG_THREAD_BUSY, MB_ICONINFORMATION);
}

// �������� �������
void CPIV::CloseProject() 
{
	if (!project.books.empty() || !project.db.empty())
	{
		logger >> L"���� �������� �������...";
		project.books.clear();
		project.db.clear();
		logger << L"�������� ������� ���������";	// �����������
	}
}

void CPIV::CloseExcel() 
{
	logger >> L"���� �������� ��������� ����������...";
	for (size_t i = 0; i < buffer.size(); i++) 
	{
		for (list <errorData>::iterator it = other.db.begin(); it != other.db.end();)
			it->book->name.Compare(NameFromPath(buffer[i])) == 0 ? other.db.erase(it++) : it++;
		for (list <bookData>::iterator it = other.books.begin(); it != other.books.end();)
			it->name.Compare(NameFromPath(buffer[i])) == 0 ? other.books.erase(it++) : it++;
	}
	CloseThread(primary);
	logger << L"�������� ���������� ���������";	// �����������
}
#pragma endregion

#pragma region SUB_FUNCTION
// ���������� ���������� (������ + ���� ������)
void CPIV::Refresh(pivData& data, const bookData& book, const errorData& error) 
{
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (book.name.Compare(it->name) == 0)
			*it = book;

	for (list <errorData>::iterator it = data.db.begin(); it != data.db.end(); it++)
		if (error.book->name.Compare(it->book->name) == 0)
			*it = error;
}

// ���������� (������)
void CPIV::Refresh(pivData& data, const bookData& book) 
{
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (book.name.Compare(it->name) == 0)
			*it = book;
}

// ���������� (���� ������)
void CPIV::Refresh(pivData& data, const errorData& error) 
{
	for (list <errorData>::iterator it = data.db.begin(); it != data.db.end(); it++)
		if (error.book->name.Compare(it->book->name) == 0)
			*it = error;
}

// ��������� ������ �� ������ ���������� ���������
list<bookData>::iterator CPIV::GetBook(pivData& data, const CString& path)
{
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (NameFromPath(path).Compare(it->name) == 0)
			return it;
	return data.books.end();
}

// ��������� ����� ��������� �� ��� ����
CString CPIV::NameFromPath(const CString& path) 
{
	int startPos = path.ReverseFind(L'\\') + 1;
	return path.Mid(startPos, path.GetLength());
}

// �������� ������� ������ ���������� ���������
bool CPIV::IsContain(pivData& data, const CString& path)
{
	bool result = false;
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		NameFromPath(path).Compare(it->name) == 0 ? result = true : result = result;
	return result;
}
#pragma endregion

#pragma region THREAD_FUNC
// ������ �������� DLL � ������
void Thread(CPIV& piv)
{
	switch (piv.hCmd)
	{
	case CPIV::open:
		piv.OpenExcel();
		break;
	case CPIV::add:
		piv.AddExcel();
		break;
	case CPIV::refresh:
		piv.RefreshExcel();
		break;
	case CPIV::close:
		piv.CloseExcel();
		break;
	default:
		piv.logger.WriteError(L"������: ������������ �������");
		break;
	}
}

// �������� ����������� ������
bool CPIV::GetStatusThread(const HANDLE& h)
{
	DWORD ty;
	bool result = true;
	GetExitCodeThread(h, &ty);
	ty != STILL_ACTIVE ? result = true : result = false;
	return result;
}

// �������� ������
void CPIV::CloseThread(HANDLE& h)
{
	if (!buffer.empty())	// ������� ������
	{
		buffer.clear();
		buffer.shrink_to_fit();
	}

	if (h != 0)
	{
		CloseHandle(h);
		h = NULL;
	}
	else
		logger.WriteError(L"������: �� ������� ������� �����");
}
#pragma endregion