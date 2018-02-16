#pragma once

#include <cstring>
#include "Defines.h"

class PIV_DECLARE CPIV;	// ���������� ������������� ������

namespace stgdll
{
	// ���������
	struct stgParams
	{
		int iProject;	// �������� �������: 0 - 930�, 1 - �����35
		int iMethod;	// ����� ��������: 0 - ���������, 1 - �������
		bool bNumPK;	// ���������� ����� �������� ��� ��������� txt
		bool bGenTxt;	// ��������� txt � ����� ������
	};

	// ����������� 
	const enum project { p930m, kprno35 };
	const enum method { patterned, fasted };

	// ����� ��������
	class PIV_DECLARE CSettings
	{
		friend class CPIV;
	public:
		static CSettings& Instance()
		{
			static CSettings stg;
			return stg;
		}

		// ��������� ���������� ���������
		stgParams GetParameters();
		int GetProject();	// ������ ��������
		int GetMethod();	// ����� ��������
		bool GetNumPk();	// ���� ��������� ������ �������� ��� ��������� txt
		bool GetGenTxt();	// ���� �������������� ��������� txt

		CString* GetRefPath();	// ������ �� ������� ���� �������
		CString GetPath();		// ������� ����
	
	private:
		CSettings();
		~CSettings();

		CSettings(CSettings const&) = delete;	// ������� ����������� �����������
		CSettings& operator= (CSettings const&) = delete;	// � ������������ ����

		void SetStgPath(const CString&);	// ��������� ���� ����������

		stgParams parameters;	// �������� ���������
		CString path = L"";		// �������� ���� ����� � �����������
	};
};
