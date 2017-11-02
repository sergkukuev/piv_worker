// PIVWorker.cpp: ���������� ��������� ������������� ��� DLL.
//

#include "stdafx.h"
#include "PIVWorker.h"
#include "ReaderExcel.h"	// ������ ����������
#include "Test.h"			// �������� �� ������ ����������
#include "Report.h"			// �������� ������� �� �������

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

// �������� �����
DWORD WINAPI PrimaryThread(LPVOID lpParam) 
{
	CoInitialize(NULL);
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	CPIV** object = static_cast<CPIV**>(lpParam);
	Thread(**object);
	CoUninitialize();
	return 0;
}

// �����������
CPIV::CPIV() 
{
	SendLog(L"������ DLL ����������");
}

// ����������
CPIV::~CPIV() 
{
	Close();
	CloseProject();
	SendLog(LOG_SLASH);
}

#pragma region SET_PARAMETERS
// ��������� ���� �������� ����������
void CPIV::SetPathToSave(const CString& pathToReport)
{
	SHFILEOPSTRUCT fos;
	ZeroMemory(&fos, sizeof(fos));

	// ��������� ������� ��� �������� ����� ����������
	fos.wFunc = FO_MOVE;
	path.Format(L"%s\\%s", path, BASE_FOLDER);	// � ����� CString ���������� ����� ����� ��� ��������, ������� ������ ���� ��� ����������� �������
	path.AppendChar(0);		// '\0' ����� ������� ������
	path.AppendChar(0);		// '\0' ����� ������� �����
	
	CString to = pathToReport;	// ����������� ���������� path
	to.AppendChar(0);	// ��� ������
	to.AppendChar(0);	// ��� ����� ������

	fos.pFrom = path;
	fos.pTo = to;
	fos.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;

	SHFileOperation(&fos);
	path = pathToReport;	// ��������� ���� �������� ����������
}

// ��������� ����� bNumPK (�������� ��������)
void CPIV::SetStatusNumPK(const bool& status) { bNumPK = status; }
#pragma endregion

#pragma region OPEN_PROJECT
// �������� ������� (������ ���) � ���������� ���� �������� ����������
void CPIV::Open(const vector<CString>& pathToExcel, const CString& pathToReport) 
{
	buffer = pathToExcel;
	path = pathToReport;
	StartOpen();
}

// ������������ ������ ���� ��������
void CPIV::Open(const vector<CString>& pathToExcel)
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
		primary = CreateThread(NULL, 0, PrimaryThread, this, 0, 0);
	}
	else
		SendLog(THREAD_BUSY, true);
}

void CPIV::OpenExcel() 
{
	try 
	{
		CReaderExcel reader; 
		for (size_t i = 0; i < buffer.size(); i++)
			project.books.push_back(reader.getBook(buffer[i]));

		CTest tester;
		project.db = tester.Start(project.books);

		// ��������� ����������
		CReport report;
		CString tPath = path + L"\\" + BASE_FOLDER;
		CreateDirectory(tPath, NULL);
		report.getReport(project, tPath, true);		// true -  ������, false - ��������� ���������
		report.getTxt(project.books, path, bNumPK);
		CloseThread(primary);
		SendLog(L"�������� ������� ���������");	// �����������
	}
	catch (MyException& exc)
	{
		SendLog(exc.GetMsg(), true);
	}
}
#pragma endregion

#pragma region OPEN_PIV
// ������� ��������� ���������:
// ����
void CPIV::Add(const CString& pathToExcel) 
{
	buffer.push_back(pathToExcel);
	StartAdd();
}

// ���������
void CPIV::Add(const vector<CString>& pathToExcel) 
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
		primary = CreateThread(NULL, 0, PrimaryThread, this, 0, 0);
	}
	else
		SendLog(THREAD_BUSY, true);
}

void CPIV::AddExcel() 
{
	try 
	{
		CReport report;
		for (size_t i = 0; i < buffer.size(); i++) 
		{
			bool contain = IsContain(other, buffer[i]);
			CReaderExcel reader;
			bookData book = reader.getBook(buffer[i]);
			contain ? Refresh(other, book) : other.books.push_back(book);
			
			CTest tester;
			bookData& pBook = GetBook(other, buffer[i]);
			errorSet error = tester.Start(pBook);
			contain ? Refresh(other, error) : other.db.push_back(error);

			report.getTxt(book, path, bNumPK);
		}
		CString tPath = path + L"\\" + BASE_FOLDER;
		CreateDirectory(tPath, NULL);
		report.getReport(other, tPath, false);	// true -  ������, false - ��������� ���������
		CloseThread(primary);
		
		SendLog(L"���������� ��� ���������");	// �����������
	}
	catch (MyException& exc) 
	{
		SendLog(exc.GetMsg(), true);
	}
}
#pragma endregion

#pragma region REFRESH_PIV
// ���������� ����������
void CPIV::Refresh(const vector<CString>& pathToExcel)
{
	buffer = pathToExcel;
	StartRefresh();
}

void CPIV::Refresh(const CString& pathToExcel) 
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
		primary = CreateThread(NULL, 0, PrimaryThread, this, 0, 0);
	}
	else
		SendLog(THREAD_BUSY, true);
}

void CPIV::RefreshExcel() 
{
	try 
	{
		CReport report;
		bool flagProj = false, flagOther = false;
		for (size_t i = 0; i < buffer.size(); i++)
		{
			CReaderExcel reader;
			bookData book = reader.getBook(buffer[i]);

			CTest tester;
			if (IsContain(project, buffer[i])) 
			{
				flagProj = true;
				Refresh(project, book);
				bookData& pBook = GetBook(project, buffer[i]);
				errorSet error = tester.Start(pBook);
				Refresh(project, error);
			}
			else if (IsContain(other, buffer[i])) 
			{
				flagOther = true;
				Refresh(other, book);
				bookData& pBook = GetBook(other, buffer[i]);
				errorSet error = tester.Start(pBook);
				Refresh(other, error);
			}
			else
				throw BookNotFound(NameFromPath(buffer[i]));

			report.getTxt(book, path, bNumPK);
		}
		CString tPath = path + L"\\" + BASE_FOLDER;
		if (flagProj)
			report.getReport(project, tPath, true);		// true - ������, false - ��������� ���������
		else if (flagOther)
			report.getReport(other, tPath, false);
	
		CloseThread(primary);
		
		SendLog(L"���������� ��� ���������");	// �����������
	}
	catch (MyException& exc) 
	{
		SendLog(exc.GetMsg(), true);
	}
}
#pragma endregion

#pragma region CLOSE_PIV
// �������� ���� ����������
void CPIV::Close() 
{
	if (!other.books.empty())
		other.books.clear();
	if (!other.db.empty())
		other.db.clear();
	SendLog(L"�������� ��� ���������");	// �����������
}

// ������
void CPIV::Close(const CString& path)
{
	buffer.push_back(path);
	StartClose();
}

// ����������
void CPIV::Close(const vector<CString>& path) 
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
		primary = CreateThread(NULL, 0, PrimaryThread, this, 0, 0);
	}
	else
		SendLog(THREAD_BUSY, true);
}

// �������� �������
void CPIV::CloseProject() 
{
	if (!project.books.empty())
		project.books.clear();
	if (!project.db.empty())
		project.db.clear();
}

void CPIV::CloseExcel() 
{
	for (size_t i = 0; i < buffer.size(); i++) 
	{
		for (list <errorSet>::iterator it = other.db.begin(); it != other.db.end();)
			it->book->name.Compare(NameFromPath(buffer[i])) == 0 ? other.db.erase(it++) : it++;
		for (list <bookData>::iterator it = other.books.begin(); it != other.books.end();)
			it->name.Compare(NameFromPath(buffer[i])) == 0 ? other.books.erase(it++) : it++;
	}
	CloseThread(primary);
	SendLog(L"�������� ��� ���������");	// �����������
}
#pragma endregion

#pragma region SUB_FUNCTION
// ���������� ���������� (������ + ���� ������)
void CPIV::Refresh(pivData& data, const bookData& book, const errorSet& error) 
{
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (book.name.Compare(it->name) == 0)
			*it = book;

	for (list <errorSet>::iterator it = data.db.begin(); it != data.db.end(); it++)
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
void CPIV::Refresh(pivData& data, const errorSet& error) 
{
	for (list <errorSet>::iterator it = data.db.begin(); it != data.db.end(); it++)
		if (error.book->name.Compare(it->book->name) == 0)
			*it = error;
}

// ��������� ������ �� ������ ���������� ���������
bookData& CPIV::GetBook(pivData& data, const CString& path)
{
	bookData& result = *data.books.end();
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (NameFromPath(path).Compare(it->name) == 0)
			result = *it;
	return result;
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

// �������� ��������� � ���������� �������� � ��� �����������
void CPIV::SendLog(const CString& msg, const bool& error) 
{
	while (!logs.st_mutex.try_lock() && logs.status)	// �������� ������������� �������� � ��������� ��������� 
		Sleep(10);
	
	//logs.st_mutex.lock();
	error ? logs.msg = L"��� �������� ��������� ������ (��������� �������� ��. � �����)" : logs.msg = msg;
	
	// ����������� � ����
	SYSTEMTIME st;
	CString str;
	GetLocalTime(&st);
	msg.CompareNoCase(LOG_SLASH) != 0 ?
		str.Format(L"%02d:%02d:%02d %02d/%02d/%d:\t%s\n", st.wHour, st.wMinute, st.wSecond, st.wDay, st.wMonth, st.wYear, msg) :
		str = msg;
	CString logPath;
	logPath.Format(L"%s\\%s\\%s", path, BASE_FOLDER, LOG_FILE_NAME);
	ofstream logStream(logPath, ios::out | ios::app);
	logStream << CT2A(str);
	logStream.close();

	logs.st_mutex.unlock();

	if (error)
		AfxMessageBox(msg, MB_ICONERROR);
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
		piv.SendLog(L"������: ������������ �������!", true);
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
		SendLog(L"������: �� ������� ������� �����!", true);
}
#pragma endregion