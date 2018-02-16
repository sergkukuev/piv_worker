#include "stdafx.h"
#include "Settings.h"

using namespace stgdll;

CSettings::CSettings()
{
	parameters.iProject = project::p930m;
	parameters.iMethod = method::patterned;
	parameters.bNumPK = false;
	parameters.bGenTxt = false;
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

	path.Format(L"%s%s", pathToSave, BASE_FOLDER);	// ��������� ���� �������� ����������
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
