#pragma once


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
	virtual void DoDataExchange(CDataExchange* pDX);    // ��������� DDX/DDV

	DECLARE_MESSAGE_MAP()
};
