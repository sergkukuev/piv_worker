// SettingDlg.cpp: файл реализации
//

#include "stdafx.h"
#include "SettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// диалоговое окно CSettingDlg

IMPLEMENT_DYNAMIC(CSettingDlg, CDialog)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent)
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif	
	CPIVWorkerDlg* dlgParent = (CPIVWorkerDlg*) pParent;
	// Установка параметров
	if (dlgParent != NULL)
	{
		this->pSettings = &dlgParent->settings;
		bNumPk = pSettings->bNumPK;
		bGenTxt = pSettings->bGenTxt;
		bComment = pSettings->bComment;
		iProject = pSettings->iProject;
		iMethod = pSettings->iMethod;
	}
	else
	{
		AfxMessageBox(L"Не удалось получить текущие настройки DLL. Установлены настройки по умолчанию.");
		bNumPk = FALSE;
		bGenTxt = FALSE;
		bComment = FALSE;
		iProject = project::p930m;
		iMethod = method::patterned;
	}
}

CSettingDlg::~CSettingDlg() {	}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_NUMPK, bNumPk);
	DDX_Check(pDX, IDC_CHECK_PARAM, bGenTxt);
	DDX_Check(pDX, IDC_CHECK_COMMENT, bComment);
	DDX_Radio(pDX, IDC_930M, iProject);
	DDX_Radio(pDX, IDC_TEMPLATE, iMethod);
}

BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_PARAM, &CSettingDlg::OnEnableBtnSave)
	ON_BN_CLICKED(IDC_CHECK_NUMPK, &CSettingDlg::OnEnableBtnSave)
	ON_BN_CLICKED(IDC_CHECK_COMMENT, &CSettingDlg::OnEnableBtnSave)
	ON_BN_CLICKED(IDC_FAST, &CSettingDlg::OnEnableBtnSave)
	ON_BN_CLICKED(IDC_TEMPLATE, &CSettingDlg::OnEnableBtnSave)
	ON_BN_CLICKED(IDC_930M, &CSettingDlg::OnEnableBtnSave)
	ON_BN_CLICKED(IDC_KPRNO35, &CSettingDlg::OnEnableBtnSave)
	ON_BN_CLICKED(IDC_OPEN_LOG, &CSettingDlg::OnBnClickedOpenLog)
END_MESSAGE_MAP()


// обработчики сообщений CSettingDlg
// Инициализация
BOOL CSettingDlg::OnInitDialog()
{
	CButton *pCheck = (CButton*)this->GetDlgItem(IDC_CHECK_NUMPK);
	pCheck->SetCheck(bNumPk);
	pCheck = (CButton*)GetDlgItem(IDC_CHECK_PARAM);
	pCheck->SetCheck(bGenTxt);
	pCheck = (CButton*)GetDlgItem(IDC_CHECK_COMMENT);
	pCheck->SetCheck(bComment);
	
	if (iProject == project::p930m)
		pCheck = (CButton*)this->GetDlgItem(IDC_930M);
	else if (iProject == project::kprno35)
		pCheck = (CButton*)this->GetDlgItem(IDC_KPRNO35);
		pCheck->SetCheck(TRUE);

	if (iMethod == method::fasted)
		pCheck = (CButton*)this->GetDlgItem(IDC_FAST);
	else if (iMethod == method::patterned)
		pCheck = (CButton*)this->GetDlgItem(IDC_TEMPLATE);
		pCheck->SetCheck(TRUE);

	return TRUE;
}

void CSettingDlg::SetParameters()
{
	pSettings->bNumPK = (bool)bNumPk;
	pSettings->bGenTxt = (bool)bGenTxt;
	pSettings->bComment = (bool)bComment;

	pSettings->iProject = iProject;
	pSettings->iMethod = iMethod;
}

void CSettingDlg::OnEnableBtnSave()
{
	CButton* m_btnSave = (CButton*)GetDlgItem(IDOK);
	m_btnSave->EnableWindow(TRUE);
}

void CSettingDlg::OnBnClickedOpenLog()
{
	stgdll::CSettings &stg = stgdll::CSettings::Instance();
	CString path = stg.GetDefaultPath();
	path.Format(L"%s\\%s", path, logdll::lgFolder);
	ShellExecute(0, L"Explore", path, NULL, NULL, SW_NORMAL);
}
