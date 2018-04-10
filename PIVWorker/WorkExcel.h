#pragma once

#include <cstring>
#include <vector>

// Автоматически созданные классы для работы с Excel
#include "excel\CApplication.h"
#include "excel\CRange.h"
#include "excel\CWorkbook.h"
#include "excel\CWorkbooks.h"
#include "excel\CWorksheet.h"
#include "excel\CWorksheets.h"

#include "MyException.h"

namespace exceldll
{
	// Индексы заголовков в массиве адресов
	const enum index {
		iRow = 0,	// Текущая строка 
		iNumWord,	// Номер слова
		iName,		// Наименование сигнала 
		iSignal,	// Идентификатор сигнала
		iDimension,	// Размерность
		iMin,		// Мин.знач.
		iMax,		// Макс.знач.
		iCSR,		// Цср 
		iBits,		// Используемые разряды
		iComment,	// Примечание
		iAdress,	// Адрес (для arinc)
	};
	// Хранение заголовков таблицы протоколов
	struct Header
	{
		vector <vector<CString>> list;	// Список допустимых заголовков 
		static const int size = 11;		// Количество заголовков
		long adress[size];				// Адреса заголовков
	};

	// Ячейка в таблице Excel
	struct Cell
	{
		long row;		// Строка
		long column;	// Столбец
	};
}

// Класс чтения данных из таблицы Excel 
class PIV_DECLARE CWorkExcel 
{
public:
	CWorkExcel(void);	// Конструктор
	~CWorkExcel(void);	// Деструктор
	
	bool OpenBook(const CString& path);		// Открытие книги
	// Получение имени книги:
	CString BookName(const CString& path);	// из пути
	CString BookName();						// из приложения Excel
	
	bool OpenSheet(const long& index);	// Открытие листа
	CString SheetName();		// Получение имени текущего листа
	long CountSheets();			// Получение количества листов в книге
	long CountRows();			// Получение количества строк на текущем листе	

	// Получение значения ячейки 
	CString CellValue(const long& row, const long& column);	
	CString CellValue(const exceldll::Cell& cell);
	exceldll::Cell Boundary();	// Получение границы страницы excel листа

	// Работа с excel
	long CPrevEmpty(const long& row, const long& column);	// Количество пустых ячеек до
	long CNextEmpty(const long& row, const long& column);	// Количество пустых ячеек после 
	long GetMerge(long& row, const long& column);			// Кол-во слитых ячеек
	bool FindCell(const CString& field, exceldll::Cell& cell);		// Поиск ячейки по содержимому, в противном cell(-1,-1)
	bool FindHeader(exceldll::Header& header, const bool& arinc);	// Поиск заголовков на текущем листе
private:
	CApplication app;	
	CWorkbooks books;	
	CWorkbook book;		
	CWorksheets sheets;	
	CWorksheet sheet;	

	COleSafeArray* cells;	// Данные текущего листа
	exceldll::Cell first, last;		// Значения первой и последней ячейки текущего листа
	CString LongToChar(const long& column);						// Преобразование long к char
	void StepLongToChar(const long& column, CString& result);	// Доп функция: Если в обозначении ячейки уже больше одной буквы
};

