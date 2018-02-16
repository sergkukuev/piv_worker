#pragma once

#include <cstring>
#include "Defines.h"

namespace stgdll
{
	// ���������
	typedef struct
	{
		int iProject;	// �������� �������: 0 - 930�, 1 - �����35
		int iMethod;	// ����� ��������: 0 - ���������, 1 - �������
		bool bNumPK;	// ���������� ����� �������� ��� ��������� txt
		bool bGenTxt;	// ��������� txt � ����� ������
	} stgParams;

	// ����������� 
	const enum project { p930m, kprno35 };
	const enum method { patterned, fasted };

	// ����� ��������
	class PIV_DECLARE CSettings
	{
	public:
		CSettings();
		~CSettings();

		// ��������� ���������� ���������
		stgParams GetParameters();
		int GetProject();	// ������ ��������
		int GetMethod();	// ����� ��������
		bool GetNumPk();	// ���� ��������� ������ �������� ��� ��������� txt
		bool GetGenTxt();	// ���� �������������� ��������� txt

		CString* GetRefPath();	// ������ �� ������� ���� �������
		CString GetPath();		// ������� ����
	protected:
		// ��������� ���������� (��������������� � ������ CPIVWorker)
		void SetParameters(const stgParams& params);
		void SetPath(const CString& path);
	private:
		stgParams parameters;	// �������� ���������
		CString path = L"";		// �������� ���� ����� � �����������
	};

	CSettings settings;
};
