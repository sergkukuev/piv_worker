// PIVWorker.cpp: определяет процедуры инициализации для DLL.
//

#include "stdafx.h"
#include "PIVWorker.h"
#include "ReaderExcel.h"	// чтение протоколов
#include "Test.h"			// проверка на ошибки протоколов
#include "Report.h"			// создание отчетов об ошибках

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

// Конструктор
CPIV::CPIV() { }

// Деструктор
CPIV::~CPIV() {
	Close();
	CloseProject();
}

// Установить путь для хранения артефактов
void CPIV::setPathToSave(const CString& pathToReport) { path = pathToReport; }

// Установить флаг bNumPK
void CPIV::setStatusNumPK(const bool& status) { bNumPK = status; }

#pragma region OPEN_PIV
// Открыть ПИВ и задать путь для артефактов (чтение, проверка, выдача артефактов)
void CPIV::Open(const vector<CString>& pathToExcel, const CString& pathToReport) {
	buffer = pathToExcel;
	path = pathToReport;
	StartOpen();
}

// Открыть ПИВ и использовать старый путь для артефактов
void CPIV::Open(const vector<CString>& pathToExcel) {
	buffer = pathToExcel;
	StartOpen();
}

// Начало открытия ПИВ
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

// Открытие ПИВ, пути которых лежат в буфере
void CPIV::OpenExcel() {
	try {
		CReaderExcel reader; // Чтение
		for (size_t i = 0; i < buffer.size(); i++)
			project.books.push_back(reader.getBook(buffer[i]));

		CTest tester;	// Проверка
		project.db = tester.Start(project.books);

		// Генерация артефактов
		CReport report;
		report.getReport(project, path);
		report.getTxt(project.books, path, bNumPK);
		AfxMessageBox(L"Открытие завершено!", MB_ICONINFORMATION);
		closeThread(primary);
	}
	catch (MyException& exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
}
#pragma endregion

#pragma region ADD_PIV
// Добавить ПИВ
void CPIV::Add(const CString& pathToExcel) {
	buffer.push_back(pathToExcel);
	StartAdd();
}

// Добавить ПИВ (перегрузка)
void CPIV::Add(const vector<CString>& pathToExcel) {
	buffer = pathToExcel;
	StartAdd();
}

// Начало добавления ПИВ
void CPIV::StartAdd() {
	if (getStatusThread(primary)) {
		hCmd = command.add;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(THREAD_BUSY);
}

// Добавление ПИВ, пути которых лежат в буфере
void CPIV::AddExcel() {
	try {
		CReport report;
		for (size_t i = 0; i < buffer.size(); i++) {
			if (IsContain(project, buffer[i]))
				continue;

			// Добавление ПИВ и ошибок
			bool contain = IsContain(other, buffer[i]);
			CReaderExcel reader;
			bookData book = reader.getBook(buffer[i]);
			CTest tester;
			errorSet error = tester.Start(book);

			if (!contain) {
				other.books.push_back(book);
				other.db.push_back(error);
			}
			else
				Refresh(other, book, error);
			report.getTxt(book, path, bNumPK);
		}
		report.getReport(other, path);	// Обновление отчета
		AfxMessageBox(L"Добавление завершено!", MB_ICONINFORMATION);
		closeThread(primary);
	}
	catch (MyException& exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
}
#pragma endregion

#pragma region REFRESH_PIV
// Обновить ПИВ
void CPIV::Refresh(const CString& pathToExcel) {
	buffer.push_back(pathToExcel);
	StartRefresh();
}

// Обновить несколько ПИВ
void CPIV::Refresh(const vector<CString>& pathToExcel) {
	buffer = pathToExcel;
	StartRefresh();
}

// Начало обновления ПИВ
void CPIV::StartRefresh() {
	if (getStatusThread(primary)) {
		hCmd = command.refresh;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(THREAD_BUSY);
}

// Обновление ПИВ
void CPIV::RefreshExcel() {
	try {
		CReport report;
		for (size_t i = 0; i < buffer.size(); i++) {
			// Добавление ПИВ и ошибок
			bool contain = IsContain(project, buffer[i]);
			CReaderExcel reader;
			bookData book = reader.getBook(buffer[i]);
			CTest tester;
			errorSet error = tester.Start(book);

			contain ? Refresh(project, book, error) : contain = IsContain(other, buffer[i]);

			if (!contain) {
				BookNotFound exc;
				exc.setName(nameFromPath(buffer[i]));
				throw exc;
			}
			else
				Refresh(other, book, error);
			report.getTxt(book, path, bNumPK);
		}
		report.getReport(project, path);
		report.getReport(other, path);
		AfxMessageBox(L"Обновление завершено!", MB_ICONINFORMATION);
		closeThread(primary);
	}
	catch (MyException& exc) {
		AfxMessageBox(exc.GetMsg(), MB_ICONERROR);
	}
}
#pragma endregion

#pragma region CLOSE_PIV
// Закрыть все
void CPIV::Close() {
	other.books.clear();
	other.db.clear();
}

// Закрытие ПИВ проекта
void CPIV::CloseProject() {
	project.books.clear();
	project.db.clear();
}

// Закрыть один ПИВ и его отчет в базе ошибок
void CPIV::Close(const CString& path) {
	buffer.push_back(path);
	StartClose();
}

// Закрыть несколько ПИВ и их отчеты в базе ошибок
void CPIV::Close(const vector<CString>& path) {
	buffer = path;
	StartClose();
}

// Начало закрытия ПИВ
void CPIV::StartClose() {
	if (getStatusThread(primary)) {
		hCmd = command.close;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(THREAD_BUSY);
}

// Закрытие ПИВ, пути которых лежат в буфере
void CPIV::CloseExcel() {
	for (size_t i = 0; i < buffer.size(); i++) {
		for (list <bookData>::iterator it = other.books.begin(); it != other.books.end();)
			it->name.CompareNoCase(nameFromPath(buffer[i])) == 0 ? other.books.erase(it++) : it++;
		for (list <errorSet>::iterator it = other.db.begin(); it != other.db.end();)
			it->book->name.CompareNoCase(nameFromPath(buffer[i])) == 0 ? other.db.erase(it++) : it++;
	}
	closeThread(primary);
}
#pragma endregion

#pragma region subFunc
// Дружественная функция для запуска операции в потоке
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
		AfxMessageBox(L"Неопознанная команда!", MB_ICONWARNING);
}

// Обновление ПИВ и ошибок
void CPIV::Refresh(pivData& data, const bookData& book, const errorSet& error) {
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (book.name.Compare(it->name) == 0)
			*it = book;

	for (list <errorSet>::iterator it = data.db.begin(); it != data.db.end(); it++)
		if (error.book->name.Compare(it->book->name) == 0)
			*it = error;
}

// Извлечение имени файла из его пути
CString CPIV::nameFromPath(const CString& path) {
	int startPos = path.ReverseFind(L'\\') + 1;
	return path.Mid(startPos, path.GetLength());
}

// Проверка наличия файла в памяти
bool CPIV::IsContain(pivData& data, const CString& path) {
	bool result = false;
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		nameFromPath(path).Compare(it->name) == 0 ? result = true : result = result;
	
	return result;
}

// Проверка доступности потока
bool CPIV::getStatusThread(const HANDLE& h) {
	DWORD ty;
	bool result = true;
	GetExitCodeThread(h, &ty);
	ty != STILL_ACTIVE ? result = true : result = false;
	return result;
}

// Закрытие потока
void CPIV::closeThread(HANDLE& h) {
	buffer.clear();
	buffer.shrink_to_fit();
	if (h != 0) {
		CloseHandle(h);
		h = NULL;
	}
	else
		AfxMessageBox(L"Не удалось закрыть поток. Он и не открывался.");
}
#pragma endregion