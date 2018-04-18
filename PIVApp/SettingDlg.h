#pragma once

#include "PIVApp.h"
#include "PIVDlg.h"
#include "PIVWorker.h"

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
	stgParams* pSettings;
	BOOL bNumPk, bGenTxt, bComment;
	int iProject = -1, iMethod = -1;
public:
	void SetParameters();
	afx_msg void OnEnableBtnSave();
	afx_msg void OnBnClickedOpenLog();
};
