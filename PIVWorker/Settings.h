#pragma once

#include <cstring>
#include "Defines.h"

namespace stgdll
{
	// Структуры
	typedef struct
	{
		int iProject;	// Проверка проекта: 0 - 930М, 1 - кпрно35
		int iMethod;	// Метод проверки: 0 - шаблонная, 1 - быстрая
		bool bNumPK;	// Установить номер подкадра при генерации txt
		bool bGenTxt;	// Генерация txt в любом случае
	} stgParams;

	// Объединения 
	const enum project { p930m, kprno35 };
	const enum method { patterned, fasted };

	// Класс настроек
	class PIV_DECLARE CSettings
	{
	public:
		CSettings();
		~CSettings();

		// Получение параметров настройки
		stgParams GetParameters();
		int GetProject();	// Проект проверки
		int GetMethod();	// Метод проверки
		bool GetNumPk();	// Флаг установки номера подкадра при генерации txt
		bool GetGenTxt();	// Флаг принудительной генерации txt

		CString* GetRefPath();	// Ссылка на текущий путь проекта
		CString GetPath();		// Текущий путь
	protected:
		// Установка параметров (устанавливается в классе CPIVWorker)
		void SetParameters(const stgParams& params);
		void SetPath(const CString& path);
	private:
		stgParams parameters;	// Основные параметры
		CString path = L"";		// Основной путь папки с артефактами
	};

	CSettings settings;
};
