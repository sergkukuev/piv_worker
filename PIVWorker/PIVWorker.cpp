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

struct MyData 
{
	CPIV* object;
};

MyData mD;

DWORD WINAPI PrimaryThread(LPVOID lpParam) 
{
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
CPIV::~CPIV() 
{
	Close();
	CloseProject();
}

// Установить путь для хранения артефактов
void CPIV::setPathToSave(const CString& pathToReport) 
{ 
	SHFILEOPSTRUCT fos;
	ZeroMemory(&fos, sizeof(fos));

	// Установка функций для переноса папки артефактов
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

// Установить флаг bNumPK
void CPIV::setStatusNumPK(const bool& status) { bNumPK = status; }

#pragma region OPEN_PIV
// Открыть ПИВ и задать путь для артефактов (чтение, проверка, выдача артефактов)
void CPIV::Open(const vector<CString>& pathToExcel, const CString& pathToReport) 
{
	buffer = pathToExcel;
	path = pathToReport;
	StartOpen();
}

// Открыть ПИВ и использовать старый путь для артефактов
void CPIV::Open(const vector<CString>& pathToExcel)
{
	buffer = pathToExcel;
	StartOpen();
}

// Начало открытия ПИВ
void CPIV::StartOpen() 
{
	if (getStatusThread(primary))
	{
		CloseProject();
		hCmd = command.open;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		WriteError(THREAD_BUSY);
}

// Открытие ПИВ, пути которых лежат в буфере
void CPIV::OpenExcel() 
{
	try 
	{
		CReaderExcel reader; // Чтение
		for (size_t i = 0; i < buffer.size(); i++)
			project.books.push_back(reader.getBook(buffer[i]));

		CTest tester;	// Проверка
		project.db = tester.Start(project.books);

		// Генерация артефактов
		CReport report;
		CString tPath = path + L"\\Artefacts";
		CreateDirectory(tPath, NULL);
		report.getReport(project, tPath, true);	// true -  проект, false - остальные пив
		report.getTxt(project.books, path, bNumPK);
		closeThread(primary);
		
		WriteLog("Открытие проекта завершено");	// Логирование
		//AfxMessageBox(L"Открытие завершено!", MB_ICONINFORMATION);
	}
	catch (MyException& exc)
	{
		WriteError(exc.GetMsg());
	}
}
#pragma endregion

#pragma region ADD_PIV
// Добавить ПИВ
void CPIV::Add(const CString& pathToExcel) 
{
	buffer.push_back(pathToExcel);
	StartAdd();
}

// Добавить ПИВ (перегрузка)
void CPIV::Add(const vector<CString>& pathToExcel) 
{
	buffer = pathToExcel;
	StartAdd();
}

// Начало добавления ПИВ
void CPIV::StartAdd() 
{
	if (getStatusThread(primary))
	{
		hCmd = command.add;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		WriteError(THREAD_BUSY);
}

// Добавление ПИВ, пути которых лежат в буфере
void CPIV::AddExcel() 
{
	try 
	{
		CReport report;
		for (size_t i = 0; i < buffer.size(); i++) 
		{
			if (IsContain(project, buffer[i]))
				continue;

			// Добавление ПИВ и ошибок
			bool contain = IsContain(other, buffer[i]);
			CReaderExcel reader;
			bookData book = reader.getBook(buffer[i]);
			contain ? Refresh(other, book) : other.books.push_back(book);
			
			CTest tester;
			bookData& pBook = getBook(other, buffer[i]);
			errorSet error = tester.Start(pBook);
			contain ? Refresh(other, error) : other.db.push_back(error);

			report.getTxt(book, path, bNumPK);
		}
		CString tPath = path + L"\\Artefacts";
		CreateDirectory(tPath, NULL);
		report.getReport(other, tPath, false);	// true -  проект, false - остальные пив
		closeThread(primary);
		
		WriteLog("Добавление ПИВ завершено");	// Логирование
		//AfxMessageBox(L"Добавление завершено!", MB_ICONINFORMATION);
	}
	catch (MyException& exc) 
	{
		WriteError(exc.GetMsg());
	}
}
#pragma endregion

#pragma region REFRESH_PIV
// Обновить ПИВ
void CPIV::Refresh(const CString& pathToExcel) 
{
	buffer.push_back(pathToExcel);
	StartRefresh();
}

// Обновить несколько ПИВ
void CPIV::Refresh(const vector<CString>& pathToExcel)
{
	buffer = pathToExcel;
	StartRefresh();
}

// Начало обновления ПИВ
void CPIV::StartRefresh() 
{
	if (getStatusThread(primary)) 
	{
		hCmd = command.refresh;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		WriteError(THREAD_BUSY);
}

// Обновление ПИВ
void CPIV::RefreshExcel() 
{
	try 
	{
		CReport report;
		bool flagProj = false, flagOther = false;
		for (size_t i = 0; i < buffer.size(); i++)
		{
			// Добавление ПИВ и ошибок
			CReaderExcel reader;
			bookData book = reader.getBook(buffer[i]);

			CTest tester;
			bool contain = IsContain(project, buffer[i]);
			if (IsContain(project, buffer[i])) 
			{
				flagProj = true;
				Refresh(project, book);
				bookData& pBook = getBook(project, buffer[i]);
				errorSet error = tester.Start(pBook);
				Refresh(project, error);
			}
			else if (IsContain(other, buffer[i])) 
			{
				flagOther = true;
				Refresh(other, book);
				bookData& pBook = getBook(other, buffer[i]);
				errorSet error = tester.Start(pBook);
				Refresh(other, error);
			}
			else
				throw BookNotFound(nameFromPath(buffer[i]));
			report.getTxt(book, path, bNumPK);
		}
		CString tPath = path + L"\\Artefacts";
		if (flagProj)
			report.getReport(project, tPath, true);
			
		if (flagOther)
			report.getReport(other, tPath, false);
	
		closeThread(primary);
		
		WriteLog("Обновление ПИВ завершено");	// Логирование
		//AfxMessageBox(L"Обновление завершено!", MB_ICONINFORMATION);
	}
	catch (MyException& exc) 
	{
		WriteError(exc.GetMsg());
	}
}
#pragma endregion

#pragma region CLOSE_PIV
// Закрыть все
void CPIV::Close() 
{
	other.books.clear();
	other.db.clear();
	WriteLog("Закрытие ПИВ завершено");	// Логирование
}

// Закрытие ПИВ проекта
void CPIV::CloseProject() 
{
	project.books.clear();
	project.db.clear();
}

// Закрыть один ПИВ и его отчет в базе ошибок
void CPIV::Close(const CString& path)
{
	buffer.push_back(path);
	StartClose();
}

// Закрыть несколько ПИВ и их отчеты в базе ошибок
void CPIV::Close(const vector<CString>& path) 
{
	buffer = path;
	StartClose();
}

// Начало закрытия ПИВ
void CPIV::StartClose() 
{
	if (getStatusThread(primary)) 
	{
		hCmd = command.close;
		mD.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mD, 0, 0);
	}
	else
		WriteError(THREAD_BUSY);
}

// Закрытие ПИВ, пути которых лежат в буфере
void CPIV::CloseExcel() 
{
	for (size_t i = 0; i < buffer.size(); i++) 
	{
		for (list <errorSet>::iterator it = other.db.begin(); it != other.db.end();)
			it->book->name.Compare(nameFromPath(buffer[i])) == 0 ? other.db.erase(it++) : it++;
		for (list <bookData>::iterator it = other.books.begin(); it != other.books.end();)
			it->name.Compare(nameFromPath(buffer[i])) == 0 ? other.books.erase(it++) : it++;
	}
	closeThread(primary);
	WriteLog("Закрытие ПИВ завершено");	// Логирование
}
#pragma endregion

#pragma region subFunc
// Дружественная функция для запуска операции в потоке
void Thread(CPIV& piv) 
{
	if (piv.hCmd == piv.command.open)
		piv.OpenExcel();
	else if (piv.hCmd == piv.command.add)
		piv.AddExcel();
	else if (piv.hCmd == piv.command.refresh)
		piv.RefreshExcel();
	else if (piv.hCmd == piv.command.close)
		piv.CloseExcel();
	else
		piv.WriteError(L"Ошибка: Неопознанная команда!");
}

// Обновление ПИВ и ошибок
void CPIV::Refresh(pivData& data, const bookData& book, const errorSet& error) 
{
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (book.name.Compare(it->name) == 0)
			*it = book;

	for (list <errorSet>::iterator it = data.db.begin(); it != data.db.end(); it++)
		if (error.book->name.Compare(it->book->name) == 0)
			*it = error;
}

// Обновление ПИВ
void CPIV::Refresh(pivData& data, const bookData& book) 
{
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (book.name.Compare(it->name) == 0)
			*it = book;
}

// Обновление ПИВ
void CPIV::Refresh(pivData& data, const errorSet& error) 
{
	for (list <errorSet>::iterator it = data.db.begin(); it != data.db.end(); it++)
		if (error.book->name.Compare(it->book->name) == 0)
			*it = error;
}

// Получить ссылку на книгу
bookData& CPIV::getBook(pivData& data, const CString& path)
{
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (nameFromPath(path).Compare(it->name) == 0)
			return *it;
	return *data.books.end();
}

// Извлечение имени файла из его пути
CString CPIV::nameFromPath(const CString& path) 
{
	int startPos = path.ReverseFind(L'\\') + 1;
	return path.Mid(startPos, path.GetLength());
}

// Проверка наличия файла в памяти
bool CPIV::IsContain(pivData& data, const CString& path)
{
	bool result = false;
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		nameFromPath(path).Compare(it->name) == 0 ? result = true : result = result;
	
	return result;
}

// Проверка доступности потока
bool CPIV::getStatusThread(const HANDLE& h) 
{
	DWORD ty;
	bool result = true;
	GetExitCodeThread(h, &ty);
	ty != STILL_ACTIVE ? result = true : result = false;
	return result;
}

// Закрытие потока
void CPIV::closeThread(HANDLE& h)
{
	buffer.clear();
	buffer.shrink_to_fit();
	if (h != 0) {
		CloseHandle(h);
		h = NULL;
	}
	else
		WriteError(L"Ошибка: Не удалось закрыть поток!");
}

// Запись в именованный канал
void CPIV::WriteLog(char szBuf[256]) 
{
	HANDLE hLogPipe = CreateFile(pipeName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	DWORD  NumBytesToWrite;	
	WriteFile(hLogPipe, szBuf, strlen(szBuf), &NumBytesToWrite, NULL);
	CloseHandle(hLogPipe);
}

// Запись ошибки в файл
void CPIV::WriteError(const CString& msg) 
{
	SYSTEMTIME st;
	CString str;
	GetLocalTime(&st);
	str.Format(L"%02d:%02d:%02d %02d/%02d/%d:\t%s\n", st.wHour, st.wMinute, st.wSecond, st.wDay, st.wMonth, st.wYear, msg);
	CString logPath;
	logPath.Format(L"%s\\Artefacts\\log.txt", path);
	ofstream log(logPath, ios::out | ios::app);
	log << CT2A(str);
	log.close();
	
	WriteLog("Ошибка операции!");
	AfxMessageBox(msg, MB_ICONERROR);
}
#pragma endregion