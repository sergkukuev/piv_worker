
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
	HACCEL m_hAccel;	// ������

	DECLARE_MESSAGE_MAP()
	virtual BOOL ProcessMessageFilter(int, LPMSG);	// �������� �������������
};

extern CPIVApp theApp;
