// PIVWorker.h: главный файл заголовка для DLL PIVWorker
//

#pragma once

#ifndef __AFXWIN_H__
#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"	
#include "Logger.h"			// логирование
#include "Settings.h"		// Настройки
#include "ReaderExcel.h"	// чтение протоколов
#include "Test.h"			// проверка на ошибки протоколов
#include "Report.h"			// создание отчетов об ошибках

 // TODO: Изменить описание в связи расширением функций
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

	bool IsUpdate();		// Проверка статуса работы DLL
	CString GetStatus();	// Получение статуса DLL
	void WriteLog(const CString& msg);	// Запись в лог

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
	
	stgdll::stgParams GetSettings();						// Получение текущих настроек
	void SetPathToSave(const CString& pathToReport);// Установка пути хранения артефактов
	void SetSettings(const stgdll::stgParams& parameters);	// Установка настроек DLL

	friend void Thread(CPIV& piv);	// Запуск операций DLL в потоке
private:
	HANDLE primary;		// Основной поток
	stgdll::CSettings& settings = stgdll::CSettings::Instance();	// Указатель на настройки
	logdll::CLogger& logger = logdll::CLogger::Instance();	// Указатель на логирование

	pivData project;	// Данные проекта
	pivData other;		// Данные остальных протоколов

	const enum command {open, add, refresh, close};	// Набор потоковых команд
	int hCmd;					// Текущая потоковая команда

	vector <CString> buffer;	// Временное хранение путей протоколов

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
	void Refresh(pivData& data, const bookData& book, const errorData& error);	// Общее обновление протоколов (данные + база ошибок)
	void Refresh(pivData& data, const bookData& book);							// Обновление только данных
	void Refresh(pivData& data, const errorData& error);							// Обновление только базы ошибок
	
	list<bookData>::iterator GetBook(pivData& data, const CString& pathToExcel);		// Получение указателя на данные требуемого протокола
	CString NameFromPath(const CString& pathToExcel);					// Выделение имени протокола из его пути
	bool IsContain(pivData& data, const CString& pathToExcel);			// Проверка наличия данных требуемого протокола

	// Работа с потоком
	bool GetStatusThread(const HANDLE& h);	// Проверка доступности потока
	void CloseThread(HANDLE& h);			// Закрытие потока
};