#pragma once

#include <cstring>
#include "Defines.h"

class PIV_DECLARE CPIV;	// Объявление существующего класса

namespace stgdll
{
	// Структуры
	struct stgParams
	{
		int iProject;	// Проверка проекта: 0 - 930М, 1 - кпрно35
		int iMethod;	// Метод проверки: 0 - шаблонная, 1 - быстрая
		bool bNumPK;	// Установить номер подкадра при генерации txt
		bool bGenTxt;	// Генерация txt в любом случае
	};

	// Объединения 
	const enum project { p930m, kprno35 };
	const enum method { patterned, fasted };

	// Класс настроек
	class PIV_DECLARE CSettings
	{
		friend class CPIV;
	public:
		static CSettings& Instance()
		{
			static CSettings stg;
			return stg;
		}

		// Получение параметров настройки
		stgParams GetParameters();
		int GetProject();	// Проект проверки
		int GetMethod();	// Метод проверки
		bool GetNumPk();	// Флаг установки номера подкадра при генерации txt
		bool GetGenTxt();	// Флаг принудительной генерации txt

		CString* GetRefPath();	// Ссылка на текущий путь проекта
		CString GetPath();		// Текущий путь
	
	private:
		CSettings();
		~CSettings();

		CSettings(CSettings const&) = delete;	// Удаляем конструктор копирования
		CSettings& operator= (CSettings const&) = delete;	// И присваивания тоже

		void SetStgPath(const CString&);	// Установка пути артефактов

		stgParams parameters;	// Основные параметры
		CString path = L"";		// Основной путь папки с артефактами
	};
};
