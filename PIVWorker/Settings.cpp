#include "stdafx.h"
#include "Settings.h"

using namespace stgdll;

CSettings::CSettings()
{
	GetModuleFileName(NULL, path.GetBuffer(_MAX_PATH), _MAX_PATH);
	path.ReleaseBuffer();
	path.Delete(path.ReverseFind(L'\\'), path.GetLength());
	path.Format(L"%s%s", path, folders[base]);	// Установка пути хранения артефактов
	CreateDirectory(path, NULL);
}

CSettings::~CSettings() {	}

// Установка пути
void CSettings::SetStgPath(const CString& pathToSave)
{
	if (!path.IsEmpty())
	{
		SHFILEOPSTRUCT fos;
		ZeroMemory(&fos, sizeof(fos));

		// Установка функций для переноса папки артефактов
		fos.wFunc = FO_MOVE;
		path.AppendChar(0);		// '\0' чтобы закрыть строку
		path.AppendChar(0);		// '\0' чтобы закрыть набор

		CString to = pathToSave;	// Формируется аналогично path
		to.AppendChar(0);	// для строки
		to.AppendChar(0);	// для всего набора

		fos.pFrom = path;
		fos.pTo = to;
		fos.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;

		SHFileOperation(&fos);
	}

	path.Format(L"%s%s", pathToSave, folders[base]);	// Установка пути хранения артефактов
	CreateDirectory(path, NULL);
}

// Получение параметров настройки
stgParams CSettings::GetParameters() { return this->parameters; }

CString CSettings::GetPath() { return this->path; }

CString* CSettings::GetRefPath() { return &path; }

// Основные параметры
int CSettings::GetProject() { return this->parameters.iProject; }

int CSettings::GetMethod() { return this->parameters.iMethod; }

bool CSettings::GetNumPk() { return this->parameters.bNumPK; }

bool CSettings::GetGenTxt() { return this->parameters.bGenTxt; }
