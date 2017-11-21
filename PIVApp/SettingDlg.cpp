// SettingDlg.cpp: файл реализации
//

#include "stdafx.h"
#include "PIVApp.h"
#include "SettingDlg.h"


// диалоговое окно CSettingDlg

IMPLEMENT_DYNAMIC(CSettingDlg, CDialog)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent)
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
END_MESSAGE_MAP()


// обработчики сообщений CSettingDlg
