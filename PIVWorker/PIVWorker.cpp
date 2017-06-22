// PIVWorker.cpp: определяет процедуры инициализации для DLL.
//

#include "stdafx.h"
#include "PIVWorker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: если эта библиотека DLL динамически связана с библиотеками DLL MFC,
//		все функции, экспортированные из данной DLL-библиотеки, которые выполняют вызовы к
//		MFC, должны содержать макрос AFX_MANAGE_STATE в
//		самое начало функции.
//
//		Например:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// тело нормальной функции
//		}
//
//		Важно, чтобы данный макрос был представлен в каждой
//		функции до вызова MFC.  Это означает, что
//		он должен быть первым оператором 
//		функции и предшествовать даже любым объявлениям переменных объекта,
//		поскольку их конструкторы могут выполнять вызовы к MFC
//		DLL.
//
//		В Технических указаниях MFC 33 и 58 содержатся более
//		подробные сведения.
//


// Единственный объект CPIVWorkerApp
CWinApp theApp;

struct MyData {
	CPIVWorker* object;
};

MyData mD;

// инициализация CPIVWorkerApp
int main() {
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr) {
		// инициализировать MFC, а также печать и сообщения об ошибках про сбое
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0)) {
			// TODO: измените код ошибки соответственно своим потребностям
			wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
			nRetCode = 1;
		}
		else {
			// TODO: Вставьте сюда код для приложения.
		}
	}
	else {
		// TODO: Измените код ошибки соответственно своим потребностям
		wprintf(L"Критическая ошибка: неудачное завершение GetModuleHandle\n");
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

// Конструктор
CPIVWorker::CPIVWorker() { }

// Деструктор
CPIVWorker::~CPIVWorker() {
	path.clear();
	books.clear();
	errorDB.syntax.clear();
	errorDB.simantic.clear();
	errorDB.warning.clear();
	buffer.clear();
}

// Установка статуса записи номера подкардра
void CPIVWorker::setStatusNumPK(bool status) { bNumPK = status; }

#pragma region READ

// Получение путей для чтения
void CPIVWorker::ReadExcel(vector <CString> pathToExcel) {
	buffer = pathToExcel;
	StartRead();
}

// Получение пути для чтения
void CPIVWorker::ReadExcel(CString pathToExcel) {
	buffer.push_back(pathToExcel);
	StartRead();
}

// Начало чтения
void CPIVWorker::StartRead() {
	if (getStatusThread(primary)) {
		hCmd = command.open;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(_T("Поток занят! Подождите окончания процесса!"));
}

// Чтение ПИВ
void CPIVWorker::ReadExcel() {
	try {
		for (size_t i = 0; i < buffer.size(); i++) {
			CReaderExcel reader;	// класс чтения книг

			if (findIndexBook(buffer[i]) == -1) { 	// Если такой книги еще нет
				books.push_back(reader.getBook(buffer[i]));
				path.push_back(buffer[i]);
			}
		}
		AfxMessageBox(_T("Чтение завершено успешно!"), MB_ICONINFORMATION);
	}
	catch (MyException &exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}

	buffer.clear();
	closeThread(primary);
}

#pragma endregion

#pragma region TEST

// Начало тестирования
void CPIVWorker::TestExcel() {
	if (getStatusThread(primary)) {
		hCmd = command.test;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(_T("Поток занят! Подождите окончания процесса!"));
}

// Проверка всех открытых книг
void CPIVWorker::Test() {
	try {
		for (size_t i = 0; i < books.size(); i++) {
			CTest tester;	// класс проверки книг
			errorOneSet errBook;

			errBook = tester.Start(books[i]);

			int indx = findReportBook(errBook.syntax.name);	// поиск в базе отчетов данной книги

			// Обновление отчета
			if (indx != -1) { 
				errorDB.syntax[indx] = errBook.syntax;
				errorDB.simantic[indx] = errBook.simantic;
				errorDB.warning[indx] = errBook.warning;
			}
			else { // Добавление нового отчета
				errorDB.syntax.push_back(errBook.syntax);
				errorDB.simantic.push_back(errBook.simantic);
				errorDB.warning.push_back(errBook.warning);
			}
				
		}
		AfxMessageBox(_T("Проверка протоколов завершена успешно!"), MB_ICONINFORMATION);
	}
	catch (MyException &exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
	closeThread(primary);
}

#pragma endregion

#pragma region REPORT

// Начало создания отчета
void CPIVWorker::Report(CString path) {
	pathReport = path;
	StartReport();
}

// Начало создания отчета (перегрузка)
void CPIVWorker::Report() { StartReport(); }

// Начало создания отчета об ошибках
void CPIVWorker::StartReport()
{
	if (pathReport.IsEmpty())
		AfxMessageBox(_T("Папка для сохранения отчета не указана!"));
	else {
		if (getStatusThread(primary)) {
			hCmd = command.report;
			mD.object = this;
			primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
		}
		else
			AfxMessageBox(_T("Поток занят! Подождите окончания процесса!"));
	}
}

// Создание отчета
void CPIVWorker::MakeReport() {
	CReport report;
	CString pathFile, msg;
	report.getReport(books, errorDB, pathReport);
	pathFile.Format(_T("%s\\Отчет.html"), pathReport);
	msg.Format(_T("Создание отчета завершено!\n\nРасположение: %s\nОткрыть для просмотра?"), pathReport);
	if (AfxMessageBox(msg, MB_YESNO | MB_ICONQUESTION) == IDYES);
		ShellExecute(0, L"open", pathFile, NULL, NULL, SW_NORMAL);	
}

#pragma endregion

#pragma region CLOSE

// Закрытие книг ПИВ
void CPIVWorker::CloseExcel(vector <CString> pathToExcel) {
	buffer = pathToExcel;
	StartClose();
}

// Закрытие книги ПИВ
void CPIVWorker::CloseExcel(CString pathToExcel) {
	buffer.push_back(pathToExcel);
	StartClose();
}

// Начало закрытия
void CPIVWorker::StartClose() {
	if (getStatusThread(primary)) {
		hCmd = command.close;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(_T("Поток занят! Подождите окончания процесса!"));
}

// Закрытие книг(и) ПИВ
void CPIVWorker::CloseExcel() {
	for (size_t i = 0; i < buffer.size(); i++) {
		int iBook = findIndexBook(buffer[i]);

		if (iBook != -1) {
			int iReport = findReportBook(books[iBook].name);

			// Чистим в базе ошибок
			if (iReport != -1) {
				errorDB.syntax.erase(errorDB.syntax.begin() + iReport);
				vector<errorBookData>(errorDB.syntax).swap(errorDB.syntax);

				errorDB.simantic.erase(errorDB.simantic.begin() + iReport);
				vector<errorBookData>(errorDB.simantic).swap(errorDB.simantic);

				errorDB.warning.erase(errorDB.warning.begin() + iReport);
				vector<errorBookData>(errorDB.warning).swap(errorDB.warning);
			}

			// Чистим пути и книжку
			books.erase(books.begin() + iBook);
			vector<bookData>(books).swap(books);

			path.erase(path.begin() + iBook);
			vector<CString>(path).swap(path);
		}
	}
}

#pragma endregion

#pragma region TXT

// Создание txt файлов
void CPIVWorker::CreateTxt(CString path) {
	pathReport = path;
	StartTxt();
}

// Создание txt файлов (перегрузка)
void CPIVWorker::CreateTxt() { StartTxt(); }

// // Начало создания txt файлов
void CPIVWorker::StartTxt() {
	if (pathReport.IsEmpty())
		AfxMessageBox(_T("Папка для сохранения txt файлов не указана!"));
	else {
		if (getStatusThread(primary)) {
			hCmd = command.txt;
			mD.object = this;
			primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
		}
		else
			AfxMessageBox(_T("Поток занят! Подождите окончания процесса!"));
	}
}

// Начало генерации txt файлов 
void CPIVWorker::GenerateTxt() {
	CReport report;
	CString msg;
	report.getTxt(books,pathReport, bNumPK);
	msg.Format(_T("Создание txt файлов завершено!\n\nРасположение: %s\nОткрыть для просмотра?"), pathReport);
	if (AfxMessageBox(msg, MB_YESNO | MB_ICONQUESTION) == IDYES);
		ShellExecute(0, L"open", pathReport, NULL, NULL, SW_NORMAL);
}

#pragma endregion

// Поиск индекса прочитанной книги в базе ошибок
int CPIVWorker::findReportBook(CString name) {
	for (size_t i = 0; i < errorDB.syntax.size(); i++)
		if (name.Compare(errorDB.syntax[i].name) == 0)
			return i;

	return -1;
}

// Поиск индекса прочитанной книги в базе книг
int CPIVWorker::findIndexBook(CString pathToExcel) {
	for (size_t i = 0; i < path.size(); i++)
		if (pathToExcel.Compare(path[i]) == 0)
			return i;

	return -1;
}

// Дружественная функция для запуска определенной операции
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
		AfxMessageBox(_T("Неопознанная команда!"), MB_ICONWARNING);
}

// Проверка потока на доступность
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

// Закрытие потока
void CPIVWorker::closeThread(HANDLE& h) {
	if (h != 0) {
		CloseHandle(h);
		h = NULL;
	}
	else
		AfxMessageBox(_T("Не удалось закрыть поток! (Возможно он даже не открывался)"));
}