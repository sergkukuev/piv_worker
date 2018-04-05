#pragma once

#include <cstring>
#include <vector>

#include "DllDeclare.h"

class PIV_DECLARE CPIV;	// ���������� ������������� ������

namespace stgdll
{
	// ����������� 
	const enum folders { base, text, project, other, error, syntax, simantic, warning }; // ������� �����
	const enum project { p930m, kprno35 };		// ������ ��������
	const enum method { patterned, fasted };	// ����� ��������

	// ���������
	struct stgParams
	{
		int iProject = p930m;		// �������� �������: 0 - 930�, 1 - �����35
		int iMethod = patterned;	// ����� ��������: 0 - ���������, 1 - �������
		bool bNumPK = false;		// ���������� ����� �������� ��� ��������� txt
		bool bGenTxt = false;		// ��������� txt � ����� ������
	};

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
		// ����� ����������
		const std::vector<CString> folders = { L"\\Artefacts", L"\\Text", L"\\Project", L"\\Other", L"\\Error",
			L"\\Error\\Syntax", L"\\Error\\Simantic", L"\\Warning" };

		// ��������� ���������� ���������
		stgParams GetParameters();
		int GetProject();	// ������ ��������
		int GetMethod();	// ����� ��������
		bool GetNumPk();	// ���� ��������� ������ �������� ��� ��������� txt
		bool GetGenTxt();	// ���� �������������� ��������� txt
		
		CString GetDefaultPath();	// ���� ����� � exe
		CString GetPath();			// ������� ����
	private:
		CSettings();	// �����������	
		~CSettings();	// ����������

		CSettings(CSettings const&) = delete;	// ������� ����������� �����������
		CSettings& operator= (CSettings const&) = delete;	// � ������������ ����

		void WriteRegKeys();	// ������ ������ � ������
		void SetStgPath(const CString&);	// ��������� ���� ����������
		void Save(const stgParams& par);	// ���������� ���������� � ����

		stgParams parameters;	// �������� ���������
		CString path = L"";		// �������� ���� ����� � �����������
	};
};
