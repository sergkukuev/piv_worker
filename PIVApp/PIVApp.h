
// PIVApp.h : ������� ���� ��������� ��� ���������� PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"		// �������� �������


// CPIVApp:
// � ���������� ������� ������ ��. PIVApp.cpp
//

class CPIVApp : public CWinApp
{
public:
	CPIVApp();

// ���������������
public:
	virtual BOOL InitInstance();

// ����������

	DECLARE_MESSAGE_MAP()
};

extern CPIVApp theApp;
