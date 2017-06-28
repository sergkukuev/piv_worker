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

// ������������� CPIVWorkerApp
int main() {
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr) {
		// ���������������� MFC, � ����� ������ � ��������� �� ������� ��� ����
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0)) {
			// TODO: �������� ��� ������ �������������� ����� ������������
			wprintf(L"����������� ������: ���� ��� ������������� MFC\n");
			nRetCode = 1;
		}
		else {
			// TODO: �������� ���� ��� ��� ����������.
		}
	}
	else {
		// TODO: �������� ��� ������ �������������� ����� ������������
		wprintf(L"����������� ������: ��������� ���������� GetModuleHandle\n");
		nRetCode = 1;
	}

	return nRetCode;
}

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
CPIVWorker::~CPIVWorker() { CloseExcel(); }

// ��������� ������� ������ ������ ���������
void CPIVWorker::setStatusNumPK(const bool& status) { bNumPK = status; }

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
	else
		AfxMessageBox(_T("����� �����! ��������� ��������� ��������!"));
}

// ������ ���
void CPIVWorker::Read() {
	try {
		CReaderExcel reader;	// ����� ������ ����
		for (size_t i = 0; i < buffer.size(); i++)
			if (!findBook(buffer[i]))
				books.push_back(reader.getBook(buffer[i]));
	
		AfxMessageBox(_T("������ ��������� �������!"), MB_ICONINFORMATION);
	}
	catch (MyException &exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}

	buffer.clear();
	buffer.shrink_to_fit();
	closeThread(primary);
}

#pragma endregion

#pragma region TEST

// ������ �������� ���� ����
void CPIVWorker::TestExcel() {
	if (getStatusThread(primary)) {
		hCmd = command.test;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(_T("����� �����! ��������� ��������� ��������!"));
}

/*// �������� ����� �����
void CPIVWorker::TestExcel(const CString& pathToExcel) {
	if (getStatusThread(primary)) {
		buffer.push_back(pathToExcel);
		hCmd = command.test;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(_T("����� �����! ��������� ��������� ��������!"));
}

// �������� ��������� ����
void CPIVWorker::TestExcel(const vector <CString>& pathToExcel) {
	if (getStatusThread(primary)) {
		buffer = pathToExcel;
		hCmd = command.test;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(_T("����� �����! ��������� ��������� ��������!"));
}*/

// �������� ���� �������� ����
void CPIVWorker::Test() {
	try {
		for (size_t i = 0; i < books.size(); i++) {
			CTest tester;	// ����� �������� ����
			Db = tester.Start(books);
		}
		AfxMessageBox(_T("�������� ���������� ��������� �������!"), MB_ICONINFORMATION);
	}
	catch (MyException &exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
	closeThread(primary);
}

#pragma endregion


#pragma region REPORT

// ������ �������� ������
void CPIVWorker::Report(const CString& path) {
	this->path = path;
	StartReport();
}

// ������ �������� ������ (����������)
void CPIVWorker::Report() { StartReport(); }

// ������ �������� ������ �� �������
void CPIVWorker::StartReport()
{
	if (path.IsEmpty())
		AfxMessageBox(_T("����� ��� ���������� ������ �� �������!"));
	else {
		if (getStatusThread(primary)) {
			hCmd = command.report;
			mD.object = this;
			primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
		}
		else
			AfxMessageBox(_T("����� �����! ��������� ��������� ��������!"));
	}
}

// �������� ������
void CPIVWorker::MakeReport() {
	CReport report;
	CString pathFile, msg;
	report.getReport(books, Db, path);
	pathFile.Format(_T("%s\\�����.html"), path);
	msg.Format(_T("�������� ������ ���������!\n\n������������: %s\n������� ��� ���������?"), pathFile);
	if (AfxMessageBox(msg, MB_YESNO | MB_ICONQUESTION) == IDYES)
		ShellExecute(0, L"open", pathFile, NULL, NULL, SW_NORMAL);	
}

#pragma endregion

#pragma region CLOSE

// �������� ���� ���
void CPIVWorker::CloseExcel() {
	books.clear();
	Db.clear();
	buffer.clear();
	buffer.shrink_to_fit();
}

// �������� ����� ���
void CPIVWorker::CloseExcel(const CString& pathToExcel) {
	buffer.push_back(pathToExcel);
	StartClose();
}

// �������� ����� ���
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
	else
		AfxMessageBox(_T("����� �����! ��������� ��������� ��������!"));
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
				findBook(nameFromPath(buffer[i])) ? Db.erase(it) : it++;

			for (list <bookData>::iterator it = books.begin(); it != books.end();)
				findBook(nameFromPath(buffer[i])) ? books.erase(it++) : it++;
		}
		buffer.clear();
		buffer.shrink_to_fit();
	}
}

#pragma endregion

#pragma region TXT

// �������� txt ������
void CPIVWorker::CreateTxt(CString path) {
	this->path = path;
	StartTxt();
}

// �������� txt ������ (����������)
void CPIVWorker::CreateTxt() { StartTxt(); }

// // ������ �������� txt ������
void CPIVWorker::StartTxt() {
	if (path.IsEmpty())
		AfxMessageBox(_T("����� ��� ���������� txt ������ �� �������!"));
	else {
		if (getStatusThread(primary)) {
			hCmd = command.txt;
			mD.object = this;
			primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
		}
		else
			AfxMessageBox(_T("����� �����! ��������� ��������� ��������!"));
	}
}

// ������ ��������� txt ������ 
void CPIVWorker::GenerateTxt() {
	CReport report;
	CString msg;
	report.getTxt(books,path, bNumPK);
	msg.Format(_T("�������� txt ������ ���������!\n\n������������: %s\n������� ��� ���������?"), path);
	if (AfxMessageBox(msg, MB_YESNO | MB_ICONQUESTION) == IDYES);
		ShellExecute(0, L"open", path, NULL, NULL, SW_NORMAL);
}

#pragma endregion

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
	else
		AfxMessageBox(_T("������������ �������!"), MB_ICONWARNING);
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
	if (h != 0) {
		CloseHandle(h);
		h = NULL;
	}
	else
		AfxMessageBox(_T("�� ������� ������� �����! (�������� �� ���� �� ����������)"));
}