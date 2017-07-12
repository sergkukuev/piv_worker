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

struct MyData {
	CPIVWorker* object;
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
CPIVWorker::CPIVWorker() { }

// ����������
CPIVWorker::~CPIVWorker() { 
	Close();
}

// ��������� ������� ������ ������ ���������
void CPIVWorker::setStatusNumPK(const bool& status) { bNumPK = status; }

// ��������� ����� ��� ����������
void CPIVWorker::setPathToSave(const CString& path) { this->path = path; }

// ��������� ������� ������
bool CPIVWorker::getStatusThread(CString& status) { 
	return getStatusThread(primary);
};

#pragma region LOAD

// �������� ������ ��� ��� ������� � �� ���������� ��������, � ������ ����������
void CPIVWorker::LoadExcel(const vector <CString>& pathToExcel, const CString& pathToReport) {
	if (getStatusThread(primary)) {
		Close();
		buffer = pathToExcel;
		path = pathToReport;

		hCmd = command.load;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else {
		AfxMessageBox(THREAD_BUSY);
	}
}

// �������� ����������, ��������� � ����� �������
void CPIVWorker::Load() {
	Read(false);
	buffer.clear();
	buffer.shrink_to_fit();
	Test(false);
	MakeReport(false);
	GenerateTxt(false);
	closeThread(primary);
}

#pragma endregion

#pragma region READ

// ��������� ����� ��� ������
void CPIVWorker::ReadExcel(const vector <CString>& pathToExcel) {
	buffer = pathToExcel;
	StartRead();
}

// ��������� ���� ��� ������
void CPIVWorker::ReadExcel(const CString& pathToExcel) {
	buffer.push_back(pathToExcel);
	StartRead();
}

// ������ ������
void CPIVWorker::StartRead() {
	if (getStatusThread(primary)) {
		hCmd = command.open;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else {
		AfxMessageBox(THREAD_BUSY);
	}
}

// ������ ���
void CPIVWorker::Read(const bool& hClose) {
	try {
		CReaderExcel reader;	// ����� ������ ����
		for (size_t i = 0; i < buffer.size(); i++) {
			bookData book = reader.getBook(buffer[i]);
			findBook(buffer[i]) ? Refresh(book) : books.push_back(book);
		}
		AfxMessageBox(L"������ ��������� �������!", MB_ICONINFORMATION);
	}
	catch (MyException &exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
	if (hClose)
		closeThread(primary);
}

// ���������� ���������
void CPIVWorker::Refresh(const bookData& book) {
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++)
		if (it->name.CompareNoCase(book.name) == 0) 
			*it = book;
}

#pragma endregion

#pragma region TEST

// �������� ���� ����
void CPIVWorker::TestExcel() { StartTest(); }

// �������� ����� �����
void CPIVWorker::TestExcel(const CString& path) {
	buffer.push_back(path);
	StartTest();
}

// �������� ��������� ����
void CPIVWorker::TestExcel(const vector<CString>& path) {
	buffer = path;
	StartTest();
}

// ������ �������� ����
void CPIVWorker::StartTest() {
	if (getStatusThread(primary)) {
		hCmd = command.test;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else {
		AfxMessageBox(THREAD_BUSY);
	}
}

// �������� ����
void CPIVWorker::Test(const bool& hClose) {
	try {
		CTest tester;
		
		if (buffer.empty())
			// �������� ���� ����
			Db = tester.Start(books);
		else {
			// �������� ��������� ����
			for (size_t i = 0; i < buffer.size(); i++)
				if (findBook(buffer[i]))
					Refresh(tester.Start(getBook(buffer[i])));
		}
		AfxMessageBox(L"�������� ���������� ��������� �������!", MB_ICONINFORMATION);
	}
	catch (MyException &exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
	if (hClose)
		closeThread(primary);
}

// ���������� ������ �� �����
void CPIVWorker::Refresh(const errorSet& set) {
	for (list <errorSet>::iterator it = Db.begin(); it != Db.end(); it++)
		if (it->book->name.CompareNoCase(set.book->name) == 0)
			*it = set;
}

#pragma endregion

#pragma region REPORT

// �������� ������
void CPIVWorker::Report() { StartReport(); }

// ������ �������� ������ �� �������
void CPIVWorker::StartReport() {
	if (getStatusThread(primary)) {
		hCmd = command.report;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else {
		AfxMessageBox(THREAD_BUSY);
	}
}

// �������� ������
void CPIVWorker::MakeReport(const bool& hClose) {
	try {
		CReport report;
		report.getReport(books, Db, path);
		CString pathFile, msg;
		pathFile.Format(_T("%s\\�����.html"), path);
		msg.Format(_T("�������� ������ ���������!\n\n������������: %s\n������� ��� ���������?"), pathFile);
		if (AfxMessageBox(msg, MB_YESNO | MB_ICONQUESTION) == IDYES)
			ShellExecute(0, L"open", pathFile, NULL, NULL, SW_NORMAL);
	}
	catch (MyException &exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
	if (hClose)
		closeThread(primary);
}

#pragma endregion

#pragma region TXT

// �������� txt ��� ���������� ���������
void CPIVWorker::CreateTxt(const CString& path) {
	buffer.push_back(path);
	StartTxt();
}

// �������� ��� ��������� ����������
void CPIVWorker::CreateTxt(const vector<CString>& path) {
	buffer = path;
	StartTxt();
}

// �������� ��� ���� ����������
void CPIVWorker::CreateTxt() { StartTxt(); }

// // ������ �������� txt ������
void CPIVWorker::StartTxt() {
	if (getStatusThread(primary)) {
		hCmd = command.txt;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else {
		AfxMessageBox(THREAD_BUSY);
	}
}

// ������ ��������� txt ������ 
void CPIVWorker::GenerateTxt(const bool& hClose) {
	try {
		CReport report;
		if (buffer.empty())
			report.getTxt(books, path, bNumPK);
		else
			for (size_t i = 0; i < buffer.size(); i++)
				report.getTxt(getBook(buffer[i]), path, bNumPK);
		
		AfxMessageBox(L"�������� txt ������ ���������!", MB_ICONINFORMATION);
	}
	catch (MyException &exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
	if (hClose)
		closeThread(primary);
}

#pragma endregion

#pragma region CLOSE

// �������� ���� ���
void CPIVWorker::CloseExcel() { StartClose(); }

// �������� ����� ���
void CPIVWorker::CloseExcel(const CString& pathToExcel) {
	buffer.push_back(pathToExcel);
	StartClose();
}

// �������� ���� ���
void CPIVWorker::CloseExcel(const vector <CString>& pathToExcel) {
	buffer = pathToExcel;
	StartClose();
}

// ������ ��������
void CPIVWorker::StartClose() {
	if (getStatusThread(primary)) {
		hCmd = command.close;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else {
		AfxMessageBox(THREAD_BUSY);
	}
}

// �������� ����(�) ���
void CPIVWorker::Close() {
	if (buffer.empty()) {
		books.clear();
		Db.clear();
	}
	else {
		for (size_t i = 0; i < buffer.size(); i++) {
			for (list <errorSet>::iterator it = Db.begin(); it != Db.end();)
				it->book->name.CompareNoCase(nameFromPath(buffer[i])) == 0 ? Db.erase(it++) : it++;
			for (list <bookData>::iterator it = books.begin(); it != books.end();)
				it->name.CompareNoCase(nameFromPath(buffer[i])) == 0 ? books.erase(it++) : it++;
		}
	}
	closeThread(primary);
}

#pragma endregion

// ��������� ����� �� ������ �������� ����
bookData CPIVWorker::getBook(const CString& path) {
	for (list<bookData>::iterator it = books.begin(); it != books.end(); it++)
		if (nameFromPath(path).CompareNoCase(it->name) == 0)
			return *it;
	
	UndefinedBook exc;
	exc.SetName(nameFromPath(path));
	throw exc;
}

// ����� ������� ����������� ����� � ���� ����
bool CPIVWorker::findBook(const CString& pathToExcel) {
	bool result = false;
	for (list<bookData>::iterator it = books.begin(); it != books.end(); it++)
		if (nameFromPath(pathToExcel).Compare(it->name) == 0)
			result = true;

	return result;
}

// ��������� ����� �����
CString CPIVWorker::nameFromPath(const CString& path) {
	int startPos = path.ReverseFind(_T('\\')) + 1;
	return path.Mid(startPos, path.GetLength());
}

// ������������� ������� ��� ������� ������������ ��������
void Thread(CPIVWorker& piv) {
	if (piv.hCmd == piv.command.open)
		piv.Read();
	else if (piv.hCmd == piv.command.test)
		piv.Test();
	else if (piv.hCmd == piv.command.report)
		piv.MakeReport();
	else if (piv.hCmd == piv.command.txt)
		piv.GenerateTxt();
	else if (piv.hCmd == piv.command.close)
		piv.Close();
	else if (piv.hCmd == piv.command.load)
		piv.Load();
	else {
		AfxMessageBox(L"������������ �������!", MB_ICONWARNING);
	}
}

// �������� ������ �� �����������
bool CPIVWorker::getStatusThread(const HANDLE& h) {
	DWORD ty;
	bool result = true;
	GetExitCodeThread(h, &ty);
	(ty != STILL_ACTIVE) ? result = true : result = false;
	return result;
}

// �������� ������
void CPIVWorker::closeThread(HANDLE& h) {
	buffer.clear();			// ������� ������
	buffer.shrink_to_fit();
	if (h != 0) {
		CloseHandle(h);
		h = NULL;
	}
	else {
		AfxMessageBox(L"�� ������� ������� �����. �� � �� ����������.");
	}
}