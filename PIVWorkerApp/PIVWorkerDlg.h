
// PIVWorkerDlg.h : ���� ���������
//

#pragma once

#include "PIVWorker.h"

// ���������� ���� CMainDlg
class CMainDlg : public CDialog
{
// ��������
public:
	CMainDlg(CWnd* pParent = NULL);	// ����������� �����������

// ������ ����������� ����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PIVWORKERAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// ��������� DDX/DDV


// ����������
private:
	CPIVWorker piv;			// ���������� ��� ������ � �����������
	vector <CString> path;	// ���� ������

protected:
	HACCEL m_AccelTable;		// ������� �������������
	HICON m_hIcon;				// ������

	// ��������� ������� ����� ���������
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPivOpen();			// �������� ����������
	afx_msg void OnPivClose();			// �������� ���������
	afx_msg void OnPivCloseAll();		// ������� ��� ���������
	afx_msg void OnPivAnalyze();		// ������������� ���������
	afx_msg void OnPivReport();			// ������� �����
	afx_msg void OnPivRepFolder();		// ������� ����� � �������
	afx_msg void OnPivTxtGenerate();	// ������������ txt �����
	afx_msg void OnAppInform();			// ���������� � �������
};
