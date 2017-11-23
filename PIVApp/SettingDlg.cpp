// SettingDlg.cpp: файл реализации
//

#include "stdafx.h"
#include "SettingDlg.h"

// диалоговое окно CSettingDlg

IMPLEMENT_DYNAMIC(CSettingDlg, CDialog)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent)
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif
	CPIVDlg* pDlg = (CPIVDlg*)pParent;
	ASSERT(pDlg != NULL);
	
	// Установка параметра
	param = &pDlg->settings;
	bNumPk = (BOOL)param->bNumPK;
	bGenTxt = (BOOL)param->bGenTxt;
	param->bProject ? iProject = p930m : iProject = kprno35;
	param->bMethod ? iMethod = regex : iMethod = fast;
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
	
	if (iProject == p930m)
		pCheck = (CButton*)this->GetDlgItem(IDC_930M);
	else if (iProject == kprno35)
		pCheck = (CButton*)this->GetDlgItem(IDC_KPRNO35);
		pCheck->SetCheck(TRUE);

	if (iMethod == fast)
		pCheck = (CButton*)this->GetDlgItem(IDC_FAST);
	else if (iMethod == regex)
		pCheck = (CButton*)this->GetDlgItem(IDC_TEMPLATE);
		pCheck->SetCheck(TRUE);

	return TRUE;
}

// Возврат значений настроек
void CSettingDlg::SetParameters()
{
	param->bNumPK = (bool)bNumPk;
	param->bGenTxt = (bool)bGenTxt;

	if (iProject == p930m)
		param->bProject = true;
	else if (iProject == kprno35)
		param->bProject = false;

	if (iMethod == fast)
		param->bMethod = false;
	else if (iProject == regex)
		param->bMethod = true;
}

void CSettingDlg::OnEnableBtnSave()
{
	CButton* m_btnSave = (CButton*)GetDlgItem(IDOK);
	m_btnSave->EnableWindow(TRUE);
}