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

// Основной поток
DWORD WINAPI PrimaryThread(LPVOID lpParam) 
{
	CoInitialize(NULL);
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	CPIV** object = static_cast<CPIV**>(lpParam);
	Thread(**object);
	CoUninitialize();
	return 0;
}

// Конструктор
CPIV::CPIV() 
{
	SendLog(L"Запуск DLL библиотеки");
}

// Деструктор
CPIV::~CPIV() 
{
	Close();
	CloseProject();
	SendLog(LOG_SLASH);
}

#pragma region SET_PARAMETERS
// Установка пути хранения артефактов
void CPIV::SetPathToSave(const CString& pathToReport)
{
	SHFILEOPSTRUCT fos;
	ZeroMemory(&fos, sizeof(fos));

	// Установка функций для переноса папки артефактов
	fos.wFunc = FO_MOVE;
	path.Format(L"%s\\%s", path, BASE_FOLDER);	// В одном CString содержится набор путей для переноса, поэтому должно быть два закрывающих символа
	path.AppendChar(0);		// '\0' чтобы закрыть строку
	path.AppendChar(0);		// '\0' чтобы закрыть набор
	
	CString to = pathToReport;	// Формируется аналогично path
	to.AppendChar(0);	// для строки
	to.AppendChar(0);	// для всего набора

	fos.pFrom = path;
	fos.pTo = to;
	fos.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;

	SHFileOperation(&fos);
	path = pathToReport;	// Установка пути хранения артефактов
}

// Установка флага bNumPK (значение подкадра)
void CPIV::SetStatusNumPK(const bool& status) { bNumPK = status; }
#pragma endregion

#pragma region OPEN_PROJECT
// Открытие проекта (набора ПИВ) с установкой пути хранения артефактов
void CPIV::Open(const vector<CString>& pathToExcel, const CString& pathToReport) 
{
	buffer = pathToExcel;
	path = pathToReport;
	StartOpen();
}

// использовать старый путь хранения
void CPIV::Open(const vector<CString>& pathToExcel)
{
	buffer = pathToExcel;
	StartOpen();
}

// Запуск потока для операции открытия проекта
void CPIV::StartOpen() 
{
	if (GetStatusThread(primary))
	{
		CloseProject();	// Очистка старых данных проекта
		hCmd = open;
		primary = CreateThread(NULL, 0, PrimaryThread, this, 0, 0);
	}
	else
		SendLog(THREAD_BUSY, true);
}

void CPIV::OpenExcel() 
{
	try 
	{
		CReaderExcel reader; 
		for (size_t i = 0; i < buffer.size(); i++)
			project.books.push_back(reader.getBook(buffer[i]));

		CTest tester;
		project.db = tester.Start(project.books);

		// Генерация артефактов
		CReport report;
		CString tPath = path + L"\\" + BASE_FOLDER;
		CreateDirectory(tPath, NULL);
		report.getReport(project, tPath, true);		// true -  проект, false - отдельные протоколы
		report.getTxt(project.books, path, bNumPK);
		CloseThread(primary);
		SendLog(L"Открытие проекта завершено");	// Логирование
	}
	catch (MyException& exc)
	{
		SendLog(exc.GetMsg(), true);
	}
}
#pragma endregion

#pragma region OPEN_PIV
// Открыть отдельные протоколы:
// один
void CPIV::Add(const CString& pathToExcel) 
{
	buffer.push_back(pathToExcel);
	StartAdd();
}

// несколько
void CPIV::Add(const vector<CString>& pathToExcel) 
{
	buffer = pathToExcel;
	StartAdd();
}

// Запуск потока для открытия отдельных протоколов
void CPIV::StartAdd() 
{
	if (GetStatusThread(primary))
	{
		hCmd = add;
		primary = CreateThread(NULL, 0, PrimaryThread, this, 0, 0);
	}
	else
		SendLog(THREAD_BUSY, true);
}

void CPIV::AddExcel() 
{
	try 
	{
		CReport report;
		for (size_t i = 0; i < buffer.size(); i++) 
		{
			bool contain = IsContain(other, buffer[i]);
			CReaderExcel reader;
			bookData book = reader.getBook(buffer[i]);
			contain ? Refresh(other, book) : other.books.push_back(book);
			
			CTest tester;
			bookData& pBook = GetBook(other, buffer[i]);
			errorSet error = tester.Start(pBook);
			contain ? Refresh(other, error) : other.db.push_back(error);

			report.getTxt(book, path, bNumPK);
		}
		CString tPath = path + L"\\" + BASE_FOLDER;
		CreateDirectory(tPath, NULL);
		report.getReport(other, tPath, false);	// true -  проект, false - отдельные протоколы
		CloseThread(primary);
		
		SendLog(L"Добавление ПИВ завершено");	// Логирование
	}
	catch (MyException& exc) 
	{
		SendLog(exc.GetMsg(), true);
	}
}
#pragma endregion

#pragma region REFRESH_PIV
// Обновление протоколов
void CPIV::Refresh(const vector<CString>& pathToExcel)
{
	buffer = pathToExcel;
	StartRefresh();
}

void CPIV::Refresh(const CString& pathToExcel) 
{
	buffer.push_back(pathToExcel);
	StartRefresh();
}

// Запуск операции обновления протоколов
void CPIV::StartRefresh() 
{
	if (GetStatusThread(primary)) 
	{
		hCmd = refresh;
		primary = CreateThread(NULL, 0, PrimaryThread, this, 0, 0);
	}
	else
		SendLog(THREAD_BUSY, true);
}

void CPIV::RefreshExcel() 
{
	try 
	{
		CReport report;
		bool flagProj = false, flagOther = false;
		for (size_t i = 0; i < buffer.size(); i++)
		{
			CReaderExcel reader;
			bookData book = reader.getBook(buffer[i]);

			CTest tester;
			if (IsContain(project, buffer[i])) 
			{
				flagProj = true;
				Refresh(project, book);
				bookData& pBook = GetBook(project, buffer[i]);
				errorSet error = tester.Start(pBook);
				Refresh(project, error);
			}
			else if (IsContain(other, buffer[i])) 
			{
				flagOther = true;
				Refresh(other, book);
				bookData& pBook = GetBook(other, buffer[i]);
				errorSet error = tester.Start(pBook);
				Refresh(other, error);
			}
			else
				throw BookNotFound(NameFromPath(buffer[i]));

			report.getTxt(book, path, bNumPK);
		}
		CString tPath = path + L"\\" + BASE_FOLDER;
		if (flagProj)
			report.getReport(project, tPath, true);		// true - проект, false - отдельные протоколы
		else if (flagOther)
			report.getReport(other, tPath, false);
	
		CloseThread(primary);
		
		SendLog(L"Обновление ПИВ завершено");	// Логирование
	}
	catch (MyException& exc) 
	{
		SendLog(exc.GetMsg(), true);
	}
}
#pragma endregion

#pragma region CLOSE_PIV
// Закрытие всех протоколов
void CPIV::Close() 
{
	if (!other.books.empty())
		other.books.clear();
	if (!other.db.empty())
		other.db.clear();
	SendLog(L"Закрытие ПИВ завершено");	// Логирование
}

// одного
void CPIV::Close(const CString& path)
{
	buffer.push_back(path);
	StartClose();
}

// нескольких
void CPIV::Close(const vector<CString>& path) 
{
	buffer = path;
	StartClose();
}

// Запуск операции закрытия
void CPIV::StartClose() 
{
	if (GetStatusThread(primary)) 
	{
		hCmd = close;
		primary = CreateThread(NULL, 0, PrimaryThread, this, 0, 0);
	}
	else
		SendLog(THREAD_BUSY, true);
}

// Закрытие проекта
void CPIV::CloseProject() 
{
	if (!project.books.empty())
		project.books.clear();
	if (!project.db.empty())
		project.db.clear();
}

void CPIV::CloseExcel() 
{
	for (size_t i = 0; i < buffer.size(); i++) 
	{
		for (list <errorSet>::iterator it = other.db.begin(); it != other.db.end();)
			it->book->name.Compare(NameFromPath(buffer[i])) == 0 ? other.db.erase(it++) : it++;
		for (list <bookData>::iterator it = other.books.begin(); it != other.books.end();)
			it->name.Compare(NameFromPath(buffer[i])) == 0 ? other.books.erase(it++) : it++;
	}
	CloseThread(primary);
	SendLog(L"Закрытие ПИВ завершено");	// Логирование
}
#pragma endregion

#pragma region SUB_FUNCTION
// Обновление протоколов (данные + база ошибок)
void CPIV::Refresh(pivData& data, const bookData& book, const errorSet& error) 
{
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (book.name.Compare(it->name) == 0)
			*it = book;

	for (list <errorSet>::iterator it = data.db.begin(); it != data.db.end(); it++)
		if (error.book->name.Compare(it->book->name) == 0)
			*it = error;
}

// Обновление (данные)
void CPIV::Refresh(pivData& data, const bookData& book) 
{
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (book.name.Compare(it->name) == 0)
			*it = book;
}

// Обновление (база ошибок)
void CPIV::Refresh(pivData& data, const errorSet& error) 
{
	for (list <errorSet>::iterator it = data.db.begin(); it != data.db.end(); it++)
		if (error.book->name.Compare(it->book->name) == 0)
			*it = error;
}

// Получение ссылки на данные требуемого протокола
bookData& CPIV::GetBook(pivData& data, const CString& path)
{
	bookData& result = *data.books.end();
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (NameFromPath(path).Compare(it->name) == 0)
			result = *it;
	return result;
}

// Выделение имени протокола из его пути
CString CPIV::NameFromPath(const CString& path) 
{
	int startPos = path.ReverseFind(L'\\') + 1;
	return path.Mid(startPos, path.GetLength());
}

// Проверка наличия данных требуемого протокола
bool CPIV::IsContain(pivData& data, const CString& path)
{
	bool result = false;
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		NameFromPath(path).Compare(it->name) == 0 ? result = true : result = result;
	return result;
}

// Передача сообщения о завершении операции и его логирование
void CPIV::SendLog(const CString& msg, const bool& error) 
{
	while (!logs.st_mutex.try_lock() && logs.status)	// Ожидание разблокировки мьютекса и прочтения сообщения 
		Sleep(10);
	
	//logs.st_mutex.lock();
	error ? logs.msg = L"При операции произошла ошибка (подробное описание см. в логах)" : logs.msg = msg;
	
	// Логирование в файл
	SYSTEMTIME st;
	CString str;
	GetLocalTime(&st);
	msg.CompareNoCase(LOG_SLASH) != 0 ?
		str.Format(L"%02d:%02d:%02d %02d/%02d/%d:\t%s\n", st.wHour, st.wMinute, st.wSecond, st.wDay, st.wMonth, st.wYear, msg) :
		str = msg;
	CString logPath;
	logPath.Format(L"%s\\%s\\%s", path, BASE_FOLDER, LOG_FILE_NAME);
	ofstream logStream(logPath, ios::out | ios::app);
	logStream << CT2A(str);
	logStream.close();

	logs.st_mutex.unlock();

	if (error)
		AfxMessageBox(msg, MB_ICONERROR);
}
#pragma endregion

#pragma region THREAD_FUNC
// Запуск операций DLL в потоке
void Thread(CPIV& piv)
{
	switch (piv.hCmd)
	{
	case CPIV::open:
		piv.OpenExcel();
		break;
	case CPIV::add:
		piv.AddExcel();
		break;
	case CPIV::refresh:
		piv.RefreshExcel();
		break;
	case CPIV::close:
		piv.CloseExcel();
		break;
	default:
		piv.SendLog(L"Ошибка: неопознанная команда!", true);
		break;
	}
}

// Проверка доступности потока
bool CPIV::GetStatusThread(const HANDLE& h)
{
	DWORD ty;
	bool result = true;
	GetExitCodeThread(h, &ty);
	ty != STILL_ACTIVE ? result = true : result = false;
	return result;
}

// Закрытие потока
void CPIV::CloseThread(HANDLE& h)
{
	if (!buffer.empty())	// Очистка буфера
	{
		buffer.clear();
		buffer.shrink_to_fit();
	}

	if (h != 0)
	{
		CloseHandle(h);
		h = NULL;
	}
	else
		SendLog(L"Ошибка: Не удалось закрыть поток!", true);
}
#pragma endregion