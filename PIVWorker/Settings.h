#pragma once

#include <cstring>
#include "Defines.h"

struct pivParams
{
	int iProject;	// �������� �������: 0 - 930�, 1 - �����35
	int iMethod;	// ����� ��������: 0 - ���������, 1 - �������
	bool bNumPK;	// ���������� ����� �������� ��� ��������� txt
	bool bGenTxt;	// ��������� txt � ����� ������
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
	pivParams parameters;	// �������� ���������
	CString path = L"";		// �������� ���� ����� � �����������
};

