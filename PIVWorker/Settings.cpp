#include "stdafx.h"
#include "Settings.h"

using namespace stgdll;

// Конструктор
CSettings::CSettings()
{
	// Чтение настроек из реестра
	path = AfxGetApp()->GetProfileStringW(L"Settings", L"Path", L"");
	if (path.IsEmpty())
		path.Format(L"%s%s", GetDefaultPath(), folders[base]); // Установка пути хранения артефактов
	CreateDirectory(path, NULL);

	parameters.iProject = AfxGetApp()->GetProfileIntW(L"Settings", L"Project", project::p930m);
	parameters.iMethod = AfxGetApp()->GetProfileIntW(L"Settings", L"Method", method::patterned);
	parameters.bNumPK = AfxGetApp()->GetProfileIntW(L"Settings", L"NumPK", false);
	parameters.bGenTxt = AfxGetApp()->GetProfileIntW(L"Settings", L"GenTXT", false);
	parameters.bComment = AfxGetApp()->GetProfileIntW(L"Settings", L"GenCOM", false);
}

// Деструктор
CSettings::~CSettings() 
{
	WriteRegKeys();
}

// Запись ключей в реестр
void CSettings::WriteRegKeys()
{
	AfxGetApp()->WriteProfileStringW(L"Settings", L"Path", path);
	AfxGetApp()->WriteProfileInt(L"Settings", L"Project", parameters.iProject);
	AfxGetApp()->WriteProfileInt(L"Settings", L"Method", parameters.iMethod);
	AfxGetApp()->WriteProfileInt(L"Settings", L"NumPK", parameters.bNumPK);
	AfxGetApp()->WriteProfileInt(L"Settings", L"GenTXT", parameters.bGenTxt);
	AfxGetApp()->WriteProfileInt(L"Settings", L"GenCOM", parameters.bComment);
}

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
	AfxGetApp()->WriteProfileStringW(L"Settings", L"Path", path);	// Запись пути в реестр
	CreateDirectory(path, NULL);
	
}

// Сохранение параметров
void CSettings::Save(const stgParams& par)
{
	parameters = par;
	WriteRegKeys();
}

// Получить путь папки с exe файлом
CString CSettings::GetDefaultPath()
{
	CString result;
	GetModuleFileName(NULL, result.GetBuffer(_MAX_PATH), _MAX_PATH);
	result.ReleaseBuffer();
	result.Delete(result.ReverseFind(L'\\'), result.GetLength());
	return result;
}

// Получение параметров настройки
stgParams CSettings::GetParameters() { return this->parameters; }

CString CSettings::GetPath() { return this->path; }

// Основные параметры
int CSettings::GetProject() { return this->parameters.iProject; }

int CSettings::GetMethod() { return this->parameters.iMethod; }

bool CSettings::GetNumPk() { return this->parameters.bNumPK; }

bool CSettings::GetGenTxt() { return this->parameters.bGenTxt; }

bool CSettings::GetGenComment() { return this->parameters.bComment; }
