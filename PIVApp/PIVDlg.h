
// PIVDlg.h : ���� ���������
//

#pragma once
#include "afxwin.h"

// �������� �����-����� ��� ����������� ������ ������������ ����
#define PROJECT 0
#define OTHER 1

// ���������� ���� CPIVDlg
class CPIVDlg : public CDialogEx
{
// ��������
public:
	CPIVDlg(CWnd* pParent = NULL);	// ����������� �����������

// ������ ����������� ����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PIVAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// ��������� DDX/DDV


// ����������
protected:
	bool SHOW_REPORT = true;	// ����������� ������
	CMenu m_contextMenu;	// ����������� ����
	HICON m_hIcon;

	// ��������� ������� ����� ���������
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void OnChangeTbtnReport();	// ���������� �� ������� ���� ��������
	CButton m_BtnHome;
};
