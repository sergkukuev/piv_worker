#pragma once

#include <cstring>
#include "Defines.h"

struct pivParams
{
	int iProject;	// Проверка проекта: 0 - 930М, 1 - кпрно35
	int iMethod;	// Метод проверки: 0 - шаблонная, 1 - быстрая
	bool bNumPK;	// Установить номер подкадра при генерации txt
	bool bGenTxt;	// Генерация txt в любом случае
};

class PIV_DECLARE CSettings
{
public:
	CSettings();
	~CSettings();
	
	const enum project { p930m, kprno35 };
	const enum method { patterned, fasted };

	void SetParameters(const pivParams& params);
	void SetPath(const CString& path);

	pivParams GetParameters();
	int GetProject();
	int GetMethod();
	bool GetNumPk();
	bool GetGenTxt();
	CString* GetRefPath();
	CString GetPath();
private:
	pivParams parameters;	// Основные параметры
	CString path = L"";		// Основной путь папки с артефактами
};

