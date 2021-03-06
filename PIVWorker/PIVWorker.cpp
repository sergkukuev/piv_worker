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
}

bool CPIV::IsUpdate() { return logger.IsRead(); }

CString CPIV::GetStatus() { return logger.GetStatus(); }

void CPIV::WriteLog(const CString& msg) { logger.Write(msg); }

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
	logger.Write(L"�������� ������������ ����� �������");
}

void CPIV::SetSettings(const stgParams& parameters) 
{ 
	while (!GetStatusThread(primary))
		Sleep(100);
	settings.Save(parameters);
	logger.Write(L"�������� ��������� ����������");
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
		AfxMessageBox(L"��������� ��������� ���������� ��������, � ����� ��������� �������", MB_ICONINFORMATION);
}

void CPIV::OpenExcel() 
{
	logger.Write(L"���� �������� ���������� �������...");
	CReaderExcel reader;
	bool bOper = true;
	for (size_t i = 0; i < buffer.size(); i++)
	{
		try
		{
			project.books.push_back(reader.GetBook(buffer[i]));
		}
		catch (MyException& exc)
		{
			bOper = false;
			logger >> exc.GetMsg();
		}
	}

	CTest tester;
	project.db = tester.Start(project.books);
	// ��������� ����������
	CReport report;
	report.GetReport(project, true) ? bOper = bOper : bOper = false;		// true -  ������, false - ��������� ���������
	report.GetTxt(project.books) ? bOper = bOper : bOper = false;
	CloseThread(primary);
	bOper ? logger.Write(L"�������� ������� ��������� �������") : logger.Write(L"�������� ������� ��������� � ��������");
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
		AfxMessageBox(L"��������� ��������� ���������� ��������, � ����� ��������� �������", MB_ICONINFORMATION);
}

void CPIV::AddExcel() 
{
	bool bOper = true;
	CReport report;
	buffer.size() == 1 ? logger.Write(L"���� ���������� ���������...") : logger.Write(L"���� ���������� ����������...");
	for (size_t i = 0; i < buffer.size(); i++) 
	{
		try
		{
			CReaderExcel reader;
			bool contain = IsContain(other, buffer[i]);
			bookData book = reader.GetBook(buffer[i]);
			contain ? Refresh(other, book) : other.books.push_back(book);
			CTest tester;
			list <bookData>::iterator pBook = GetBook(other, buffer[i]);
			if (pBook == other.books.end())	// ����� �� ������a, ���� ��������� ����
				throw BookNotFound(NameFromPath(buffer[i]));
			errorData error = tester.Start(*pBook);
			contain ? Refresh(other, error) : other.db.push_back(error);
			report.GetTxt(*pBook) ? bOper = bOper : bOper = false;
		}
		catch (MyException& exc)
		{
			bOper = false;
			logger >> exc.GetMsg();
		}
	}
	report.GetReport(other, false) ? bOper = bOper : bOper = false;	// true -  ������, false - ��������� ���������
	CloseThread(primary);
	bOper ? logger.Write(L"���������� ��������� �������") : logger.Write(L"���������� ��������� � ��������");
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
		AfxMessageBox(L"��������� ��������� ���������� ��������, � ����� ��������� �������", MB_ICONINFORMATION);
}

void CPIV::RefreshExcel() 
{
	bool bOper = true, bProj = true;
	CReport report;
	buffer.size() == 1 ? logger.Write(L"���� ���������� ���������� ���������...") : logger.Write(L"���� ���������� ��������� ����������...");
	for (size_t i = 0; i < buffer.size(); i++)
	{
		try
		{
			CReaderExcel reader;
			bookData book = reader.GetBook(buffer[i]);
			list <bookData>::iterator pBook;
			CTest tester;
			if (IsContain(project, buffer[i]))
			{
				bProj = true;
				Refresh(project, book);
				pBook = GetBook(project, buffer[i]);
				errorData error = tester.Start(*pBook);
				Refresh(project, error);
			}
			else if (IsContain(other, buffer[i]))
			{
				bProj = false;
				Refresh(other, book);
				pBook = GetBook(other, buffer[i]);
				errorData error = tester.Start(*pBook);
				Refresh(other, error);
			}
			else
				throw BookNotFound(NameFromPath(buffer[i]));
			report.GetTxt(*pBook) ? bOper = bOper : bOper = false;
		}
		catch (MyException& exc)
		{
			bOper = false;
			logger >> exc.GetMsg();
		}
	}
	bProj ? (report.GetReport(project, true) ? bOper = bOper : bOper = false) : 
		(report.GetReport(other, false) ? bOper = bOper : bOper = false);	// true -  ������, false - ��������� ���������
	CloseThread(primary);
	bOper ? logger.Write(L"���������� ��������� �������") : logger.Write(L"���������� ��������� � ��������");
}
#pragma endregion

#pragma region CLOSE_PIV
// �������� ���� ����������
void CPIV::Close() 
{
	logger.Write(L"���� �������� ���� ����������...");
	if (!other.books.empty())
		other.books.clear();
	if (!other.db.empty())
		other.db.clear();
	logger.Write(L"�������� ���� ���������� ���������");	// �����������
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
		AfxMessageBox(L"��������� ��������� ���������� ��������, � ����� ��������� �������", MB_ICONINFORMATION);
}

// �������� �������
void CPIV::CloseProject() 
{
	logger.Write(L"���� �������� �������...");
	if (!project.books.empty())
		project.books.clear();
	if (!project.db.empty())
		project.db.clear();
	logger.Write(L"�������� ������� ���������");	// �����������
}

void CPIV::CloseExcel() 
{
	buffer.size() == 1 ? logger.Write(L"���� �������� ���������� ���������...") : logger.Write(L"���� �������� ��������� ����������...");
	for (size_t i = 0; i < buffer.size(); i++) 
	{
		for (list <errorData>::iterator it = other.db.begin(); it != other.db.end();)
			it->book->name.Compare(NameFromPath(buffer[i])) == 0 ? other.db.erase(it++) : it++;
		for (list <bookData>::iterator it = other.books.begin(); it != other.books.end();)
			it->name.Compare(NameFromPath(buffer[i])) == 0 ? other.books.erase(it++) : it++;
	}
	CloseThread(primary);
	logger.Write(L"�������� ��������� �������");	// �����������
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
	CString result = path.Mid(startPos, path.GetLength() - startPos);
	startPos = result.ReverseFind(L'.');
	result.Delete(startPos, result.GetLength() - startPos);
	return result;
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
		piv.logger.WriteError(L"������������ ������������ ������� ��� ���������� DLL �������� � ������");
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
		logger.WriteError(L"��������� ������ �������� ��������� ������ DLL");
}
#pragma endregion