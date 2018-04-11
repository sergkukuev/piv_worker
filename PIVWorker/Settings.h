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
		bool bNpBase = false;		// ������������� ���� � �������� �������
	};

	// ����������� �������, ������� ������� ����� ���� ��� ���������� ����������� ����
	static bool CreateDir(CString path)
	{
		BOOL result = !PathIsDirectory(path);
		while (CreateDirectory(path, NULL) == FALSE && result)
		{
			int pos = path.ReverseFind(L'\\');
			CString str = path.Mid(0, pos);
			str.ReverseFind(L'\\') != -1 ? result = CreateDir(str) : result = FALSE;
		}
		return result;
	}

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
		bool GetNpBase();	// ���� ������������� ���� ������� �������
		
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
