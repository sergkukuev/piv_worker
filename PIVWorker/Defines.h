#pragma once

// Определение импорта или экспорта DLL

#ifdef PIV_WORKER_EXPORTS
#define PIV_DECLARE __declspec(dllimport)
#else
#define PIV_DECLARE __declspec(dllexport)
#endif

// Папки
#define BASE_FOLDER L"\\Artefacts"
#define TEXT_FOLDER L"\\Text"
#define PROJECT_FOLDER L"\\Project"
#define OTHER_FOLDER L"\\Other"
#define SYNTAX_FOLDER L"\\Error\\Syntax"
#define SIMANTIC_FOLDER L"\\Error\\Simantic"
#define WARNING_FOLDER L"\\Warning"

// Длины названия основных папок
#define PROJECT_SIZE 8
#define OTHER_SIZE 6

// Файлы
#define REPORT_NAME L"Отчет.html"
#define LOG_FILE_NAME L"log.txt"

// Идентификаторы значений таблицы
#define RESERVE_SIGNAL L"Резерв"

// Статусы DLL и логирование
#define LOG_THREAD_BUSY L"Поток занят! Подождите окончания процесса!"
#define LOG_SLASH	L"================================================================\n"
#define LOG_START	L"Приложение запушено"
#define LOD_END		L"Приложение закрыто"
#define LOG_OPEN	L"Открытие ПИВ завершено"
#define LOG_REFRESH L"Обновление ПИВ завершено"
#define LOG_ADD		L"Добавление ПИВ завершено"
#define LOG_CLOSE	L"Закрытие ПИВ завершено"
#define LOG_FOLDER	L"Изменено расположение папки отчетов"
#define LOG_ERROR	L"При операции произошла ошибка (подробное описание см. в логах)"