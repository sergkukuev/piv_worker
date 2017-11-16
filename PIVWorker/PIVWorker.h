// PIVWorker.h: главный файл заголовка для DLL PIVWorker
//

#pragma once

#ifndef __AFXWIN_H__
#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"
#include "StructPIV.h"		// Основные структуры и макросы		
#include "Logger.h"			// логирование
#include "ReaderExcel.h"	// чтение протоколов
#include "Test.h"			// проверка на ошибки протоколов
#include "Report.h"			// создание отчетов об ошибках
 
//	Основной класс DLL для работы с протоколами информационного взаимодействия (ПИВ)
//	Содержит следующий перечень функций:
//		- открытие проекта (набор ПИВ): чтение, проверка, создание артефактов (отчет + txt наборы);
//		- открытие отдельных ПИВ: чтение, проверка, создание артефактов (отчет + txt наборы);
//		- обновление выбранных ПИВ;
//		- закрытие выбранных ПИВ;
//		- установка пути для хранения артефактов
//		- установка необходимых флагов для генерации артефактов (наличие подкадров и т.д.)
class PIV_DECLARE CPIV 
{
public:
	CPIV();		// Конструктор
	~CPIV();	// Деструктор

	CLogger logger;

	void Open(const vector<CString> pathToExcel, const CString pathToReport);	// Открытие проекта (набора ПИВ) с установкой пути хранения артефактов
	void Open(const vector<CString> pathToExcel);								// использовать старый путь хранения

	void Add(const vector<CString> pathToExcel);		// Открытие отдельных протоколов
	void Add(const CString pathToExcel);

	void Refresh(const vector<CString> pathToExcel);	// Обновление протоколов
	void Refresh(const CString pathToExcel);

	void Close();										// Закрытие всех протоколов
	void Close(const vector<CString> pathToExcel);		// выбранных
	void Close(const CString pathToExcel);				// одного

	// Получение путей артефактов
	CString GetProjectPath();
	CString GetOtherPath();
	CString GetPath();

	void SetPathToSave(const CString pathToReport);	// Установка пути хранения артефактов
	void SetStatusNumPK(const bool status);			// Установка флага bNumPK (значение подкадра)

protected:
	friend void Thread(CPIV& piv);	// Запуск операций DLL в потоке

private:
	HANDLE primary;		// Основной поток

	pivData project;	// Данные проекта
	pivData other;		// Данные остальных протоколов

	const enum command {open, add, refresh, close};	// Набор потоковых команд
	int hCmd;					// Текущая потоковая команда

	vector <CString> buffer;	// Временное хранение путей протоколов
	CString path;				// Путь сохранения отчетов
	bool bNumPK = false;		// Значение установки подкадра в txt: устанавливать (true) или не устанавливать (false)

	// Методы запуска потока для старта операций:
	void StartOpen();		// открытие проекта
	void StartAdd();		// открытие отдельного(ых) протокола(ов)
	void StartRefresh();	// обновление
	void StartClose();		// закрытие

	// Основные операции:
	void OpenExcel();		// открытие проекта
	void AddExcel();		// открытие отдельного(ых) протокола(ов)
	void RefreshExcel();	// обновление
	void CloseExcel();		// закрытие
	void CloseProject();	// закрытие проекта

	// Дополнительные методы
	void Refresh(pivData& data, const bookData& book, const errorSet& error);	// Общее обновление протоколов (данные + база ошибок)
	void Refresh(pivData& data, const bookData& book);							// Обновление только данных
	void Refresh(pivData& data, const errorSet& error);							// Обновление только базы ошибок
	
	list<bookData>::iterator GetBook(pivData& data, const CString& pathToExcel);		// Получение указателя на данные требуемого протокола
	CString NameFromPath(const CString& pathToExcel);					// Выделение имени протокола из его пути
	bool IsContain(pivData& data, const CString& pathToExcel);			// Проверка наличия данных требуемого протокола

	// Работа с потоком
	bool GetStatusThread(const HANDLE& h);	// Проверка доступности потока
	void CloseThread(HANDLE& h);			// Закрытие потока
};