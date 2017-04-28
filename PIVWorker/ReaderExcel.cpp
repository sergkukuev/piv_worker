#include "stdafx.h"
#include "ReaderExcel.h"

// Конструктор
CReaderExcel::CReaderExcel()
{
}

// Деструктор
CReaderExcel::~CReaderExcel()
{
}

// Конструктор с параметром
CReaderExcel::CReaderExcel(vector <CString>& pathToExcel)
{
	path = pathToExcel;

	// Открытие книг
	for (size_t i = 0; i < path.size(); i++)
		work.openWorkBook(path[i]);

	// Установка набора заголовков
	HeaderTable.push_back(list<CString>{ _T("№ слов"), _T("№ п/п"), _T("№ слова в подадресе") });
	HeaderTable.push_back(list<CString>{ _T("Наименование параметра"), _T("Наименование сигнала") });
	HeaderTable.push_back(list<CString>{ _T("Обозначение сигнала"), _T("Условное обозначение параметра / сигнала") });
	HeaderTable.push_back(list<CString>{ _T("Размерность"), _T("Единица измерения") });
	HeaderTable.push_back(list<CString>{ _T("Минимальное значение"), _T("Миним. значение"), _T("Минимал значение") });
	HeaderTable.push_back(list<CString>{ _T("Максимальное значение"), _T("Максим. значение"), _T("Максимал значение") });
	HeaderTable.push_back(list<CString>{ _T("Цена старшего разряда") });
	HeaderTable.push_back(list<CString>{ _T("Используемые разряды"), _T("Используе-мые разряды") });
	HeaderTable.push_back(list<CString>{ _T("Примечание") });
}

// Получить количество книг
int CReaderExcel::getSize()
{
	return work.getCountBooks();
}

// Чтение книг
vector <bookData> CReaderExcel::getBooks()
{
	vector <bookData> books;
	books.resize(work.getCountBooks());

	for (int i = 1; i < books.size() + 1; i++)
	{
		long clBooks = static_cast<long>(i);

		work.setActivBook(clBooks);
		books[i - 1].nameBook = work.getNameBook();
		books[i - 1].sheets.resize(work.getCountSheets());

		// Считать листы
	}

	return books;
}
