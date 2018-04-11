#pragma once

#include <cstring>
#include <vector>

#include "DllDeclare.h"

class PIV_DECLARE CPIV;	// Объявление существующего класса

namespace stgdll
{
	// Объединения 
	const enum folders { base, text, project, other, error, syntax, simantic, warning }; // индексы папок
	const enum project { p930m, kprno35 };		// Объект проверки
	const enum method { patterned, fasted };	// Метод проверки

	// Структуры
	struct stgParams
	{
		int iProject = p930m;		// Проверка проекта: 0 - 930М, 1 - кпрно35
		int iMethod = patterned;	// Метод проверки: 0 - шаблонная, 1 - быстрая
		bool bNumPK = false;		// Установить номер подкадра при генерации txt
		bool bGenTxt = false;		// Генерация txt в любом случае
		bool bNpBase = false;		// Использование базы с номерами наборов
	};

	// Рекурсивная функция, которая создает папку даже при отсутствии предыдущего пути
	static bool CreateDir(CString path)
	{
		BOOL result = !PathIsDirectory(path);
		while (CreateDirectory(path, NULL) == FALSE && result)
		{
			int pos = path.ReverseFind(L'\\');
			CString str = path.Mid(0, pos);
			str.ReverseFind(L'\\') != -1 ? result = CreateDir(str) : result = FALSE;
		}
		return result;
	}

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
		// Папки артефактов
		const std::vector<CString> folders = { L"\\Artefacts", L"\\Text", L"\\Project", L"\\Other", L"\\Error",
			L"\\Error\\Syntax", L"\\Error\\Simantic", L"\\Warning" };

		// Получение параметров настройки
		stgParams GetParameters();
		int GetProject();	// Проект проверки
		int GetMethod();	// Метод проверки
		bool GetNumPk();	// Флаг установки номера подкадра при генерации txt
		bool GetGenTxt();	// Флаг принудительной генерации txt
		bool GetNpBase();	// Флаг использования базы номеров наборов
		
		CString GetDefaultPath();	// Путь папки с exe
		CString GetPath();			// Текущий путь
	private:
		CSettings();	// Конструктор	
		~CSettings();	// Деструктор

		CSettings(CSettings const&) = delete;	// Удаляем конструктор копирования
		CSettings& operator= (CSettings const&) = delete;	// И присваивания тоже

		void WriteRegKeys();	// Запись ключей в реестр
		void SetStgPath(const CString&);	// Установка пути артефактов
		void Save(const stgParams& par);	// Сохранение параметров в файл

		stgParams parameters;	// Основные параметры
		CString path = L"";		// Основной путь папки с артефактами
	};
};
