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
	CPIVWorker* object;
};

// Единственный объект CPIVWorkerWorkerApp
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
CPIVWorker::CPIVWorker() {	}

// Деструктор
CPIVWorker::~CPIVWorker() 
{
	Close();
	CloseProject();
}

/*bool CPIVWorker::IsUpdate() { return logger.IsRead(); }

CString CPIVWorker::GetStatus() { return logger.GetStatus(); }

void CPIVWorker::WriteLog(const CString& msg) { logger.Write(msg); }

// Получение путей параметров
CString CPIVWorker::GetPath() { return settings.GetPath(); }

CString CPIVWorker::GetOtherPath()
{
	CString result;
	result.Format(L"%s%s", settings.GetPath(), settings.folders[folders::other]);
	return result;
}

CString CPIVWorker::GetProjectPath()
{
	CString result;
	result.Format(L"%s%s", settings.GetPath(), settings.folders[folders::project]);
	return result;
}
#pragma region SET_PARAMETERS
// Установка пути хранения артефактов
void CPIVWorker::SetPathToSave(const CString& pathToReport)
{
	settings.SetStgPath(pathToReport);
	logger.Write(L"Изменено расположение папки отчетов");
}

void CPIVWorker::SetSettings(const stgParams& parameters) 
{ 
	while (!GetStatusThread(primary))
		Sleep(100);
	settings.Save(parameters);
	logger.Write(L"Изменены настройки приложения");
}

stgParams CPIVWorker::GetSettings() { return settings.GetParameters(); }
#pragma endregion

#pragma region OPEN_PROJECT
// Открытие проекта (набора ПИВ) с установкой пути хранения артефактов
void CPIVWorker::Open(const vector<CString> pathToExcel, const CString pathToReport) 
{
	buffer = pathToExcel;
	settings.SetStgPath(pathToReport);
	StartOpen();
}

// использовать старый путь хранения
void CPIVWorker::Open(const vector<CString> pathToExcel)
{
	buffer = pathToExcel;
	StartOpen();
}

// Запуск потока для операции открытия проекта
void CPIVWorker::StartOpen() 
{
	if (GetStatusThread(primary))
	{
		CloseProject();	// Очистка старых данных проекта
		hCmd = open;
		mData.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mData, 0, 0);
	}
	else
		AfxMessageBox(L"Подождите окончания выполнения операции, а затем повторите попытку", MB_ICONINFORMATION);
}

void CPIVWorker::OpenExcel() 
{
	logger.Write(L"Идет открытие протоколов проекта...");
	CReader reader;
	bool bOper = true;
	for (size_t i = 0; i < buffer.size(); i++)
	{
		try
		{
			project.books.push_back(reader.GetBook(buffer[i]));
		}
		catch (MyException& exc)
		{
			bOper = false;
			logger >> exc.GetMsg();
		}
	}

	CTest tester;
	project.db = tester.Start(project.books);
	// Генерация артефактов
	CReport report;
	report.GetReport(project, true) ? bOper = bOper : bOper = false;		// true -  проект, false - отдельные протоколы
	report.GetTxt(project.books) ? bOper = bOper : bOper = false;
	CloseThread(primary);
	bOper ? logger.Write(L"Открытие проекта завершено успешно") : logger.Write(L"Открытие проекта завершено с ошибками");
}
#pragma endregion

#pragma region OPEN_PIV
// Открыть отдельные протоколы:
// один
void CPIVWorker::Add(const CString pathToExcel) 
{
	buffer.push_back(pathToExcel);
	StartAdd();
}

// несколько
void CPIVWorker::Add(const vector<CString> pathToExcel) 
{
	buffer = pathToExcel;
	StartAdd();
}

// Запуск потока для открытия отдельных протоколов
void CPIVWorker::StartAdd() 
{
	if (GetStatusThread(primary))
	{
		hCmd = add;
		mData.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mData, 0, 0);
	}
	else
		AfxMessageBox(L"Подождите окончания выполнения операции, а затем повторите попытку", MB_ICONINFORMATION);
}

void CPIVWorker::AddExcel() 
{
	bool bOper = true;
	CReport report;
	buffer.size() == 1 ? logger.Write(L"Идет добавление протокола...") : logger.Write(L"Идет добавление протоколов...");
	for (size_t i = 0; i < buffer.size(); i++) 
	{
		try
		{
			CReader reader;
			bool contain = IsContain(other, buffer[i]);
			bookData book = reader.GetBook(buffer[i]);
			contain ? Refresh(other, book) : other.books.push_back(book);
			CTest tester;
			list <bookData>::iterator pBook = GetBook(other, buffer[i]);
			if (pBook == other.books.end())	// Книга не найденa, хотя добавлена выше
				throw BookNotFound(NameFromPath(buffer[i]));
			errorData error = tester.Start(*pBook);
			contain ? Refresh(other, error) : other.db.push_back(error);
			report.GetTxt(*pBook) ? bOper = bOper : bOper = false;
		}
		catch (MyException& exc)
		{
			bOper = false;
			logger >> exc.GetMsg();
		}
	}
	report.GetReport(other, false) ? bOper = bOper : bOper = false;	// true -  проект, false - отдельные протоколы
	CloseThread(primary);
	bOper ? logger.Write(L"Добавление завершено успешно") : logger.Write(L"Добавление завершено с ошибками");
}
#pragma endregion

#pragma region REFRESH_PIV
// Обновление протоколов
void CPIVWorker::Refresh(const vector<CString> pathToExcel)
{
	buffer = pathToExcel;
	StartRefresh();
}

void CPIVWorker::Refresh(const CString pathToExcel) 
{
	buffer.push_back(pathToExcel);
	StartRefresh();
}

// Запуск операции обновления протоколов
void CPIVWorker::StartRefresh() 
{
	if (GetStatusThread(primary))
	{
		hCmd = refresh;
		mData.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mData, 0, 0);
	}
	else
		AfxMessageBox(L"Подождите окончания выполнения операции, а затем повторите попытку", MB_ICONINFORMATION);
}

void CPIVWorker::RefreshExcel() 
{
	bool bOper = true, bProj = true;
	CReport report;
	buffer.size() == 1 ? logger.Write(L"Идет обновление выбранного протокола...") : logger.Write(L"Идет обновление выбранных протоколов...");
	for (size_t i = 0; i < buffer.size(); i++)
	{
		try
		{
			CReader reader;
			bookData book = reader.GetBook(buffer[i]);
			list <bookData>::iterator pBook;
			CTest tester;
			if (IsContain(project, buffer[i]))
			{
				bProj = true;
				Refresh(project, book);
				pBook = GetBook(project, buffer[i]);
				errorData error = tester.Start(*pBook);
				Refresh(project, error);
			}
			else if (IsContain(other, buffer[i]))
			{
				bProj = false;
				Refresh(other, book);
				pBook = GetBook(other, buffer[i]);
				errorData error = tester.Start(*pBook);
				Refresh(other, error);
			}
			else
				throw BookNotFound(NameFromPath(buffer[i]));
			report.GetTxt(*pBook) ? bOper = bOper : bOper = false;
		}
		catch (MyException& exc)
		{
			bOper = false;
			logger >> exc.GetMsg();
		}
	}
	bProj ? (report.GetReport(project, true) ? bOper = bOper : bOper = false) : 
		(report.GetReport(other, false) ? bOper = bOper : bOper = false);	// true -  проект, false - отдельные протоколы
	CloseThread(primary);
	bOper ? logger.Write(L"Обновление завершено успешно") : logger.Write(L"Обновление завершено с ошибками");
}
#pragma endregion

#pragma region CLOSE_PIV
// Закрытие всех протоколов
void CPIVWorker::Close() 
{
	logger.Write(L"Идет закрытие всех протоколов...");
	if (!other.books.empty())
		other.books.clear();
	if (!other.db.empty())
		other.db.clear();
	logger.Write(L"Закрытие всех протоколов завершено");	// Логирование
}

// одного
void CPIVWorker::Close(const CString path)
{
	buffer.push_back(path);
	StartClose();
}

// нескольких
void CPIVWorker::Close(const vector<CString> path) 
{
	buffer = path;
	StartClose();
}

// Запуск операции закрытия
void CPIVWorker::StartClose() 
{
	if (GetStatusThread(primary))
	{
		hCmd = close;
		mData.object = this;
		primary = CreateThread(NULL, 0, PrimaryThread, &mData, 0, 0);
	}
	else
		AfxMessageBox(L"Подождите окончания выполнения операции, а затем повторите попытку", MB_ICONINFORMATION);
}

// Закрытие проекта
void CPIVWorker::CloseProject() 
{
	logger.Write(L"Идет закрытие проекта...");
	if (!project.books.empty())
		project.books.clear();
	if (!project.db.empty())
		project.db.clear();
	logger.Write(L"Закрытие проекта завершено");	// Логирование
}

void CPIVWorker::CloseExcel() 
{
	buffer.size() == 1 ? logger.Write(L"Идет закрытие выбранного протокола...") : logger.Write(L"Идет закрытие выбранных протоколов...");
	for (size_t i = 0; i < buffer.size(); i++) 
	{
		for (list <errorData>::iterator it = other.db.begin(); it != other.db.end();)
			it->book->name.Compare(NameFromPath(buffer[i])) == 0 ? other.db.erase(it++) : it++;
		for (list <bookData>::iterator it = other.books.begin(); it != other.books.end();)
			it->name.Compare(NameFromPath(buffer[i])) == 0 ? other.books.erase(it++) : it++;
	}
	CloseThread(primary);
	logger.Write(L"Закрытие завершено успешно");	// Логирование
}
#pragma endregion

#pragma region SUB_FUNCTION
// Обновление протоколов (данные + база ошибок)
void CPIVWorker::Refresh(pivData& data, const bookData& book, const errorData& error) 
{
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (book.name.Compare(it->name) == 0)
			*it = book;

	for (list <errorData>::iterator it = data.db.begin(); it != data.db.end(); it++)
		if (error.book->name.Compare(it->book->name) == 0)
			*it = error;
}

// Обновление (данные)
void CPIVWorker::Refresh(pivData& data, const bookData& book) 
{
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (book.name.Compare(it->name) == 0)
			*it = book;
}

// Обновление (база ошибок)
void CPIVWorker::Refresh(pivData& data, const errorData& error) 
{
	for (list <errorData>::iterator it = data.db.begin(); it != data.db.end(); it++)
		if (error.book->name.Compare(it->book->name) == 0)
			*it = error;
}

// Получение ссылки на данные требуемого протокола
list<bookData>::iterator CPIVWorker::GetBook(pivData& data, const CString& path)
{
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		if (NameFromPath(path).Compare(it->name) == 0)
			return it;
	return data.books.end();
}

// Выделение имени протокола из его пути
CString CPIVWorker::NameFromPath(const CString& path) 
{
	int startPos = path.ReverseFind(L'\\') + 1;
	CString result = path.Mid(startPos, path.GetLength() - startPos);
	startPos = result.ReverseFind(L'.');
	result.Delete(startPos, result.GetLength() - startPos);
	return result;
}

// Проверка наличия данных требуемого протокола
bool CPIVWorker::IsContain(pivData& data, const CString& path)
{
	bool result = false;
	for (list <bookData>::iterator it = data.books.begin(); it != data.books.end(); it++)
		NameFromPath(path).Compare(it->name) == 0 ? result = true : result = result;
	return result;
}
#pragma endregion

#pragma region THREAD_FUNC
// Запуск операций DLL в потоке
void Thread(CPIVWorker& piv)
{
	switch (piv.hCmd)
	{
	case CPIVWorker::open:
		piv.OpenExcel();
		break;
	case CPIVWorker::add:
		piv.AddExcel();
		break;
	case CPIVWorker::refresh:
		piv.RefreshExcel();
		break;
	case CPIVWorker::close:
		piv.CloseExcel();
		break;
	default:
		piv.logger.WriteError(L"Использована неопознанная команда для выполнения DLL операции в потоке");
		break;
	}
}

// Проверка доступности потока
bool CPIVWorker::GetStatusThread(const HANDLE& h)
{
	DWORD ty;
	bool result = true;
	GetExitCodeThread(h, &ty);
	ty != STILL_ACTIVE ? result = true : result = false;
	return result;
}

// Закрытие потока
void CPIVWorker::CloseThread(HANDLE& h)
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
		logger.WriteError(L"Произошла ошибка закрытия основного потока DLL");
}
#pragma endregion*/