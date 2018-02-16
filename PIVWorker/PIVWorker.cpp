// PIVWorker.cpp: определяет процедуры инициализации для DLL.
//

#include "stdafx.h"
#include "PIVWorker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// TODO: Важно! Перейдите по ссылке, чтобы увидеть подробности.
//	 	 Если эта библиотека DLL динамически связана с библиотеками DLL MFC,
//		 все функции, экспортированные из данной DLL-библиотеки, которые выполняют вызовы к
//		 MFC, должны содержать макрос AFX_MANAGE_STATE в
//		 самое начало функции.
//
//		 Например:
//
//		 extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		 {
//			 AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			 // тело нормальной функции
//		 }
//
//		 Важно, чтобы данный макрос был представлен в каждой
//		 функции до вызова MFC.  Это означает, что
//		 он должен быть первым оператором 
//		 функции и предшествовать даже любым объявлениям переменных объекта,
//		 поскольку их конструкторы могут выполнять вызовы к MFC
//		 DLL.
//
//		 В Технических указаниях MFC 33 и 58 содержатся более
//		 подробные сведения.
//

using namespace stgdll;	// Пространство имен настроек
struct ThreadData
{
	CPIV* object;
};

// Единственный объект CPIVWorkerApp
CWinApp theApp;
ThreadData mData;

// Основной поток
DWORD WINAPI PrimaryThread(LPVOID lpParam) 
{
	CoInitialize(NULL);
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	ThreadData* data = static_cast<ThreadData*>(lpParam);
	Thread(*data->object);
	CoUninitialize();
	return 0;
}

// Конструктор
CPIV::CPIV() {	}

// Деструктор
CPIV::~CPIV() 
{
	Close();
	CloseProject();
	logger.Write(LOG_SLASH);
}

bool CPIV::IsUpdate() { return logger.IsRead(); }

CString CPIV::GetStatus() { return logger.GetStatus(); }

void CPIV::WriteLog(const CString& msg) { logger << msg; }

// Получение путей параметров
CString CPIV::GetPath() { return settings.GetPath(); }

CString CPIV::GetOtherPath()
{
	CString result;
	result.Format(L"%s%s", settings.GetPath(), settings.folders[folders::other]);
	return result;
}

CString CPIV::GetProjectPath()
{
	CString result;
	result.Format(L"%s%s", settings.GetPath(), settings.folders[folders::project]);
	return result;
}
#pragma region SET_PARAMETERS
// Установка пути хранения артефактов
void CPIV::SetPathToSave(const CString& pathToReport)
{
	settings.SetStgPath(pathToReport);
	logger << L"Изменено расположение папки отчетов";
}

void CPIV::SetSettings(const stgParams& parameters) 
{ 
	while (!GetStatusThread(primary))
		Sleep(100);
	settings.parameters = parameters;
	logger << L"Изменены настройки приложения";
}

stgParams CPIV::GetSettings() { return settings.GetParameters(); }
#pragma endregion

#pragma region OPEN_PROJECT
// Открытие проекта (набора ПИВ) с установкой пути хранения артефактов
void CPIV::Open(const vector<CString> pathToExcel, const CString pathToReport) 
{
	buffer = pathToExcel;
	settings.SetStgPath(pathToReport);
	StartOpen();
}

// использовать старый путь хранения
void CPIV::Open(const vector<CString> pathToExcel)
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
		mData.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mData, 0, 0);
	}
	else
		AfxMessageBox(LOG_THREAD_BUSY, MB_ICONINFORMATION);
}

void CPIV::OpenExcel() 
{
	try 
	{
		logger << L"Идет открытие протоколов...";
		CReaderExcel reader;
		for (size_t i = 0; i < buffer.size(); i++)
			project.books.push_back(reader.GetBook(buffer[i]));

		CTest tester;
		project.db = tester.Start(project.books);

		// Генерация артефактов
		CReport report;
		report.GetReport(project, true);		// true -  проект, false - отдельные протоколы
		report.GetTxt(project.books);
		CloseThread(primary);
		logger << L"Открытие протоколов завершено";	// Логирование
	}
	catch (MyException& exc)
	{
		logger.WriteError(exc.GetMsg());
	}
}
#pragma endregion

#pragma region OPEN_PIV
// Открыть отдельные протоколы:
// один
void CPIV::Add(const CString pathToExcel) 
{
	buffer.push_back(pathToExcel);
	StartAdd();
}

// несколько
void CPIV::Add(const vector<CString> pathToExcel) 
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
		mData.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mData, 0, 0);
	}
	else
		AfxMessageBox(LOG_THREAD_BUSY, MB_ICONINFORMATION);
}

void CPIV::AddExcel() 
{
	try 
	{
		logger >> L"Идет добавление протоколов...";
		CReport report;
		for (size_t i = 0; i < buffer.size(); i++) 
		{
			bool contain = IsContain(other, buffer[i]);
			CReaderExcel reader;
			bookData book = reader.GetBook(buffer[i]);
			contain ? Refresh(other, book) : other.books.push_back(book);
			
			CTest tester;
			list <bookData>::iterator pBook = GetBook(other, buffer[i]);
			errorData error = tester.Start(*pBook);
			contain ? Refresh(other, error) : other.db.push_back(error);

			report.GetTxt(*pBook);
		}
		
		report.GetReport(other, false);	// true -  проект, false - отдельные протоколы
		CloseThread(primary);
		
		logger << L"Добавление протоколов завершено";	// Логирование
	}
	catch (MyException& exc) 
	{
		logger.WriteError(exc.GetMsg());
	}
}
#pragma endregion

#pragma region REFRESH_PIV
// Обновление протоколов
void CPIV::Refresh(const vector<CString> pathToExcel)
{
	buffer = pathToExcel;
	StartRefresh();
}

void CPIV::Refresh(const CString pathToExcel) 
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
		mData.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mData, 0, 0);
	}
	else
		AfxMessageBox(LOG_THREAD_BUSY, MB_ICONINFORMATION);
}

void CPIV::RefreshExcel() 
{
	try 
	{
		logger >> L"Идет обновление выбранных протоколов...";
		CReport report;
		bool flag = true;
		for (size_t i = 0; i < buffer.size(); i++)
		{
			CReaderExcel reader;
			bookData book = reader.GetBook(buffer[i]);
			list <bookData>::iterator pBook;

			CTest tester;
			if (IsContain(project, buffer[i])) 
			{
				flag = true;
				Refresh(project, book);
				pBook = GetBook(project, buffer[i]);
				errorData error = tester.Start(*pBook);
				Refresh(project, error);
			}
			else if (IsContain(other, buffer[i])) 
			{
				flag = false;
				Refresh(other, book);
				pBook = GetBook(other, buffer[i]);
				errorData error = tester.Start(*pBook);
				Refresh(other, error);
			}
			else
				throw BookNotFound(NameFromPath(buffer[i]));

			report.GetTxt(*pBook);
		}
		
		flag ?	report.GetReport(project, true) : report.GetReport(other, false);
	
		CloseThread(primary);
		
		logger << L"Обновление протоколов завершено";	// Логирование
	}
	catch (MyException& exc) 
	{
		logger.WriteError(exc.GetMsg());
	}
}
#pragma endregion

#pragma region CLOSE_PIV
// Закрытие всех протоколов
void CPIV::Close() 
{
	if (!other.books.empty() || !other.db.empty())
	{
		logger >> L"Идет закрытие всех протоколов...";
		other.books.clear();
		other.db.clear();
		logger << L"Закрытие протоколов завершено";	// Логирование
	}
}

// одного
void CPIV::Close(const CString path)
{
	buffer.push_back(path);
	StartClose();
}

// нескольких
void CPIV::Close(const vector<CString> path) 
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
		mData.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mData, 0, 0);
	}
	else
		AfxMessageBox(LOG_THREAD_BUSY, MB_ICONINFORMATION);
}

// Закрытие проекта
void CPIV::CloseProject() 
{
	if (!project.books.empty() || !project.db.empty())
	{
		logger >> L"Идет закрытие проекта...";
		project.books.clear();
		project.db.clear();
		logger << L"Закрытие проекта завершено";	// Логирование
	}
}

void CPIV::CloseExcel() 
{
	logger >> L"Идет закрытие выбранных протоколов...";
	for (size_t i = 0; i < buffer.size(); i++) 
	{
		for (list <errorData>::iterator it = other.db.begin(); it != other.db.end();)
			it->book->name.Compare(NameFromPath(buffer[i])) == 0 ? other.db.erase(it++) : it++;
		for (list <bookData>::iterator it = other.books.begin(); it != other.books.end();)
			it->name.Compare(NameFromPath(buffer[i])) == 0 ? other.books.erase(it++) : it++;
	}
	CloseThread(primary);
	logger << L"Закрытие протоколов завершено";	// Логирование
}
#pragma endregion

#pragma region SUB_FUNCTION
// Обновление протоколов (данные + база ошибок)
void CPIV::Refresh(pivData& data, const bookData& book, const errorData& error) 
{
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (book.name.Compare(it->name) == 0)
			*it = book;

	for (list <errorData>::iterator it = data.db.begin(); it != data.db.end(); it++)
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
void CPIV::Refresh(pivData& data, const errorData& error) 
{
	for (list <errorData>::iterator it = data.db.begin(); it != data.db.end(); it++)
		if (error.book->name.Compare(it->book->name) == 0)
			*it = error;
}

// Получение ссылки на данные требуемого протокола
list<bookData>::iterator CPIV::GetBook(pivData& data, const CString& path)
{
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (NameFromPath(path).Compare(it->name) == 0)
			return it;
	return data.books.end();
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
		piv.logger.WriteError(L"Ошибка: неопознанная команда");
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
		logger.WriteError(L"Ошибка: Не удалось закрыть поток");
}
#pragma endregion