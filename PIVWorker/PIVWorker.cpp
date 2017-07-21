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

struct MyData {
	CPIV* object;
};

MyData mD;

DWORD WINAPI PrimaryThread(LPVOID lpParam) {
	CoInitialize(NULL);
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	MyData* myD = static_cast<MyData*>(lpParam);

	Thread(*(myD->object));
	CoUninitialize();
	return 0;
}

// �����������
CPIV::CPIV() { 
	hLogPipe = CreateFile(pipeName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
}

// ����������
CPIV::~CPIV() {
	Close();
	CloseProject();
	CloseHandle(hLogPipe);
}

// ���������� ���� ��� �������� ����������
void CPIV::setPathToSave(const CString& pathToReport) { 
	SHFILEOPSTRUCT fos;
	ZeroMemory(&fos, sizeof(fos));

	// ��������� ������� ��� �������� ����� ����������
	fos.wFunc = FO_MOVE;
	path.Format(L"%s\\Artefacts", path);
	path.AppendChar(0);
	path.AppendChar(0);
	CString to = pathToReport;
	to.AppendChar(0);
	to.AppendChar(0);
	
	fos.pFrom = path;
	fos.pTo = to;
	fos.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;
	
	SHFileOperation(&fos);
	path = pathToReport; 
}

// ���������� ���� bNumPK
void CPIV::setStatusNumPK(const bool& status) { bNumPK = status; }

#pragma region OPEN_PIV
// ������� ��� � ������ ���� ��� ���������� (������, ��������, ������ ����������)
void CPIV::Open(const vector<CString>& pathToExcel, const CString& pathToReport) {
	buffer = pathToExcel;
	path = pathToReport;
	StartOpen();
}

// ������� ��� � ������������ ������ ���� ��� ����������
void CPIV::Open(const vector<CString>& pathToExcel) {
	buffer = pathToExcel;
	StartOpen();
}

// ������ �������� ���
void CPIV::StartOpen() {
	if (getStatusThread(primary)) {
		CloseProject();
		hCmd = command.open;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(THREAD_BUSY);
}

// �������� ���, ���� ������� ����� � ������
void CPIV::OpenExcel() {
	try {
		CReaderExcel reader; // ������
		for (size_t i = 0; i < buffer.size(); i++)
			project.books.push_back(reader.getBook(buffer[i]));

		CTest tester;	// ��������
		project.db = tester.Start(project.books);

		// ��������� ����������
		CReport report;
		CString tPath = path + L"\\Artefacts";
		CreateDirectory(tPath, NULL);
		tPath.Format(L"%s\\Project", tPath);
		CreateDirectory(tPath, NULL);
		report.getReport(project, tPath);
		report.getTxt(project.books, path, bNumPK);
		closeThread(primary);
		
		WriteLog("�������� ������� ���������");	// �����������
		//AfxMessageBox(L"�������� ���������!", MB_ICONINFORMATION);
	}
	catch (MyException& exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
}
#pragma endregion

#pragma region ADD_PIV
// �������� ���
void CPIV::Add(const CString& pathToExcel) {
	buffer.push_back(pathToExcel);
	StartAdd();
}

// �������� ��� (����������)
void CPIV::Add(const vector<CString>& pathToExcel) {
	buffer = pathToExcel;
	StartAdd();
}

// ������ ���������� ���
void CPIV::StartAdd() {
	if (getStatusThread(primary)) {
		hCmd = command.add;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(THREAD_BUSY);
}

// ���������� ���, ���� ������� ����� � ������
void CPIV::AddExcel() {
	try {
		CReport report;
		for (size_t i = 0; i < buffer.size(); i++) {
			if (IsContain(project, buffer[i]))
				continue;

			// ���������� ��� � ������
			bool contain = IsContain(other, buffer[i]);
			CReaderExcel reader;
			bookData book = reader.getBook(buffer[i]);
			contain ? Refresh(other, book) : other.books.push_back(book);
			
			CTest tester;
			errorSet error = tester.Start(getBook(other, buffer[i]));
			contain ? Refresh(other, error) : other.db.push_back(error);

			report.getTxt(book, path, bNumPK);
		}
		CString tPath = path + L"\\Artefacts";
		CreateDirectory(tPath, NULL);
		tPath.Format(L"%s\\Other", tPath);
		CreateDirectory(tPath, NULL);
		report.getReport(other, tPath);	// ���������� ������
		closeThread(primary);
		
		WriteLog("���������� ��� ���������");	// �����������
		//AfxMessageBox(L"���������� ���������!", MB_ICONINFORMATION);
	}
	catch (MyException& exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
}
#pragma endregion

#pragma region REFRESH_PIV
// �������� ���
void CPIV::Refresh(const CString& pathToExcel) {
	buffer.push_back(pathToExcel);
	StartRefresh();
}

// �������� ��������� ���
void CPIV::Refresh(const vector<CString>& pathToExcel) {
	buffer = pathToExcel;
	StartRefresh();
}

// ������ ���������� ���
void CPIV::StartRefresh() {
	if (getStatusThread(primary)) {
		hCmd = command.refresh;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(THREAD_BUSY);
}

// ���������� ���
void CPIV::RefreshExcel() {
	try {
		CReport report;
		bool flagProj = false, flagOther = false;
		for (size_t i = 0; i < buffer.size(); i++) {
			// ���������� ��� � ������
			CReaderExcel reader;
			bookData book = reader.getBook(buffer[i]);

			CTest tester;
			bool contain = IsContain(project, buffer[i]);
			if (IsContain(project, buffer[i])) {
				flagProj = true;
				Refresh(project, book);
				errorSet error = tester.Start(getBook(project, buffer[i]));
				Refresh(project, error);
			}
			else if (IsContain(other, buffer[i])) {
				flagOther = true;
				Refresh(project, book);
				errorSet error = tester.Start(getBook(project, buffer[i]));
				Refresh(project, error);
			}
			else {
				BookNotFound exc;
				exc.setName(nameFromPath(buffer[i]));
				throw exc;
			}
			report.getTxt(book, path, bNumPK);
		}
		if (flagProj)
			report.getReport(project, path);
		if (flagOther)
			report.getReport(other, path);
		closeThread(primary);
		
		WriteLog("���������� ��� ���������");	// �����������
		//AfxMessageBox(L"���������� ���������!", MB_ICONINFORMATION);
	}
	catch (MyException& exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
}
#pragma endregion

#pragma region CLOSE_PIV
// ������� ���
void CPIV::Close() {
	other.books.clear();
	other.db.clear();
}

// �������� ��� �������
void CPIV::CloseProject() {
	project.books.clear();
	project.db.clear();
}

// ������� ���� ��� � ��� ����� � ���� ������
void CPIV::Close(const CString& path) {
	buffer.push_back(path);
	StartClose();
}

// ������� ��������� ��� � �� ������ � ���� ������
void CPIV::Close(const vector<CString>& path) {
	buffer = path;
	StartClose();
}

// ������ �������� ���
void CPIV::StartClose() {
	if (getStatusThread(primary)) {
		hCmd = command.close;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(THREAD_BUSY);
}

// �������� ���, ���� ������� ����� � ������
void CPIV::CloseExcel() {
	for (size_t i = 0; i < buffer.size(); i++) {
		for (list <errorSet>::iterator it = other.db.begin(); it != other.db.end();)
			it->book->name.Compare(nameFromPath(buffer[i])) == 0 ? other.db.erase(it++) : it++;
		for (list <bookData>::iterator it = other.books.begin(); it != other.books.end();)
			it->name.Compare(nameFromPath(buffer[i])) == 0 ? other.books.erase(it++) : it++;
	}
	closeThread(primary);
	WriteLog("�������� ���������");	// �����������
}
#pragma endregion

#pragma region subFunc
// ������������� ������� ��� ������� �������� � ������
void Thread(CPIV& piv) {
	if (piv.hCmd == piv.command.open)
		piv.OpenExcel();
	else if (piv.hCmd == piv.command.add)
		piv.AddExcel();
	else if (piv.hCmd == piv.command.refresh)
		piv.RefreshExcel();
	else if (piv.hCmd == piv.command.close)
		piv.CloseExcel();
	else
		AfxMessageBox(L"������������ �������!", MB_ICONERROR);
}

// ���������� ��� � ������
void CPIV::Refresh(pivData& data, const bookData& book, const errorSet& error) {
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (book.name.Compare(it->name) == 0)
			*it = book;

	for (list <errorSet>::iterator it = data.db.begin(); it != data.db.end(); it++)
		if (error.book->name.Compare(it->book->name) == 0)
			*it = error;
}

// ���������� ���
void CPIV::Refresh(pivData& data, const bookData& book) {
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (book.name.Compare(it->name) == 0)
			*it = book;
}

// ���������� ���
void CPIV::Refresh(pivData& data, const errorSet& error) {
	for (list <errorSet>::iterator it = data.db.begin(); it != data.db.end(); it++)
		if (error.book->name.Compare(it->book->name) == 0)
			*it = error;
}

// �������� ������ �� �����
bookData& CPIV::getBook(pivData& data, const CString& path) {
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (nameFromPath(path).Compare(it->name) == 0)
			return *it;
}

// ���������� ����� ����� �� ��� ����
CString CPIV::nameFromPath(const CString& path) {
	int startPos = path.ReverseFind(L'\\') + 1;
	return path.Mid(startPos, path.GetLength());
}

// �������� ������� ����� � ������
bool CPIV::IsContain(pivData& data, const CString& path) {
	bool result = false;
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		nameFromPath(path).Compare(it->name) == 0 ? result = true : result = result;
	
	return result;
}

// �������� ����������� ������
bool CPIV::getStatusThread(const HANDLE& h) {
	DWORD ty;
	bool result = true;
	GetExitCodeThread(h, &ty);
	ty != STILL_ACTIVE ? result = true : result = false;
	return result;
}

// �������� ������
void CPIV::closeThread(HANDLE& h) {
	buffer.clear();
	buffer.shrink_to_fit();
	if (h != 0) {
		CloseHandle(h);
		h = NULL;
	}
	else
		AfxMessageBox(L"�� ������� ������� �����. �� � �� ����������.", MB_ICONERROR);
}

// ������ � ����������� �����
void CPIV::WriteLog(char szBuf[256]) {
	if (hLogPipe != INVALID_HANDLE_VALUE) {
		DWORD  cbWritten;
		WriteFile(hLogPipe, szBuf, strlen(szBuf) + 1, &cbWritten, NULL);
	}
}
#pragma endregion