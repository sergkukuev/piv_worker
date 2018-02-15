#pragma once

#include "PIVApp.h"
#include "PIVDlg.h"
#include "Settings.h"

// ���������� ���� CSettingDlg

class CSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // ����������� �����������
	virtual ~CSettingDlg();

// ������ ����������� ����
	enum { IDD = IDD_SETTINGBOX };
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // ��������� DDX/DDV

	DECLARE_MESSAGE_MAP()
private:
	pivParams* settings;
	BOOL bNumPk, bGenTxt;
	int iProject = -1, iMethod = -1;

	const enum project { p930m, kprno35 };
	const enum method { patterned, fasted };
public:
	void SetParameters();
	afx_msg void OnEnableBtnSave();
};
