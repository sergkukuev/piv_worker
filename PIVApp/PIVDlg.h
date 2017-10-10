
// PIVDlg.h : ���� ���������
//

#pragma once
#include "afxwin.h"
#include <vector>
#include "PIVWorker.h"

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
	CListBox* m_ListBox;	// ������ �������� ����������
	HICON m_hIcon;

	// ��������� ������� ����� ���������
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	void menuLogic();
	DECLARE_MESSAGE_MAP()

private:
	std::vector <CString> pathProj;
	std::vector <CString> pathOther;

	CPIV piv;

	CString getFolder();	// ����� �����
public:
	DECLARE_EVENTSINK_MAP()
	void OnChangeTbtnReport();		// ���������� �� ������� ���� ��������
	afx_msg void OnOpenProj();		// �������� �������
	afx_msg void OnRefreshPiv();	// ���������� ��������� ������
	afx_msg void OnChangeFolder();	// ����� ����������
	afx_msg void OnOpenReport();	// ������� ����� � ����
	afx_msg void OnFolderTxt();		// ������� ����� � ��� �������
	afx_msg void OnOpenPiv();		// ������� �������� ���
	afx_msg void OnClosePiv();		// ������� ��������� ���
	afx_msg void OnCloseAll();		// ������� ���
	afx_msg void OnCloseProj();		// �������� ������	std::vector <CString> pathOther;
	afx_msg void OnCbnSelchangeCombo();
	afx_msg void OnBnClickedBtnHome();
	afx_msg void OnBnClickedBtnPrev();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedBtnRefresh();
};
