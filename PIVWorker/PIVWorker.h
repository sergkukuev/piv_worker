// PIVWorker.h: ������� ���� ��������� ��� DLL PIVWorker
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"		// �������� �������


// CPIVWorkerApp
// ��� ���������� ������� ������ ��. PIVWorker.cpp
//

class CPIVWorkerApp : public CWinApp
{
public:
	CPIVWorkerApp();

// ���������������
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
