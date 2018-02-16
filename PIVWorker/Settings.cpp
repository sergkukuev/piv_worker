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

// ��������� ����������
void CSettings::SetParameters(const stgParams& params)
{
	this->parameters = params;
}

// ��������� ����
void CSettings::SetPath(const CString& pathToArtefact)
{
	if (!path.IsEmpty())
	{
		SHFILEOPSTRUCT fos;
		ZeroMemory(&fos, sizeof(fos));

		// ��������� ������� ��� �������� ����� ����������
		fos.wFunc = FO_MOVE;
		path.AppendChar(0);		// '\0' ����� ������� ������
		path.AppendChar(0);		// '\0' ����� ������� �����

		CString to = pathToArtefact;	// ����������� ���������� path
		to.AppendChar(0);	// ��� ������
		to.AppendChar(0);	// ��� ����� ������

		fos.pFrom = path;
		fos.pTo = to;
		fos.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;

		SHFileOperation(&fos);
	}

	path.Format(L"%s%s", pathToArtefact, BASE_FOLDER);	// ��������� ���� �������� ����������
	CreateDirectory(path, NULL);
	this->path = path;
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
