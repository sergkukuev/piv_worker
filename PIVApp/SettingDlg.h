#pragma once

#include "PIVApp.h"
#include "PIVDlg.h"
#include "PIVWorker.h"	// TODO: Разобраться с ошибками С2143, С4430, С2238

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
	pivParam* param;
	BOOL bNumPk, bGenTxt;
	int iProject = -1, iMethod = -1;

	const enum project {p930m, kprno35};
	const enum method {regex, fast};
public:
	void SetParameters();	// Возврат значений настроек
	afx_msg void OnEnableBtnSave();
};
