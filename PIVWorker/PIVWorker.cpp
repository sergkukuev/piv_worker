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
	Close();
}

// Установка статуса записи номера подкардра
void CPIVWorker::setStatusNumPK(const bool& status) { bNumPK = status; }

// Установка путей для артефактов
void CPIVWorker::setPathToSave(const CString& path) { this->path = path; }

// Получения статуса потока
bool CPIVWorker::getStatusThread(CString& status) { 
	return getStatusThread(primary);
};

#pragma region LOAD

// Загрузка списка пив для проекта и их дальнейшая проверка, и выдача артефактов
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

// Загрузка протоколов, обработка и вывод отчетов
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
	else {
		AfxMessageBox(THREAD_BUSY);
	}
}

// Чтение ПИВ
void CPIVWorker::Read(const bool& hClose) {
	try {
		CReaderExcel reader;	// класс чтения книг
		for (size_t i = 0; i < buffer.size(); i++) {
			bookData book = reader.getBook(buffer[i]);
			findBook(buffer[i]) ? Refresh(book) : books.push_back(book);
		}
		AfxMessageBox(L"Чтение завершено успешно!", MB_ICONINFORMATION);
	}
	catch (MyException &exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
	if (hClose)
		closeThread(primary);
}

// Обновление протокола
void CPIVWorker::Refresh(const bookData& book) {
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++)
		if (it->name.CompareNoCase(book.name) == 0) 
			*it = book;
}

#pragma endregion

#pragma region TEST

// Проверка всех книг
void CPIVWorker::TestExcel() { StartTest(); }

// Проверка одной книги
void CPIVWorker::TestExcel(const CString& path) {
	buffer.push_back(path);
	StartTest();
}

// Проверка выбранных книг
void CPIVWorker::TestExcel(const vector<CString>& path) {
	buffer = path;
	StartTest();
}

// Начало проверки книг
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

// Проверка книг
void CPIVWorker::Test(const bool& hClose) {
	try {
		CTest tester;
		
		if (buffer.empty())
			// Проверка всех книг
			Db = tester.Start(books);
		else {
			// Проверка требуемых книг
			for (size_t i = 0; i < buffer.size(); i++)
				if (findBook(buffer[i]))
					Refresh(tester.Start(getBook(buffer[i])));
		}
		AfxMessageBox(L"Проверка протоколов завершена успешно!", MB_ICONINFORMATION);
	}
	catch (MyException &exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
	if (hClose)
		closeThread(primary);
}

// Обновление ошибок по книге
void CPIVWorker::Refresh(const errorSet& set) {
	for (list <errorSet>::iterator it = Db.begin(); it != Db.end(); it++)
		if (it->book->name.CompareNoCase(set.book->name) == 0)
			*it = set;
}

#pragma endregion

#pragma region REPORT

// Создание отчета
void CPIVWorker::Report() { StartReport(); }

// Начало создания отчета об ошибках
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

// Создание отчета
void CPIVWorker::MakeReport(const bool& hClose) {
	try {
		CReport report;
		report.getReport(books, Db, path);
		CString pathFile, msg;
		pathFile.Format(_T("%s\\Отчет.html"), path);
		msg.Format(_T("Создание отчета завершено!\n\nРасположение: %s\nОткрыть для просмотра?"), pathFile);
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

// Создание txt для выбранного протокола
void CPIVWorker::CreateTxt(const CString& path) {
	buffer.push_back(path);
	StartTxt();
}

// Создание для выбранных протоколов
void CPIVWorker::CreateTxt(const vector<CString>& path) {
	buffer = path;
	StartTxt();
}

// Создание для всех протоколов
void CPIVWorker::CreateTxt() { StartTxt(); }

// // Начало создания txt файлов
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

// Начало генерации txt файлов 
void CPIVWorker::GenerateTxt(const bool& hClose) {
	try {
		CReport report;
		if (buffer.empty())
			report.getTxt(books, path, bNumPK);
		else
			for (size_t i = 0; i < buffer.size(); i++)
				report.getTxt(getBook(buffer[i]), path, bNumPK);
		
		AfxMessageBox(L"Создание txt файлов завершено!", MB_ICONINFORMATION);
	}
	catch (MyException &exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
	if (hClose)
		closeThread(primary);
}

#pragma endregion

#pragma region CLOSE

// Закрытие книг ПИВ
void CPIVWorker::CloseExcel() { StartClose(); }

// Закрытие книги ПИВ
void CPIVWorker::CloseExcel(const CString& pathToExcel) {
	buffer.push_back(pathToExcel);
	StartClose();
}

// Закрытие книг ПИВ
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
	else {
		AfxMessageBox(THREAD_BUSY);
	}
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
				it->book->name.CompareNoCase(nameFromPath(buffer[i])) == 0 ? Db.erase(it++) : it++;
			for (list <bookData>::iterator it = books.begin(); it != books.end();)
				it->name.CompareNoCase(nameFromPath(buffer[i])) == 0 ? books.erase(it++) : it++;
		}
	}
	closeThread(primary);
}

#pragma endregion

// Получение книги из списка открытых книг
bookData CPIVWorker::getBook(const CString& path) {
	for (list<bookData>::iterator it = books.begin(); it != books.end(); it++)
		if (nameFromPath(path).CompareNoCase(it->name) == 0)
			return *it;
	
	UndefinedBook exc;
	exc.SetName(nameFromPath(path));
	throw exc;
}

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
	else if (piv.hCmd == piv.command.load)
		piv.Load();
	else {
		AfxMessageBox(L"Неопознанная команда!", MB_ICONWARNING);
	}
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
	buffer.clear();			// Очистка буфера
	buffer.shrink_to_fit();
	if (h != 0) {
		CloseHandle(h);
		h = NULL;
	}
	else {
		AfxMessageBox(L"Не удалось закрыть поток. Он и не открывался.");
	}
}