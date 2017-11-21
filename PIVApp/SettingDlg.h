#pragma once


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
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
};
