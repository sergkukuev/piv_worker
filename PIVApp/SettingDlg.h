#pragma once

#include "PIVApp.h"
#include "RegWinMsg.h"
#include "Settings.h"

// диалоговое окно CSettingDlg

class CSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // стандартный конструктор
	virtual ~CSettingDlg();

// Данные диалогового окна
	enum { IDD = IDD_SETTINGBOX };
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
private:
	BOOL bNumPk, bGenTxt;
	int iProject = -1, iMethod = -1;

	const enum project { p930m, kprno35 };
	const enum method { patterned, fasted };
public:
	afx_msg void OnEnableBtnSave();
};
