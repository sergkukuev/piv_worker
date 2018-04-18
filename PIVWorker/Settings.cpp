#include "stdafx.h"
#include "Settings.h"

using namespace stgdll;

// �����������
CSettings::CSettings()
{
	// ������ �������� �� �������
	path = AfxGetApp()->GetProfileStringW(L"Settings", L"Path", L"");
	if (path.IsEmpty())
		path.Format(L"%s%s", GetDefaultPath(), folders[base]); // ��������� ���� �������� ����������
	CreateDirectory(path, NULL);

	parameters.iProject = AfxGetApp()->GetProfileIntW(L"Settings", L"Project", project::p930m);
	parameters.iMethod = AfxGetApp()->GetProfileIntW(L"Settings", L"Method", method::patterned);
	parameters.bNumPK = AfxGetApp()->GetProfileIntW(L"Settings", L"NumPK", false);
	parameters.bGenTxt = AfxGetApp()->GetProfileIntW(L"Settings", L"GenTXT", false);
	parameters.bComment = AfxGetApp()->GetProfileIntW(L"Settings", L"GenCOM", false);
}

// ����������
CSettings::~CSettings() 
{
	WriteRegKeys();
}

// ������ ������ � ������
void CSettings::WriteRegKeys()
{
	AfxGetApp()->WriteProfileStringW(L"Settings", L"Path", path);
	AfxGetApp()->WriteProfileInt(L"Settings", L"Project", parameters.iProject);
	AfxGetApp()->WriteProfileInt(L"Settings", L"Method", parameters.iMethod);
	AfxGetApp()->WriteProfileInt(L"Settings", L"NumPK", parameters.bNumPK);
	AfxGetApp()->WriteProfileInt(L"Settings", L"GenTXT", parameters.bGenTxt);
	AfxGetApp()->WriteProfileInt(L"Settings", L"GenCOM", parameters.bComment);
}

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
	AfxGetApp()->WriteProfileStringW(L"Settings", L"Path", path);	// ������ ���� � ������
	CreateDirectory(path, NULL);
	
}

// ���������� ����������
void CSettings::Save(const stgParams& par)
{
	parameters = par;
	WriteRegKeys();
}

// �������� ���� ����� � exe ������
CString CSettings::GetDefaultPath()
{
	CString result;
	GetModuleFileName(NULL, result.GetBuffer(_MAX_PATH), _MAX_PATH);
	result.ReleaseBuffer();
	result.Delete(result.ReverseFind(L'\\'), result.GetLength());
	return result;
}

// ��������� ���������� ���������
stgParams CSettings::GetParameters() { return this->parameters; }

CString CSettings::GetPath() { return this->path; }

// �������� ���������
int CSettings::GetProject() { return this->parameters.iProject; }

int CSettings::GetMethod() { return this->parameters.iMethod; }

bool CSettings::GetNumPk() { return this->parameters.bNumPK; }

bool CSettings::GetGenTxt() { return this->parameters.bGenTxt; }

bool CSettings::GetGenComment() { return this->parameters.bComment; }
