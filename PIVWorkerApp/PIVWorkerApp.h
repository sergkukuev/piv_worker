
// PIVWorkerApp.h : ������� ���� ��������� ��� ���������� PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"		// �������� �������


// CPIVWorkerApp:
// � ���������� ������� ������ ��. PIVWorkerApp.cpp
//

class CPIVWorkerApp : public CWinApp
{
public:
	CPIVWorkerApp();

// ���������������
public:
	virtual BOOL InitInstance();

// ����������

	DECLARE_MESSAGE_MAP()
};

extern CPIVWorkerApp theApp;