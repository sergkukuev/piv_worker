#include "stdafx.h"
#include "Settings.h"


CSettings::CSettings()
{
	parameters.iProject = project::p930m;
	parameters.iMethod = method::patterned;
	parameters.bNumPK = false;
	parameters.bGenTxt = false;
}


CSettings::~CSettings() {	}

void CSettings::SetParameters(const pivParams& params)
{
	this->parameters = params;
}

void CSettings::SetPath(const CString& pathToArtefact)
{
	if (!path.IsEmpty())
	{
		SHFILEOPSTRUCT fos;
		ZeroMemory(&fos, sizeof(fos));

		// Установка функций для переноса папки артефактов
		fos.wFunc = FO_MOVE;
		path.AppendChar(0);		// '\0' чтобы закрыть строку
		path.AppendChar(0);		// '\0' чтобы закрыть набор

		CString to = pathToArtefact;	// Формируется аналогично path
		to.AppendChar(0);	// для строки
		to.AppendChar(0);	// для всего набора

		fos.pFrom = path;
		fos.pTo = to;
		fos.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;

		SHFileOperation(&fos);
	}

	path.Format(L"%s%s", pathToArtefact, BASE_FOLDER);	// Установка пути хранения артефактов
	CreateDirectory(path, NULL);
	this->path = path;
}

pivParams CSettings::GetParameters() { return this->parameters; }

CString CSettings::GetPath() { return this->path; }

CString* CSettings::GetRefPath() { return &path; }

int CSettings::GetProject() { return this->parameters.iProject; }

int CSettings::GetMethod() { return this->parameters.iMethod; }

bool CSettings::GetNumPk() { return this->parameters.bNumPK; }

bool CSettings::GetGenTxt() { return this->parameters.bGenTxt; }
