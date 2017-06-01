
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
	CString folder;			// ���� ��� ������
	vector <CString> path;	// ���� ������
	HANDLE hWait;	// ����� ��� ����������� ������ ������ CPIVWorker
	int cmd = 0;	// ����� ������� ��� ����: 1 - �������, 2 - �������������, 3 - �������, 4 - ������������ �����

	CListBox* lsBox;	// ���� ����
	void setMenu(int command);	// ��������� ���������� ����
	void AddPath(CString fullPath, CString name);	// ���������� ����� ������

protected:
	HACCEL m_AccelTable;		// ������� �������������
	HICON m_hIcon;				// ������

	// ��������� ������� ����� ���������
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	friend void waitThread(CMainDlg& object);	// ������������� ������� ��� �������� ��������� ������

public:
	void logicMenu(int command);		// ������ ������ ����
	afx_msg void OnPivOpen();			// �������� ����������
	afx_msg void OnPivClose();			// �������� ���������
	afx_msg void OnPivCloseAll();		// ������� ��� ���������
	afx_msg void OnPivAnalyze();		// ������������� ���������
	afx_msg void OnPivReport();			// ������� �����
	afx_msg void OnPivRepFolder();		// ������� ����� � �������
	afx_msg void OnPivTxtGenerate();	// ������������ txt �����
	afx_msg void OnAppInform();			// ���������� � �������
};
