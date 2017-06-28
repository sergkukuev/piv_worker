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
CPIVWorker::~CPIVWorker() { CloseExcel(); }

// Установка статуса записи номера подкардра
void CPIVWorker::setStatusNumPK(const bool& status) { bNumPK = status; }

#pragma region READ

// Получение путей для чтения
void CPIVWorker::ReadExcel(const vector <CString>& pathToExcel) {
	buffer = pathToExcel;
	StartRead();
}

// Получение пути для чтения
void CPIVWorker::ReadExcel(const CString& pathToExcel) {
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
void CPIVWorker::Read() {
	try {
		CReaderExcel reader;	// класс чтения книг
		for (size_t i = 0; i < buffer.size(); i++)
			if (!findBook(buffer[i]))
				books.push_back(reader.getBook(buffer[i]));
	
		AfxMessageBox(_T("Чтение завершено успешно!"), MB_ICONINFORMATION);
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

// Начало проверки всех книг
void CPIVWorker::TestExcel() {
	if (getStatusThread(primary)) {
		hCmd = command.test;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(_T("Поток занят! Подождите окончания процесса!"));
}

/*// Проверка одной книги
void CPIVWorker::TestExcel(const CString& pathToExcel) {
	if (getStatusThread(primary)) {
		buffer.push_back(pathToExcel);
		hCmd = command.test;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(_T("Поток занят! Подождите окончания процесса!"));
}

// Проверка диапазона книг
void CPIVWorker::TestExcel(const vector <CString>& pathToExcel) {
	if (getStatusThread(primary)) {
		buffer = pathToExcel;
		hCmd = command.test;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(_T("Поток занят! Подождите окончания процесса!"));
}*/

// Проверка всех открытых книг
void CPIVWorker::Test() {
	try {
		for (size_t i = 0; i < books.size(); i++) {
			CTest tester;	// класс проверки книг
			Db = tester.Start(books);
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
void CPIVWorker::Report(const CString& path) {
	this->path = path;
	StartReport();
}

// Начало создания отчета (перегрузка)
void CPIVWorker::Report() { StartReport(); }

// Начало создания отчета об ошибках
void CPIVWorker::StartReport()
{
	if (path.IsEmpty())
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
	report.getReport(books, Db, path);
	pathFile.Format(_T("%s\\Отчет.html"), path);
	msg.Format(_T("Создание отчета завершено!\n\nРасположение: %s\nОткрыть для просмотра?"), pathFile);
	if (AfxMessageBox(msg, MB_YESNO | MB_ICONQUESTION) == IDYES)
		ShellExecute(0, L"open", pathFile, NULL, NULL, SW_NORMAL);	
}

#pragma endregion

#pragma region CLOSE

// Закрытие книг ПИВ
void CPIVWorker::CloseExcel() {
	books.clear();
	Db.clear();
	buffer.clear();
	buffer.shrink_to_fit();
}

// Закрытие книги ПИВ
void CPIVWorker::CloseExcel(const CString& pathToExcel) {
	buffer.push_back(pathToExcel);
	StartClose();
}

// Закрытие книги ПИВ
void CPIVWorker::CloseExcel(const vector <CString>& pathToExcel) {
	buffer = pathToExcel;
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

// Создание txt файлов
void CPIVWorker::CreateTxt(CString path) {
	this->path = path;
	StartTxt();
}

// Создание txt файлов (перегрузка)
void CPIVWorker::CreateTxt() { StartTxt(); }

// // Начало создания txt файлов
void CPIVWorker::StartTxt() {
	if (path.IsEmpty())
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
	report.getTxt(books,path, bNumPK);
	msg.Format(_T("Создание txt файлов завершено!\n\nРасположение: %s\nОткрыть для просмотра?"), path);
	if (AfxMessageBox(msg, MB_YESNO | MB_ICONQUESTION) == IDYES);
		ShellExecute(0, L"open", path, NULL, NULL, SW_NORMAL);
}

#pragma endregion

// Поиск индекса прочитанной книги в базе книг
bool CPIVWorker::findBook(const CString& pathToExcel) {
	bool result = false;
	for (list<bookData>::iterator it = books.begin(); it != books.end(); it++)
		if (nameFromPath(pathToExcel).Compare(it->name) == 0)
			result = true;

	return result;
}

// Получение имени файла
CString CPIVWorker::nameFromPath(const CString& path) {
	int startPos = path.ReverseFind(_T('\\')) + 1;
	return path.Mid(startPos, path.GetLength());
}

// Дружественная функция для запуска определенной операции
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
		AfxMessageBox(_T("Неопознанная команда!"), MB_ICONWARNING);
}

// Проверка потока на доступность
bool CPIVWorker::getStatusThread(const HANDLE& h) {
	DWORD ty;
	bool result = true;
	GetExitCodeThread(h, &ty);
	(ty != STILL_ACTIVE) ? result = true : result = false;
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