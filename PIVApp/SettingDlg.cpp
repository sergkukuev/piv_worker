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
	CPIVDlg* dlgParent = (CPIVDlg*) pParent;
	// Установка параметров
	if (dlgParent != NULL)
	{
		this->pSettings = &dlgParent->settings;
		bNumPk = pSettings->bNumPK;
		bGenTxt = pSettings->bGenTxt;
		iProject = pSettings->iProject;
		iMethod = pSettings->iMethod;
	}
	else
	{
		AfxMessageBox(L"Не удалось получить текущие настройки DLL. Установлены настройки по умолчанию.");
		bNumPk = TRUE;
		bGenTxt = FALSE;
		iProject = project::p930m;
		iMethod = method::patterned;
	}
}

CSettingDlg::~CSettingDlg() {	}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_NUMPK, bNumPk);
	DDX_Check(pDX, IDC_CHECK_TXT100, bGenTxt);
	DDX_Radio(pDX, IDC_930M, iProject);
	DDX_Radio(pDX, IDC_TEMPLATE, iMethod);
}

BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_TXT100, &CSettingDlg::OnEnableBtnSave)
	ON_BN_CLICKED(IDC_CHECK_NUMPK, &CSettingDlg::OnEnableBtnSave)
	ON_BN_CLICKED(IDC_FAST, &CSettingDlg::OnEnableBtnSave)
	ON_BN_CLICKED(IDC_TEMPLATE, &CSettingDlg::OnEnableBtnSave)
	ON_BN_CLICKED(IDC_930M, &CSettingDlg::OnEnableBtnSave)
	ON_BN_CLICKED(IDC_KPRNO35, &CSettingDlg::OnEnableBtnSave)
END_MESSAGE_MAP()


// обработчики сообщений CSettingDlg
// Инициализация
BOOL CSettingDlg::OnInitDialog()
{
	CButton *pCheck = (CButton*)this->GetDlgItem(IDC_CHECK_NUMPK);
	pCheck->SetCheck(bNumPk);
	pCheck = (CButton*)GetDlgItem(IDC_CHECK_TXT100);
	pCheck->SetCheck(bGenTxt);
	
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

	pSettings->iProject = iProject;
	pSettings->iMethod = iMethod;
}

void CSettingDlg::OnEnableBtnSave()
{
	CButton* m_btnSave = (CButton*)GetDlgItem(IDOK);
	m_btnSave->EnableWindow(TRUE);
}