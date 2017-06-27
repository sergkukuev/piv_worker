#pragma once

#include "excel\CApplication.h"
#include "excel\CRange.h"
#include "excel\CWorkbook.h"
#include "excel\CWorkbooks.h"
#include "excel\CWorksheet.h"
#include "excel\CWorksheets.h"
#include "MyException.h"

#include <cstring>
#include <vector>

// Структура заголовка
struct Header {
	std::vector <std::vector<CString>> list;	// Список допустимых заголовков 
	static const int size = 10;		// Количество заголовков
	long adress[size];				// Массив адресов для заголовков

	const int iRow = 0;			//	Индекс строки в массиве адресов
	const int iNumWord = 1;		//	Индекс слова в массиве адресов
	const int iName = 2;		//	Индекс наименования сигнала в массиве адресов
	const int iSignal = 3;		//	Индекс идентификатора сигнала в массиве адресов
	const int iDimension = 4;	//	Индекс размерности в массиве адресов
	const int iMin = 5;			//	Индекс мин.знач. в массиве адресов
	const int iMax = 6;			//	Индекс макс.знач. в массиве адресов
	const int iCSR = 7;			//	Индекс цср в массиве адресов
	const int iBits = 8;		//	Индекс битов в массиве адресов
	const int iComment = 9;		//	Индекс примечаний в массиве адресов
};

// Адрес ячейки
struct Cell {
	long row;		// Строка
	long column;	// Столбец
};

#define LINE_FIELD L"Линия"
#define NP_FIELD L"NP="
#define PK_FIELD L'№'
#define REMARK1 L"Примечания:"
#define REMARK2 L"Примечание:"

// Класс чтения данных из Excel 
class CWorkExcel {
public:
	CWorkExcel(void);	// Конструктор
	~CWorkExcel(void);	// Деструктор
	
	bool openBook(const CString& path);		// Открытие книги
	CString bookName();						// Имя книги из файла
	CString bookName(const CString& path);	// Имя книги из пути

	bool openSheet(const long& index);	// Открытие листа
	CString sheetName();				// Имя листа
	long countSheets();					// Количество листов в книги
	long countRows();					// Количество столбцов на текущем листе

	VARIANT lineValue();				// Получение значения линии передачи
	int npValue(const Header& head);	// Получение номера набора
	int pkValue(const Header& head);	// Получение значения номера подкадра			

	VARIANT cellValue(const long& row, const long& column);	// Получение значения ячейки
	VARIANT cellValue(const Cell& cell);					// Перегрузка
	long getMerge(long& row, const long& column);			// Кол-во пустых ячеек по стобцам
	bool findHeader(Header& header);						// Поиск заголовков на текущем листе

private:
	CApplication app;	// Приложение excel
	CWorkbooks books;	// Книги
	CWorkbook book;		// Книга
	CWorksheets sheets;	// Листы в книге
	CWorksheet sheet;	// Лист

	COleSafeArray* cells;	// Данные листа
	Cell first, last;		// Индексы первой и последней ячейки

	bool findCell(const CString& field, Cell& cell);	// Поиск ячейки по содержимому, в противном cell(-1,-1)
};

