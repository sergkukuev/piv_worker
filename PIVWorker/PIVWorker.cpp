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

// инициализация CPIVWorker
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

// Установка путей для артефактов
void CPIVWorker::setPathToSave(const CString& path) { this->path = path; }

// Получения статуса потока
bool CPIVWorker::getStatusThread(CString& status) { 
	status = hStatus;
	return getStatusThread(primary);
};

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
		hStatus = L"Выполняется чтение протоколов ИВ...";
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else {
		hStatus = THREAD_BUSY;
		AfxMessageBox(THREAD_BUSY);
	}
}

// Чтение ПИВ
void CPIVWorker::Read() {
	try {
		CReaderExcel reader;	// класс чтения книг
		for (size_t i = 0; i < buffer.size(); i++) {
			bookData book = reader.getBook(buffer[i]);
			findBook(buffer[i]) ? Refresh(book) : books.push_back(book);
		}
		AfxMessageBox(L"Чтение завершено успешно!", MB_ICONINFORMATION);
		hStatus = L"Чтение протоколов ИВ завершено...";
	}
	catch (MyException &exc) {
		hStatus = L"Ошибка чтения протоколов ИВ...";
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
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

// Првоерка выбранных книг
void CPIVWorker::TestExcel(const vector<CString>& path) {
	buffer = path;
	StartTest();
}

// Начало проверки всех книг
void CPIVWorker::StartTest() {
	if (getStatusThread(primary)) {
		hCmd = command.test;
		hStatus = L"Выполняется проверка протоколов ИВ...";
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else {
		hStatus = THREAD_BUSY;
		AfxMessageBox(THREAD_BUSY);
	}
}

// Проверка книг
void CPIVWorker::Test() {
	try {
		if (buffer.empty()) {
			CTest tester;	// проверка всех книг
			Db = tester.Start(books);
		}
		else {
			// проверка требуемых книг
			for (size_t i = 0; i < buffer.size(); i++) {
				bookData& pBook = getBook(buffer[i]);
				CTest tester;
				if (findBook(buffer[i]))
					Refresh(tester.Start(pBook));
			}
		}
		hStatus = L"Проверка протоколов ИВ завершена...";
		AfxMessageBox(L"Проверка протоколов завершена успешно!", MB_ICONINFORMATION);
	}
	catch (MyException &exc) {
		hStatus = L"При проверки протоколов ИВ возникла ошибка...";
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
	closeThread(primary);
}

// Обновление ошибок
void CPIVWorker::Refresh(const errorSet& set) {
	for (list <errorSet>::iterator it = Db.begin(); it != Db.end(); it++)
		if (it->book->name.CompareNoCase(set.book->name) == 0)
			*it = set;
}

#pragma endregion

#pragma region REPORT

// Начало создания отчета
void CPIVWorker::Report(const CString& pathToExcel) {
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
			hStatus = L"Подготовка отчета об ошибках...";
			mD.object = this;
			primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
		}
		else {
			hStatus = THREAD_BUSY;
			AfxMessageBox(THREAD_BUSY);
		}
	}
}

// Создание отчета
void CPIVWorker::MakeReport() {
	try {
		CReport report;
		CString pathFile, msg;
		
		report.getReport(books, Db, path);
		pathFile.Format(_T("%s\\Отчет.html"), path);
		msg.Format(_T("Создание отчета завершено!\n\nРасположение: %s\nОткрыть для просмотра?"), pathFile);
		if (AfxMessageBox(msg, MB_YESNO | MB_ICONQUESTION) == IDYES)
			ShellExecute(0, L"open", pathFile, NULL, NULL, SW_NORMAL);
		hStatus = L"Создание отчета об ошибках завершено...";
	}
	catch (MyException &exc) {
		hStatus = L"При создании отчета об ошибках возникла ошибка...";
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
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
		hStatus = L"Подготовка к закрытию протоколов ИВ...";
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else {
		hStatus = THREAD_BUSY;
		AfxMessageBox(THREAD_BUSY);
	}
}

// Закрытие книг(и) ПИВ
void CPIVWorker::Close() {
	/*if (buffer.empty()) {
		books.clear();
		Db.clear();
	}
	else {
		for (size_t i = 0; i < buffer.size(); i++) {
			for (list <errorSet>::iterator it = Db.begin(); it != Db.end();)
				findBook(buffer[i]) ? Db.erase(it) : it++;

			for (list <bookData>::iterator it = books.begin(); it != books.end();)
				findBook(buffer[i]) ? books.erase(it++) : it++;
		}
		buffer.clear();
		buffer.shrink_to_fit();
	}*/
	hStatus = L"Закрытие протоколов завершено...";
}

#pragma endregion

#pragma region TXT

// Создание для выбранных протоколов
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
	if (path.IsEmpty())
		AfxMessageBox(L"Папка для сохранения txt файлов не указана!");
	else {
		if (getStatusThread(primary)) {
			hCmd = command.txt;
			mD.object = this;
			hStatus = L"Идет преобразование протоколов ИВ в txt формат...";
			primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
		}
		else {
			hStatus = THREAD_BUSY;
			AfxMessageBox(THREAD_BUSY);
		}
	}
}

// Начало генерации txt файлов 
void CPIVWorker::GenerateTxt() {
	try {
		if (buffer.empty()) {
			CReport report;
			report.getTxt(books, path, bNumPK);
		}
		else {
			for (size_t i = 0; i < buffer.size(); i++) {
				CReport report;
				bookData& pBook = getBook(buffer[i]);
				report.getTxt(pBook, path, bNumPK);
			}
		}
		hStatus = L"Преобразование в txt завершено...";
		AfxMessageBox(L"Создание txt файлов завершено!", MB_ICONINFORMATION);
	}
	catch (MyException &exc) {
		hStatus = L"При преобразовании в txt возникла ошибка...";
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
	closeThread(primary);
}

#pragma endregion

// Получение ссылки на книге в листе
bookData& CPIVWorker::getBook(const CString& path) {
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
	else {
		AfxMessageBox(L"Неопознанная команда!", MB_ICONWARNING);
		piv.hStatus = L"Неопознанная команда выполнения...";
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
	buffer.clear();
	buffer.shrink_to_fit();
	if (h != 0) {
		CloseHandle(h);
		h = NULL;
	}
	else {
		hStatus = L"Неудачное закрытие потока, перезапустите приложение...";
		AfxMessageBox(L"Не удалось закрыть поток. Он и не открывался.");
	}
}