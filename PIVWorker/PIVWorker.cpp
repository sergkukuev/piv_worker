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

// Конструктор
CPIVWorker::CPIVWorker()
{

}

// Деструктор
CPIVWorker::~CPIVWorker()
{
	path.clear();
	books.clear();
	errorDB.clear();
}

// Чтение ПИВ
void CPIVWorker::ReadExcel(CString pathToExcel)
{
	try
	{
		CReaderExcel reader;

		if (!isExist(pathToExcel))
		{
			books.push_back(reader.getBook(pathToExcel));
			path.push_back(pathToExcel);
		}
	}
	catch (MyException& exc)
	{
		AfxMessageBox(exc.GetMsg());
	}
}

// Чтение ПИВ
void CPIVWorker::ReadExcel(vector<CString> pathToExcel)
{
	try
	{
		for (size_t i = 0; i < pathToExcel.size(); i++)
		{
			CReaderExcel reader;	// класс чтения книг

			if (!isExist(pathToExcel[i]))
			{
				books.push_back(reader.getBook(pathToExcel[i]));
				path.push_back(pathToExcel[i]);
			}
		}
	}
	catch (MyException &exc)
	{
		AfxMessageBox(exc.GetMsg());
	}
}

// Проверка всех открытых книг
void CPIVWorker::Test()
{
	try
	{
		for (size_t i = 0; i < books.size(); i++)
		{
			CTest tester;	// класс проверки книг
			errorBookData errBook;

			errBook = tester.Start(books[i]);

			int indx = findReportBook(errBook.name);	// поиск в базе отчетов данной книги

			if (indx != -1)
				errorDB[indx] = errBook;	// Обновление отчета
			else
				errorDB.push_back(errBook); // Добавление нового отчета
		}
	}
	catch (MyException &exc)
	{
		AfxMessageBox(exc.GetMsg());
	}
}

// Имеется ли уже такая книга
bool CPIVWorker::isExist(CString pathToExcel)
{
	for (size_t i = 0; i < path.size(); i++)
		if (pathToExcel.Compare(path[i]) == 0)
			return true;

	return false;
}

// Имеется ли отчет об ошибках по данной книге
int CPIVWorker::findReportBook(CString nameBook)
{
	int result = -1;

	for (size_t i = 0; i < errorDB.size(); i++)
		if (errorDB[i].name.Compare(nameBook) == 0)
			result = i;

	return result;
}
