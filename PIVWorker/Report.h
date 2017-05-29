#pragma once

#include <fstream>
#include "StructPIV.h"

struct Amount	// Структура подсчета наборов данных и их ошибок
{
	int allSets = 0;
	int setsWithError = 0;
	int setsWithoutError = 0;
	int error = 0;
	int warning = 0;
};

// Шапки таблицы
const CString errRemarks[7] =
{
	// Номер параметра
	_T("Замечание. Поле \"№ слова\" заполнено не верно."),
	// Обозначение параметра
	_T("Замечание. Поле \"Обозначение сигнала\" заполнено не верно."),
	// Проверка мин, макс, ЦСР
	_T("Замечание. Поле \"Минимальное значение\" заполнено не верно."),
	_T("Замечание. Поле \"Максимальное значение\" заполнено не верно."),
	_T("Замечание. Поле \"Цена старшего разряда\" заполнено не верно."),
	// Разряды
	_T("Замечание. Поле \"Используемые разряды\" заполнено не верно."),
	_T("Замечание. Поле \"Примечание\" заполнено не верно.")
};

// Класс генерации отчета
class CReport
{
public:
	CReport();	// Конструктор
	~CReport();	// Деструктор

	bool Generate(vector <bookData> books, errorSet errorDB);	// Генерация отчета
	void setPath(CString path);	// Установка пути сохранения

private:
	CString path;	// Путь сохранения
	Amount amount;	// Количество наборов данных и их ошибок

	vector <bookData> books;	// книги
	errorSet errorDB;			// База ошибок

	void startGenerate();	// Начало генерации отчета
	void makeReport();		// Генерация не пустого отчета
	void makeEmptyReport();	// Генерация пустого отчета

	void writeSheets(ofstream& file, int cBooks);		// Создание страниц
	void writeError(ofstream& file, errorBookData error, CString folder);	// Создание страницы с ошибками
	
	void Clear();			// Очистка переменных
	int getSetsAmount();	// Количество наборов данных
	int getSetsAmountWithError();	// Количество наборов данных с ошибкой
	int getErrorAmount(vector <errorBookData> error);	// Количество ошибок

	void hightlightError(ofstream& file, int tIndx, int Indx, CString field);	// Подсветка нужного параметра
	int getIndexErrorField(list<errorSignalData>::iterator it);					// Поиск индекса параметра
};

