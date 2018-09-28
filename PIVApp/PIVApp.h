
// PIVApp.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CPIVWorkerApp:
// О реализации данного класса см. PIVApp.cpp
//

class CPIVWorkerApp : public CWinApp
{
public:
	CPIVWorkerApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация
	HACCEL m_hAccel;	// хоткеи

	DECLARE_MESSAGE_MAP()
	virtual BOOL ProcessMessageFilter(int, LPMSG);	// Передача акселераторов
};

extern CPIVWorkerApp theApp;
