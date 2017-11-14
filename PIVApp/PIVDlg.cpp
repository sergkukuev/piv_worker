// PIVDlg.cpp : ���� ����������
//

#include "stdafx.h"
#include "PIVApp.h"
#include "PIVDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma region ABOUT_MODAL
// ���������� ���� CAboutDlg ������������ ��� �������� �������� � ����������
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ������ ����������� ����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // ��������� DDX/DDV

	// ����������
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)	{	}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
#pragma endregion

// ���������� ���� CPIVDlg
struct ThreadData
{
	CPIVDlg* dialog;
};

ThreadData mData;

#pragma region EXCHANGE
// �����������
DWORD WINAPI WaitThread(LPVOID lpParam) {
	CoInitialize(NULL);
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	ThreadData* data = static_cast<ThreadData*>(lpParam);
	Waiting(*(data->dialog));
	CoUninitialize();
	return 0;
}

void Waiting(CPIVDlg& app)
{
	app.ReceiveMessage();
}

void CPIVDlg::ReceiveMessage()
{
	while (1)
	{
		m_StatusBar->SetWindowTextW(piv.logger.GetStatus());
	}
}
#pragma endregion

CPIVDlg::CPIVDlg(CWnd* pParent /*=NULL*/) : CDialogEx(IDD_PIVAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPIVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPIVDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_OPEN_PROJ, &CPIVDlg::OnOpenProj)
	ON_COMMAND(ID_REFRESH_PIV, &CPIVDlg::OnRefreshPiv)
	ON_COMMAND(ID_CHANGE_FOLDER, &CPIVDlg::OnChangeFolder)
	ON_COMMAND(ID_OPEN_REPORT, &CPIVDlg::OnOpenReport)
	ON_COMMAND(ID_FOLDER_TXT, &CPIVDlg::OnFolderTxt)
	ON_COMMAND(ID_OPEN_PIV, &CPIVDlg::OnOpenPiv)
	ON_COMMAND(ID_CLOSE_PIV, &CPIVDlg::OnClosePiv)
	ON_COMMAND(ID_CLOSE_ALL, &CPIVDlg::OnCloseAll)
	ON_CBN_SELCHANGE(IDC_COMBO, &CPIVDlg::OnCbnSelchangeCombo)
	ON_BN_CLICKED(IDC_BTN_HOME, &CPIVDlg::OnBnClickedBtnHome)
	ON_BN_CLICKED(IDC_BTN_PREV, &CPIVDlg::OnBnClickedBtnPrev)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPIVDlg::OnBnClickedBtnNext)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CPIVDlg::OnBnClickedBtnRefresh)
	ON_BN_CLICKED(IDC_BTN_SETTING, &CPIVDlg::OnBnClickedSetting)
	ON_BN_CLICKED(IDC_SHOW_REP, &CPIVDlg::OnBnClickedShowRep)
END_MESSAGE_MAP()


// ����������� ��������� CPIVDlg

BOOL CPIVDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���������� ������ "� ���������..." � ��������� ����.

	// IDM_ABOUTBOX ������ ���� � �������� ��������� �������.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ������ ������ ��� ����� ����������� ����.  ����� ������ ��� �������������,
	//  ���� ������� ���� ���������� �� �������� ����������
	SetIcon(m_hIcon, TRUE);			// ������� ������
	SetIcon(m_hIcon, FALSE);		// ������ ������

	// ���������� ���� � �����-�����
	m_contextMenu.LoadMenuW(IDR_CONTEXT_MENU);
	CComboBox* m_Cmb = (CComboBox*)GetDlgItem(IDC_COMBO);
	m_Cmb->AddString(L"������");
	m_Cmb->AddString(L"��������� ���");
	m_Cmb->SetCurSel(1);

	// �������� ���� �������
	CString reports;
	GetModuleFileName(NULL, reports.GetBuffer(_MAX_PATH), _MAX_PATH);
	reports.ReleaseBuffer();
	reports.Delete(reports.ReverseFind(L'\\'), reports.GetLength());
	CEdit* m_Edt = (CEdit*)GetDlgItem(IDC_EDIT_PATH);
	piv.SetPathToSave(reports);
	m_Edt->SetWindowTextW(reports);
	
	// ��������� ������ �� ��������
	CButton* m_Btn = (CButton*)GetDlgItem(IDC_BTN_HOME);
	HICON hBmp  = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICO_HOME));
	m_Btn->SetIcon(hBmp);

	m_Btn = (CButton*)GetDlgItem(IDC_BTN_NEXT);
	hBmp = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICO_REDO));
	m_Btn->SetIcon(hBmp);

	m_Btn = (CButton*)GetDlgItem(IDC_BTN_PREV);
	hBmp = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICO_UNDO));
	m_Btn->SetIcon(hBmp);

	m_Btn = (CButton*)GetDlgItem(IDC_BTN_REFRESH);
	hBmp = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICO_REFRESH));
	m_Btn->SetIcon(hBmp);

	m_Btn = (CButton*)GetDlgItem(IDC_BTN_SETTING);
	hBmp = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICO_SETTING));
	m_Btn->SetIcon(hBmp);

	// ������������� �������� ��������� ����
	m_ListBox = (CListBox*)GetDlgItem(IDC_LISTBOX);	
	m_StatusBar = new CStatusBarCtrl();
	m_StatusBar->Create(WS_CHILD | WS_VISIBLE | LBS_MULTIPLESEL | CBRS_BOTTOM, CRect(2, 0, 280, 527), this, AFX_IDW_STATUS_BAR);

	// ������ �������� ������ ������
	CWnd* pWnd = this->GetWindow(IDR_MAINFRAME);
	this->SetWindowPos(pWnd, 0, 0, 280, 527, SWP_NOMOVE | SWP_NOZORDER);	// TODO: ������� �������� ���� ����������

	// ������������� ������ ������ ��������� ����� DLL � �����������
	mData.dialog = this;
	hWait = CreateThread(NULL, 0, WaitThread, &mData, 0, 0);

	return TRUE;  // ������� �������� TRUE, ���� ����� �� ������� �������� ����������
}

void CPIVDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��� ���������� ������ ����������� � ���������� ���� ����� ��������������� ����������� ���� �����,
//  ����� ���������� ������.  ��� ���������� MFC, ������������ ������ ���������� ��� �������������,
//  ��� ������������� ����������� ������� ��������.

void CPIVDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �������� ���������� ��� ���������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ������������ ������ �� ������ ����������� ��������������
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ��������� ������
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ������� �������� ��� ������� ��� ��������� ����������� ������� ��� �����������
//  ���������� ����.
HCURSOR CPIVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ��������� ������������ ����
void CPIVDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu* menu;	// ��������� �� �������
	CRect rect;		// ������ �������� ����������� ����
	CComboBox* m_Cmb = (CComboBox*)GetDlgItem(IDC_COMBO);
	
	m_Cmb->GetCurSel() == PROJECT ? menu = m_contextMenu.GetSubMenu(PROJECT) : menu = m_contextMenu.GetSubMenu(OTHER);	// ����� �������
	menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, point.x, point.y, this);
}

#pragma region Handlers
// ���������� �������
void CPIVDlg::OnRefreshPiv()
{
	int nCount = m_ListBox->GetSelCount();			
	CArray<int> sel;
	
	vector <CString> forRef;
	sel.SetSize(nCount);
	m_ListBox->GetSelItems(nCount, sel.GetData());

	CComboBox* m_Cmb = (CComboBox*)GetDlgItem(IDC_COMBO);
	for (int i = 0; i < sel.GetSize(); i++)
		m_Cmb->GetCurSel() == PROJECT ? forRef.push_back(pathProj[i]) : forRef.push_back(pathOther[i]);

	if (forRef.size() != 0)
		piv.Refresh(forRef);
}

// ����� ������� ����������
void CPIVDlg::OnChangeFolder()
{
	CEdit* m_Edit = (CEdit*)GetDlgItem(IDC_EDIT_PATH);
	CString folder = getFolder();
	piv.SetPathToSave(folder);
	m_Edit->SetWindowTextW(folder);
}

// ������� ����� � ��������
void CPIVDlg::OnOpenReport()
{
	CEdit* m_Edit = (CEdit*)GetDlgItem(IDC_EDIT_PATH);
	CString folder;
	m_Edit->GetWindowTextW(folder);

	CComboBox* m_Cmb = (CComboBox*)GetDlgItem(IDC_COMBO);
	m_Cmb->GetCurSel() == PROJECT ? folder.Format(L"%s\\%s\\%s\\%s", folder, BASE_FOLDER, PROJECT_FOLDER, REPORT_NAME) : 
		folder.Format(L"%s\\%s\\%s\\%s", folder, BASE_FOLDER, OTHER_FOLDER, REPORT_NAME);
	ShellExecute(0, L"Open", folder, NULL, NULL, SW_NORMAL);
}

// �������� ����� � ���������� �����������
void CPIVDlg::OnFolderTxt()
{
	CEdit* m_Edit = (CEdit*)GetDlgItem(IDC_EDIT_PATH);
	CString folder;
	m_Edit->GetWindowTextW(folder);
	folder.Format(L"%s\\%s\\%s", folder, BASE_FOLDER, TEXT_FOLDER);
	ShellExecute(0, L"Explore", folder, NULL, NULL, SW_NORMAL);
}

// �������� �������
void CPIVDlg::OnOpenProj()
{
	CString folder;
	pathProj.clear();
	OpenFile(folder, pathProj);

	piv.Open(pathProj, folder);
	
	RefreshList();
}

// �������� ��������� 
void CPIVDlg::OnOpenPiv()
{
	CString folder;
	OpenFile(folder, pathOther);
	
	piv.SetPathToSave(folder);
	piv.Open(pathOther);

	RefreshList();
}

// �������� ������ ��� ���������� ��������� ����������
void CPIVDlg::OnClosePiv()
{
	vector <CString> forDelete;
	int nCount = m_ListBox->GetSelCount();
	CArray<int> sel;
	
	sel.SetSize(nCount);
	m_ListBox->GetSelItems(nCount, sel.GetData());

	for (int i = 0; i < sel.GetSize(); i++)
	{
		forDelete.push_back(pathOther[sel[i]]);
		pathOther.erase(pathOther.begin() + sel[i]);
		m_ListBox->DeleteString(sel[i]);
	}

	piv.Close(forDelete);
}

// �������� ���� ��������� ����������
void CPIVDlg::OnCloseAll()
{
	m_ListBox->ResetContent();
	pathOther.clear();
	pathOther.shrink_to_fit();
	piv.Close();
}

// ��������� ������� ������ (������ � ��������, ������ � ���������� �������)
void CPIVDlg::OnCbnSelchangeCombo()
{
	RefreshList();
	SetNumericPIV();
	// ��������� ����� ������
}

void CPIVDlg::OnBnClickedSetting()
{
	// TODO: �������� ���������� ���� ��������
}

void CPIVDlg::OnBnClickedShowRep()
{
	// TODO: �������� ���� ��� ����������� �����������
	CWnd* pWnd = this->GetWindow(IDR_MAINFRAME);
	CButton* chk = (CButton*)GetDlgItem(IDC_SHOW_REP);
	int chkw = chk->GetCheck();
	chkw == BST_CHECKED ? this->SetWindowPos(pWnd, 0, 0, 1059, 527, SWP_NOMOVE | SWP_NOZORDER) :
		this->SetWindowPos(pWnd, 0, 0, 280, 527, SWP_NOMOVE | SWP_NOZORDER);			// TODO: ������� �������� ���� ����������
}
#pragma endregion

// ����� �����
CString CPIVDlg::getFolder()
{
	BROWSEINFO	bi;
	TCHAR	szDisplayName[MAX_PATH];
	LPITEMIDLIST	pidl;
	CString folder = L"";
	ZeroMemory(&bi, sizeof(bi));
	bi.hwndOwner = NULL;
	bi.pszDisplayName = szDisplayName;
	bi.lpszTitle = TEXT("�������� ����� ��� ���������� �������");
	bi.ulFlags = BIF_NEWDIALOGSTYLE; //or BIF_VALIDATE//BIF_RETURNONLYFSDIRS;		
	pidl = SHBrowseForFolder(&bi);

	if (pidl)
	{
		SHGetPathFromIDList(pidl, szDisplayName);
		CEdit* m_Edit = (CEdit*)(this->GetDlgItem(IDC_EDIT_PATH));
		m_Edit->SetWindowTextW(szDisplayName);
		folder = szDisplayName;
	}
	return folder;
}

// �������� ������
void CPIVDlg::OpenFile(CString& folder, vector <CString>& path)
{
	CWnd* TheWindow = GetActiveWindow();
	CFileDialog openDialog(true, NULL, NULL, OFN_ALLOWMULTISELECT + OFN_HIDEREADONLY, NULL, TheWindow);
	openDialog.m_ofn.lpstrFilter = _T("Excel files(*.xls;*.xlsx)\0*.xls;*.xlsx\0All files(*.*)\0*.*\0\0");
	openDialog.m_ofn.lpstrTitle = TEXT("�������� ��������� ��� ���������");

	CEdit* m_Edit = (CEdit *)(this->GetDlgItem(IDC_EDIT_PATH));
	m_Edit->GetWindowTextW(folder);

	if (folder.IsEmpty())
	{
		folder = getFolder();
		m_Edit->SetWindowTextW(folder);

	}

	if (openDialog.DoModal() == IDOK)
		readPath(openDialog, path);
}

// ���������� ����� ����� �� ��� ����
CString CPIVDlg::NameFromPath(const CString& path)
{
	int startPos = path.ReverseFind(L'\\') + 1;
	return path.Mid(startPos, path.GetLength());
}

// ���������� ����� ������
void CPIVDlg::readPath(const CFileDialog& dlg, vector <CString>& path)
{
	POSITION pos = dlg.GetStartPosition();

	while (pos)
	{
		bool exist = false;
		CString fullPath = dlg.GetNextPathName(pos);
		CString name = dlg.GetFileName();

		if (name == L"")
		{
			CString _path = dlg.GetPathName();
			CString tmp = fullPath;
			tmp.Delete(0, _path.GetLength() + 1);
			name = tmp;
		}

		for (size_t i = 0; i < path.size(); i++)
			if (fullPath.Compare(path[i]) == 0)
				exist = true;

		if (!exist)
			path.push_back(fullPath);
	}
}


#pragma region Browser
// TODO: �������� ���������� ����
void CPIVDlg::OnBnClickedBtnHome()
{
}


void CPIVDlg::OnBnClickedBtnPrev()
{
}


void CPIVDlg::OnBnClickedBtnNext()
{
}


void CPIVDlg::OnBnClickedBtnRefresh()
{
}
#pragma endregion

// ���������� �����
void CPIVDlg::RefreshList()
{
	m_ListBox->ResetContent();
	SetNumericPIV();
	CComboBox* m_Combo = (CComboBox*)GetDlgItem(IDC_COMBO);

	if (m_Combo->GetCurSel() == PROJECT)
		for (size_t i = 0; i < pathProj.size(); i++)
			m_ListBox->AddString(NameFromPath(pathProj[i]));
	if (m_Combo->GetCurSel() == OTHER)
		for (size_t i = 0; i < pathOther.size(); i++)
			m_ListBox->AddString(NameFromPath(pathOther[i]));
}

// ��������� ���������� ��������� ���
void CPIVDlg::SetNumericPIV()
{
	CString res;
	CComboBox* m_Combo = (CComboBox*)GetDlgItem(IDC_COMBO);

	if (m_Combo->GetCurSel() == PROJECT)
		res.Format(L"%d", pathProj.size());
	if (m_Combo->GetCurSel() == OTHER)
		res.Format(L"%d", pathOther.size());

	CStatic* m_Numeric = (CStatic*)GetDlgItem(IDC_NUM_PIV);
	m_Numeric->SetWindowTextW(res);
}

// ���������� ����
void CPIVDlg::UpdateMenu()
{
	CComboBox* m_Combo = (CComboBox*)GetDlgItem(IDC_COMBO);
	if (m_Combo->GetCurSel() == PROJECT)
	{
		CMenu* menu = m_contextMenu.GetSubMenu(0);
		if (m_ListBox->GetCount() > 0)
		{
			menu->EnableMenuItem(ID_REFRESH_PIV, MFS_ENABLED);
			menu->EnableMenuItem(ID_FOLDER_TXT, MFS_ENABLED);
			menu->EnableMenuItem(ID_OPEN_REPORT, MFS_ENABLED);
		}
		else
		{
			menu->EnableMenuItem(ID_REFRESH_PIV, MFS_GRAYED);
			menu->EnableMenuItem(ID_FOLDER_TXT, MFS_GRAYED);
			menu->EnableMenuItem(ID_OPEN_REPORT, MFS_GRAYED);
		}
	}

	if (m_Combo->GetCurSel() == OTHER)
	{
		CMenu* menu = m_contextMenu.GetSubMenu(1);
		if (m_ListBox->GetCount() > 0)
		{
			menu->EnableMenuItem(ID_CLOSE_PIV, MFS_ENABLED);
			menu->EnableMenuItem(ID_CLOSE_ALL, MFS_ENABLED);
			menu->EnableMenuItem(ID_REFRESH_PIV, MFS_ENABLED);
			menu->EnableMenuItem(ID_OPEN_REPORT, MFS_ENABLED);
			menu->EnableMenuItem(ID_FOLDER_TXT, MFS_ENABLED);
		}
		else
		{
			menu->EnableMenuItem(ID_CLOSE_PIV, MFS_GRAYED);
			menu->EnableMenuItem(ID_CLOSE_ALL, MFS_GRAYED);
			menu->EnableMenuItem(ID_REFRESH_PIV, MFS_GRAYED);
			menu->EnableMenuItem(ID_OPEN_REPORT, MFS_GRAYED);
			menu->EnableMenuItem(ID_FOLDER_TXT, MFS_GRAYED);
		}
	}
}
