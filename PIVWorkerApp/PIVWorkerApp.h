
// PIVWorkerApp.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CPIVWorkerApp:
// О реализации данного класса см. PIVWorkerApp.cpp
//

class CPIVWorkerApp : public CWinApp
{
public:
	CPIVWorkerApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация
	HACCEL m_hAccel;

	DECLARE_MESSAGE_MAP();

	virtual BOOL ProcessMessageFilter(int, LPMSG);
};

extern CPIVWorkerApp theApp;