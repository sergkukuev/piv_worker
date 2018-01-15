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

// Обозначения способов передачи данных
#define LINE_FIELD L"Линия"	// Ячейка записи линии передачи данных
#define ARINC L"РТМ"	
#define MKIO L"МКИО"

// Специальные обозначения для MKIO
#define SIGN_FIELD L"Зн-4"	// Наличие отрицательного символа
#define NP_FIELD L"NP="		// Значение номера набора
#define PK_FIELD L'№'		// Номер подкадра

// Специальные обозначения для ARINC
#define ARINC_REMARK L"*Примечание:"

// Примечания в конце таблицы
#define REMARK1 L"Примечания:"
#define REMARK2 L"Примечание:"

// Хранение заголовков таблицы протоколов
struct Header 
{
	std::vector <std::vector<CString>> list;	// Список допустимых заголовков 
	static const int size = 11;		// Количество заголовков
	long adress[size];				// Адреса заголовков

	// Индексы заголовков в массиве адресов:
	const int iRow = 0;			// Текущая строка 
	const int iNumWord = 1;		// Номер слова
	const int iName = 2;		// Наименование сигнала 
	const int iSignal = 3;		// Идентификатор сигнала
	const int iDimension = 4;	// Размерность
	const int iMin = 5;			// Мин.знач.
	const int iMax = 6;			// Макс.знач.
	const int iCSR = 7;			// Цср 
	const int iBits = 8;		// Используемые разряды
	const int iComment = 9;		// Примечание
	const int iAdress = 10;		// Адрес (для arinc)
};

// Ячейка в таблице Excel
struct Cell 
{
	long row;		// Строка
	long column;	// Столбец
};

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

	bool IsArinc();						// Значение линии передачи (arinc или mkio)
	int NpValue(const CString& comment/*const Header& head*/);	// Значение номера набора
	int PkValue(const Header& head);	// Значение номера подкадра			

	CString CellValue(const long& row, const long& column);		// Значение ячейки
	CString CellValue(const Cell& cell);					

	long CPrevEmpty(const long& row, const long& column);		// Количество пустых ячеек до
	long CNextEmpty(const long& row, const long& column);		// Количество пустых ячеек после 
	long GetMerge(long& row, const long& column);				// Кол-во слитых ячеек
	bool FindHeader(Header& header);							// Поиск заголовков на текущем листе

private:
	CApplication app;	
	CWorkbooks books;	
	CWorkbook book;		
	CWorksheets sheets;	
	CWorksheet sheet;	

	COleSafeArray* cells;	// Данные текущего листа
	Cell first, last;		// Значения первой и последней ячейки текущего листа

	bool FindCell(const CString& field, Cell& cell);	// Поиск ячейки по содержимому, в противном cell(-1,-1)
	CString LongToChar(const long& column);						// Преобразование long к char
	void StepLongToChar(const long& column, CString& result);	// Доп функция: Если в обозначении ячейки уже больше одной буквы
};

