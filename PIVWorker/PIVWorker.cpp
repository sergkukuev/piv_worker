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
CPIVWorker::~CPIVWorker() {
	path.clear();
	books.clear();
	errorDB.syntax.clear();
	errorDB.simantic.clear();
	errorDB.warning.clear();
	buffer.clear();
}

// ��������� ������� ������ ������ ���������
void CPIVWorker::setStatusNumPK(bool status) { bNumPK = status; }

#pragma region READ

// ��������� ����� ��� ������
void CPIVWorker::ReadExcel(vector <CString> pathToExcel) {
	buffer = pathToExcel;
	StartRead();
}

// ��������� ���� ��� ������
void CPIVWorker::ReadExcel(CString pathToExcel) {
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
void CPIVWorker::ReadExcel() {
	try {
		for (size_t i = 0; i < buffer.size(); i++) {
			CReaderExcel reader;	// ����� ������ ����

			if (findIndexBook(buffer[i]) == -1) { 	// ���� ����� ����� ��� ���
				books.push_back(reader.getBook(buffer[i]));
				path.push_back(buffer[i]);
			}
		}
		AfxMessageBox(_T("������ ��������� �������!"), MB_ICONINFORMATION);
	}
	catch (MyException &exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}

	buffer.clear();
	closeThread(primary);
}

#pragma endregion

#pragma region TEST

// ������ ������������
void CPIVWorker::TestExcel() {
	if (getStatusThread(primary)) {
		hCmd = command.test;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(_T("����� �����! ��������� ��������� ��������!"));
}

// �������� ���� �������� ����
void CPIVWorker::Test() {
	try {
		for (size_t i = 0; i < books.size(); i++) {
			CTest tester;	// ����� �������� ����
			errorOneSet errBook;

			errBook = tester.Start(books[i]);

			int indx = findReportBook(errBook.syntax.name);	// ����� � ���� ������� ������ �����

			// ���������� ������
			if (indx != -1) { 
				errorDB.syntax[indx] = errBook.syntax;
				errorDB.simantic[indx] = errBook.simantic;
				errorDB.warning[indx] = errBook.warning;
			}
			else { // ���������� ������ ������
				errorDB.syntax.push_back(errBook.syntax);
				errorDB.simantic.push_back(errBook.simantic);
				errorDB.warning.push_back(errBook.warning);
			}
				
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
void CPIVWorker::Report(CString path) {
	pathReport = path;
	StartReport();
}

// ������ �������� ������ (����������)
void CPIVWorker::Report() { StartReport(); }

// ������ �������� ������ �� �������
void CPIVWorker::StartReport()
{
	if (pathReport.IsEmpty())
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
	report.getReport(books, errorDB, pathReport);
	pathFile.Format(_T("%s\\�����.html"), pathReport);
	msg.Format(_T("�������� ������ ���������!\n\n������������: %s\n������� ��� ���������?"), pathReport);
	if (AfxMessageBox(msg, MB_YESNO | MB_ICONQUESTION) == IDYES);
		ShellExecute(0, L"open", pathFile, NULL, NULL, SW_NORMAL);	
}

#pragma endregion

#pragma region CLOSE

// �������� ���� ���
void CPIVWorker::CloseExcel(vector <CString> pathToExcel) {
	buffer = pathToExcel;
	StartClose();
}

// �������� ����� ���
void CPIVWorker::CloseExcel(CString pathToExcel) {
	buffer.push_back(pathToExcel);
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
void CPIVWorker::CloseExcel() {
	for (size_t i = 0; i < buffer.size(); i++) {
		int iBook = findIndexBook(buffer[i]);

		if (iBook != -1) {
			int iReport = findReportBook(books[iBook].name);

			// ������ � ���� ������
			if (iReport != -1) {
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

#pragma endregion

#pragma region TXT

// �������� txt ������
void CPIVWorker::CreateTxt(CString path) {
	pathReport = path;
	StartTxt();
}

// �������� txt ������ (����������)
void CPIVWorker::CreateTxt() { StartTxt(); }

// // ������ �������� txt ������
void CPIVWorker::StartTxt() {
	if (pathReport.IsEmpty())
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
	report.getTxt(books,pathReport, bNumPK);
	msg.Format(_T("�������� txt ������ ���������!\n\n������������: %s\n������� ��� ���������?"), pathReport);
	if (AfxMessageBox(msg, MB_YESNO | MB_ICONQUESTION) == IDYES);
		ShellExecute(0, L"open", pathReport, NULL, NULL, SW_NORMAL);
}

#pragma endregion

// ����� ������� ����������� ����� � ���� ������
int CPIVWorker::findReportBook(CString name) {
	for (size_t i = 0; i < errorDB.syntax.size(); i++)
		if (name.Compare(errorDB.syntax[i].name) == 0)
			return i;

	return -1;
}

// ����� ������� ����������� ����� � ���� ����
int CPIVWorker::findIndexBook(CString pathToExcel) {
	for (size_t i = 0; i < path.size(); i++)
		if (pathToExcel.Compare(path[i]) == 0)
			return i;

	return -1;
}

// ������������� ������� ��� ������� ������������ ��������
void Thread(CPIVWorker& piv) {
	if (piv.hCmd == piv.command.open)
		piv.ReadExcel();
	else if (piv.hCmd == piv.command.test)
		piv.Test();
	else if (piv.hCmd == piv.command.report)
		piv.MakeReport();
	else if (piv.hCmd == piv.command.txt)
		piv.GenerateTxt();
	else if (piv.hCmd == piv.command.close)
		piv.CloseExcel();
	else
		AfxMessageBox(_T("������������ �������!"), MB_ICONWARNING);
}

// �������� ������ �� �����������
bool CPIVWorker::getStatusThread(HANDLE h) {
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
void CPIVWorker::closeThread(HANDLE& h) {
	if (h != 0) {
		CloseHandle(h);
		h = NULL;
	}
	else
		AfxMessageBox(_T("�� ������� ������� �����! (�������� �� ���� �� ����������)"));
}