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

struct MyData
{
	CPIVWorker* object;
};

MyData mD;

// инициализация CPIVWorkerApp
int main()
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// инициализировать MFC, а также печать и сообщения об ошибках про сбое
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: измените код ошибки соответственно своим потребностям
			wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
			nRetCode = 1;
		}
		else
		{
			// TODO: Вставьте сюда код для приложения.
		}
	}
	else
	{
		// TODO: Измените код ошибки соответственно своим потребностям
		wprintf(L"Критическая ошибка: неудачное завершение GetModuleHandle\n");
		nRetCode = 1;
	}

	return nRetCode;
}

DWORD WINAPI ReaderThread(LPVOID lpParam)
{
	CoInitialize(NULL);
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	MyData* myD = static_cast<MyData*>(lpParam);

	ReadExcel(*(myD->object));

	CoUninitialize();

	return 0;
}

DWORD WINAPI TestThread(LPVOID lpParam)
{
	CoInitialize(NULL);
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	MyData* myD = static_cast<MyData*>(lpParam);

	Test(*(myD->object));

	CoUninitialize();

	return 0;
}

DWORD WINAPI ReportThread(LPVOID lpParam)
{
	CoInitialize(NULL);
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	MyData* myD = static_cast<MyData*>(lpParam);

	Report(*(myD->object));

	CoUninitialize();

	return 0;
}

// Конструктор
CPIVWorker::CPIVWorker()
{
}

// Деструктор
CPIVWorker::~CPIVWorker()
{
	path.clear();
	books.clear();
	errorDB.syntax.clear();
	errorDB.simantic.clear();
	errorDB.warning.clear();
	checkPath.clear();
}

// Получение путей для чтения
void CPIVWorker::ReadExcel(vector <CString> pathToExcel)
{
	checkPath = pathToExcel;
	StartRead();
}

// Получение пути для чтения
void CPIVWorker::ReadExcel(CString pathToExcel)
{
	checkPath.push_back(pathToExcel);
	StartRead();
}

// Начало чтения
void CPIVWorker::StartRead()
{
	if (getStatusThread(primary))
	{
		mD.object = this;
		primary = CreateThread(NULL, 0, ReaderThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(_T("Поток занят! Подождите окончания процесса!"));
}

// Чтение ПИВ
void CPIVWorker::ReadExcel()
{
	try
	{
		for (size_t i = 0; i < checkPath.size(); i++)
		{
			CReaderExcel reader;	// класс чтения книг

			if (findIndexBook(checkPath[i]) == -1)		// Если такой книги еще нет
			{
				books.push_back(reader.getBook(checkPath[i]));
				path.push_back(checkPath[i]);
			}
		}
		AfxMessageBox(_T("Чтение завершено успешно!"));
	}
	catch (MyException &exc)
	{
		AfxMessageBox(exc.GetMsg());
	}

	checkPath.clear();
	closeThread(primary);
}

// Начало тестирования
void CPIVWorker::Test()
{
	if (getStatusThread(primary))
	{
		mD.object = this;
		primary = CreateThread(NULL, 0, TestThread, &mD, 0, 0);
	}
	else
		AfxMessageBox(_T("Поток занят! Подождите окончания процесса!"));
}

// Проверка всех открытых книг
void CPIVWorker::TestAll()
{
	try
	{
		for (size_t i = 0; i < books.size(); i++)
		{
			CTest tester;	// класс проверки книг
			errorOneSet errBook;

			errBook = tester.Start(books[i]);

			int indx = findReportBook(errBook.syntax.name);	// поиск в базе отчетов данной книги

			if (indx != -1)	// Обновление отчета
			{
				errorDB.syntax[indx] = errBook.syntax;
				errorDB.simantic[indx] = errBook.simantic;
				errorDB.warning[indx] = errBook.warning;
			}
			else // Добавление нового отчета
			{
				errorDB.syntax.push_back(errBook.syntax);
				errorDB.simantic.push_back(errBook.simantic);
				errorDB.warning.push_back(errBook.warning);
			}
				
		}
		AfxMessageBox(_T("Проверка протоколов завершена успешно!"));
	}
	catch (MyException &exc)
	{
		AfxMessageBox(exc.GetMsg());
	}
	closeThread(primary);
}

// Создание отчета об ошибках
void CPIVWorker::Report(CString path)
{
	if (path.IsEmpty())
		AfxMessageBox(_T("Укажите путь сохранения отчета!"));
	else
	{
		pathRep = path;
		if (getStatusThread(primary))
		{
			mD.object = this;
			primary = CreateThread(NULL, 0, ReportThread, &mD, 0, 0);
		}
		else
			AfxMessageBox(_T("Поток занят! Подождите окончания процесса!"));
	}
}

// Создание отчета
void CPIVWorker::MakeReport()
{
	CReport report;

	report.setPath(pathRep);
	report.Generate(books, errorDB);
	AfxMessageBox(_T("Создание отчета завершено!"));
}

// Закрыть книжечку
void CPIVWorker::CloseExcel(CString pathToExcel)
{
	int iBook = findIndexBook(pathToExcel);
	
	if (iBook != -1)
	{
		int iReport = findReportBook(books[iBook].nameBook);

		if (iReport != -1)	// Чистим в базе ошибок
		{
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

// Закрыть книжечки
void CPIVWorker::CloseExcel(vector <CString> pathToExcel)
{
	for (size_t i = 0; i < pathToExcel.size(); i++)
	{
		int iBook = findIndexBook(pathToExcel[i]);

		if (iBook != -1)
		{
			int iReport = findReportBook(books[iBook].nameBook);

			if (iReport != -1)	// Чистим в базе ошибок
			{
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

// Поиск индекса прочитанной книги в базе ошибок
int CPIVWorker::findReportBook(CString name)
{
	for (size_t i = 0; i < errorDB.syntax.size(); i++)
		if (name.Compare(errorDB.syntax[i].name) == 0)
			return i;

	return -1;
}

// Поиск индекса прочитанной книги в базе книг
int CPIVWorker::findIndexBook(CString pathToExcel)
{
	for (size_t i = 0; i < path.size(); i++)
		if (pathToExcel.Compare(path[i]) == 0)
			return i;

	return -1;
}

// Дружественная функция для запуска чтения протоколов
void ReadExcel(CPIVWorker& piv)
{
	piv.ReadExcel();
}

// Дружественная функция для запуска анализа протоколов
void Test(CPIVWorker& piv)
{
	piv.TestAll();
}

// Дружественная функция для запуска создания отчета
void Report(CPIVWorker& piv)
{
	piv.MakeReport();
}

// Проверка потока на доступность
bool CPIVWorker::getStatusThread(HANDLE h)
{
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
void CPIVWorker::closeThread(HANDLE& h)
{
	if (h != 0)
	{
		CloseHandle(h);
		h = NULL;
	}
	else
		AfxMessageBox(_T("А он и не открывался, парам парарам!"));
}