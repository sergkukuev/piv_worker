#pragma once
// Автоматически созданные классы для работы с Excel
#include "excel\CApplication.h"
#include "excel\CRange.h"
#include "excel\CWorkbook.h"
#include "excel\CWorkbooks.h"
#include "excel\CWorksheet.h"
#include "excel\CWorksheets.h"

#include "MyException.h"

// Класс доступа к данным excel файлов 
class PIV_DECLARE CWorkExcel : private CApplication, private CRange, private CWorkbooks, private CWorkbook, private CWorksheets, private CWorksheet
{
public:
	// Открытие excel-файла
	CWorkExcel(const CString&);
	CWorkExcel();
	~CWorkExcel();
	
	// Адрес ячейки в excel таблице
	struct Cell
	{
		long row = 0;		// Строка
		long column = 0;	// Столбец
	};

	// Открытие книги
	bool OpenBook(const CString&);
	// Закрытие книги
	void CloseBook();
	// Имя книги 
	CString BookName();
	// Имя книги из пути файла
	CString BookName(const CString&);

	// Открытие листа
	bool OpenSheet(const long& index);
	// Имя текущего листа
	CString SheetName();		
	// Количество листов в текущей книги
	long CountSheets();	
	// Количество строк на текущем листе
	long CountRows();

	// Значение ячейки
	CString CellValue(const long& row, const long& column);	
	// Значение ячейки
	CString CellValue(const Cell&);
	// Поиск ячейки по содержимому
	// Результат не найден, ячейка (-1,-1)
	bool FindCell(const CString& field, Cell&);	
	// Границы листа
	Cell Boundary();
	// Количество пустых ячеек выше указанной
	long CPrevEmpty(const long& row, const long& column);
	// Количество пустых ячеек ниже указанной
	long CNextEmpty(const long& row, const long& column);
	// Количество слитых ячеек
	long GetMerge(long& row, const long& column);
protected:
	Cell Start() { return first; }	// Первая ячейка excel
	Cell End() { return last; }		// Последняя ячейка excel
private:
	COleSafeArray* cells = NULL;	// Данные текущего листа
	Cell first;		// Первая ячейка текущего листа
	Cell last;		// Последня ячейка текущего листа

	void Initialize();	// Инициализация excel
	void Destroy();		// Закрытие всех компонентов excel
	CString LongToChar(const long& column);		// Преобразование long к char
	void StepLongToChar(const long& column, CString& result);	// Доп функция: Если в обозначении ячейки уже больше одной буквы
};