
// PIVApp.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CPIVApp:
// О реализации данного класса см. PIVApp.cpp
//

class CPIVApp : public CWinApp
{
public:
	CPIVApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CPIVApp theApp;
