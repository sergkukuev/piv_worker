
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
	CPIV piv;				// ���������� ��� ������ � �����������
	vector <CString> path;	// ���� ������
	HANDLE hWait;			// ����� ��� ����������� ������ ������ CPIVWorker
	int command = 0;		// ����� ������� ��� ����: 1 - �������, 2 - �������������, 3 - �������, 4 - ������������ �����

	void AddPath(CString fullPath, CString name, CListBox* list);		// ���������� ����� ������
	void readPath(const CFileDialog& dlg, CListBox* list);	// �������� ������ ����� �� ����������� ����
	CString getFolder();

protected:
	CStatusBarCtrl* pStatusBar;	// ������ ���
	CListBox* pList;			// ���� ���� �������
	CListBox* pListOther;		// ���� ���� ���������
	HACCEL m_AccelTable;		// ������� �������������
	HICON m_hIcon;				// ������

	// ��������� ������� ����� ���������
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);	// ��������� ������������ ����
	DECLARE_MESSAGE_MAP()
	friend void waitThread(CMainDlg& object);	// ������������� ������� ��� �������� ��������� ������

public:
	afx_msg void OnOpenProject();		// �������� �������
	afx_msg void OnPivOpen();			// �������� ����������
	afx_msg void OnPivClose();			// �������� ���������
	afx_msg void OnPivCloseAll();		// ������� ��� ���������
	afx_msg void OnPivReport();			// ������� �����
	afx_msg void OnPivRepFolder();		// ������� ����� � �������
	afx_msg void OnPivTxtOpen();		// ������� txt �����
	afx_msg void OnAppInform();			// ���������� � �������
};
