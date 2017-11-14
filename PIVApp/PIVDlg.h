
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
	CMenu m_contextMenu;		// ����������� ����
	CListBox* m_ListBox;		// ������ �������� ����������
	CStatusBarCtrl* m_StatusBar;
	HICON m_hIcon;
	HANDLE hWait;

	// ��������� ������� ����� ���������
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	// �����������
	friend void Waiting(CPIVDlg& app);
	void ReceiveMessage();

	DECLARE_MESSAGE_MAP()
private:
	CPIV piv;	// ����� ��� ������ � DLL �� ��������� ����������
	std::vector <CString> pathProj;		// ���� ������ �������
	std::vector <CString> pathOther;	// ���� ��������� ������

	CString getFolder();	// ����� �����
	void OpenFile(CString& folder, vector <CString>& path);			// �������� ������
	void readPath(const CFileDialog& dlg, vector <CString>& path);	// ���������� ����� ������
	CString NameFromPath(const CString& path); // ��� ����� �� ��� ����

	// �������������� � ���������� �����
	void RefreshList();		// ���������� ListBox
	void SetNumericPIV();	// ��������� ���������� ��������� ���
	void UpdateMenu();		// ���������� ����
public:
	// ����������� �������
	afx_msg void OnOpenProj();			// �������� �������
	afx_msg void OnRefreshPiv();		// ���������� ��������� ������
	afx_msg void OnChangeFolder();		// ����� ����������
	afx_msg void OnOpenReport();		// ������� ����� � ����
	afx_msg void OnFolderTxt();			// ������� ����� � ��� �������
	afx_msg void OnOpenPiv();			// ������� �������� ���
	afx_msg void OnClosePiv();			// ������� ��������� ���
	afx_msg void OnCloseAll();			// ������� ���
	afx_msg void OnCbnSelchangeCombo();	// ����� ������ ������ (������, ��������� ���)
	afx_msg void OnBnClickedSetting();	// ������� ���� ���������
	afx_msg void OnBnClickedShowRep();	// ����������� ������

	//	������ � ���������
	afx_msg void OnBnClickedBtnHome();
	afx_msg void OnBnClickedBtnPrev();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedBtnRefresh();
};
