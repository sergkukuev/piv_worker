#include "stdafx.h"
#include "Settings.h"

using namespace stgdll;

CSettings::CSettings()
{
	GetModuleFileName(NULL, path.GetBuffer(_MAX_PATH), _MAX_PATH);
	path.ReleaseBuffer();
	path.Delete(path.ReverseFind(L'\\'), path.GetLength());
	path.Format(L"%s%s", path, folders[base]);	// ��������� ���� �������� ����������
	CreateDirectory(path, NULL);
}

CSettings::~CSettings() {	}

// ��������� ����
void CSettings::SetStgPath(const CString& pathToSave)
{
	if (!path.IsEmpty())
	{
		SHFILEOPSTRUCT fos;
		ZeroMemory(&fos, sizeof(fos));

		// ��������� ������� ��� �������� ����� ����������
		fos.wFunc = FO_MOVE;
		path.AppendChar(0);		// '\0' ����� ������� ������
		path.AppendChar(0);		// '\0' ����� ������� �����

		CString to = pathToSave;	// ����������� ���������� path
		to.AppendChar(0);	// ��� ������
		to.AppendChar(0);	// ��� ����� ������

		fos.pFrom = path;
		fos.pTo = to;
		fos.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;

		SHFileOperation(&fos);
	}

	path.Format(L"%s%s", pathToSave, folders[base]);	// ��������� ���� �������� ����������
	CreateDirectory(path, NULL);
}

// ��������� ���������� ���������
stgParams CSettings::GetParameters() { return this->parameters; }

CString CSettings::GetPath() { return this->path; }

CString* CSettings::GetRefPath() { return &path; }

// �������� ���������
int CSettings::GetProject() { return this->parameters.iProject; }

int CSettings::GetMethod() { return this->parameters.iMethod; }

bool CSettings::GetNumPk() { return this->parameters.bNumPK; }

bool CSettings::GetGenTxt() { return this->parameters.bGenTxt; }
