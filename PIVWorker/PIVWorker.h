// PIVWorker.h: главный файл заголовка для DLL PIVWorker
//

#pragma once

#ifndef __AFXWIN_H__
#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "StructPIV.h"		// структуры протоколов
#include "resource.h"		// основные символы

#define THREAD_BUSY L"Поток занят! Подождите окончания процесса!"
// CPIVWorkerApp
// Про реализацию данного класса см. PIVWorker.cpp
//

struct cmd_set {
	const int open = 0;
	const int add = 1;
	const int refresh = 2;
	const int close = 3;
};

class PIV_DECLARE CPIV {
public:
	CPIV();		// Конструктор
	~CPIV();	// Деструктор

	LPWSTR pipeName = L"\\\\.\\pipe\\log"; // Имя канала

	void Open(const vector<CString>& pathToExcel, const CString& pathToReport);	// Открыть ПИВ и задать путь для артефактов (чтение, проверка, выдача артефактов)
	void Open(const vector<CString>& pathToExcel);								// Открыть ПИВ и использовать старый путь для артефактов

	void Add(const CString& pathToExcel);			// Добавить ПИВ
	void Add(const vector<CString>& pathToExcel);	// Добавить несколько ПИВ

	void Refresh(const CString& pathToExcel);			// Обновить ПИВ
	void Refresh(const vector<CString>& pathToExcel);	// Обновить несколько ПИВ

	void Close();									// Закрыть остальные ПИВ
	void Close(const CString& pathToExcel);			// Закрыть один ПИВ и его отчет в базе ошибок
	void Close(const vector<CString>& pathToExcel);	// Закрыть несколько ПИВ и их отчеты в базе ошибок

	void setPathToSave(const CString& pathToReport);// Установить путь для хранения артефактов
	void setStatusNumPK(const bool& status);		// Установить флаг bNumPK
protected:
	friend void Thread(CPIV& piv);	// Дружественная функция для запуска операции в потоке
private:
	HANDLE primary;		// Основной поток
	HANDLE hLogPipe;	// Именованный канал логирования

	pivData project;	// Данные проекта
	pivData other;		// Данные остальных протоколов

	const cmd_set command;		// Набор команд
	int hCmd;					// Команда для потока

	vector <CString> buffer;	// Вектор временного хранения путей файлов
	CString path;				// Путь сохранения отчетов
	bool bNumPK = false;		// Нужно ли устанавливать в txt подкадры

	void StartOpen();		// Начало открытия ПИВ
	void StartAdd();		// Начало добавления ПИВ
	void StartRefresh();	// Начало обновления ПИВ
	void StartClose();		// Начало закрытия ПИВ

	void CloseProject();			// Закрытие ПИВ проекта

									// Работа с файлами, пути которых расположены в буфере
	void OpenExcel();		// Открытие ПИВ
	void AddExcel();		// Добавление ПИВ
	void RefreshExcel();	// Обновление ПИВ
	void CloseExcel();		// Закрытие ПИВ

	void WriteLog(char szBuf[256]);	// Логирование

	void Refresh(pivData& data, const bookData& book, const errorSet& error);	// Обновление ПИВ и ошибок
	void Refresh(pivData& data, const bookData& book);			// Обновление ПИВ
	void Refresh(pivData& data, const errorSet& error);			// Обновление ошибок
	bookData& getBook(pivData& data, const CString& pathToExcel);		// Получить ссылку на книгу
	CString nameFromPath(const CString& pathToExcel);					// Извлечение имени файла из его пути
	bool IsContain(pivData& data, const CString& pathToExcel);			// Проверка наличия файла в памяти

	bool getStatusThread(const HANDLE& h);	// Проверка доступности потока
	void closeThread(HANDLE& h);			// Закрытие потока
};