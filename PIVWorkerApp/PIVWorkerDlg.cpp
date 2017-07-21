
// PIVWorkerDlg.cpp : ���� ����������
//

#include "stdafx.h"
#include "PIVWorkerApp.h"
#include "PIVWorkerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���������� ���� CAboutDlg ������������ ��� �������� �������� � ����������

class CAboutDlg : public CDialogEx {
public:
	CAboutDlg();

// ������ ����������� ����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // ��������� DDX/DDV

// ����������
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {	}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/) : CDialog(IDD_PIVWORKERAPP_DIALOG, pParent) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMainDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	
	ON_COMMAND(ID_PIV_OPEN, &CMainDlg::OnPivOpen)
	ON_COMMAND(ID_OPEN_PROJECT, &CMainDlg::OnOpenProject)
	ON_COMMAND(ID_PIV_CLOSE, &CMainDlg::OnPivClose)
	ON_COMMAND(ID_PIV_CLOSE_ALL, &CMainDlg::OnPivCloseAll)
	ON_COMMAND(ID_PIV_REFRESH, &CMainDlg::OnPivRefresh)
	
	ON_COMMAND(ID_SET_FOLDER, &CMainDlg::OnPivSetFolder)
	ON_COMMAND(ID_PROJECT_REPORT, &CMainDlg::OnProjectReport)
	ON_COMMAND(ID_REP_OPEN, &CMainDlg::OnOtherReport)
	ON_COMMAND(ID_REP_FOLDER, &CMainDlg::OnPivRepFolder)
	ON_COMMAND(ID_TXT_OPEN, &CMainDlg::OnPivTxtOpen)

	ON_COMMAND(ID_APP_INFORM, &CMainDlg::OnAppInform)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// ����������� ��������� CMainDlg

BOOL CMainDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	// ���������� ������ "� ���������..." � ��������� ����.

	// IDM_ABOUTBOX ������ ���� � �������� ��������� �������.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL) {
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty()) {
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �������� � ��������� ListBox
	//pLsBox = new CListBox();
	//pLsBox->Create(WS_CHILD | WS_VISIBLE | LBS_MULTIPLESEL, CRect(10, 30, 300, 300), this, 0x118);
	//pLsBox->GetItemHeight(20);

	//pLsBoxOther = new CListBox();
	//pLsBoxOther->Create(WS_CHILD | WS_VISIBLE | LBS_MULTIPLESEL, CRect(310, 330, 300, 300), this, 0x118);
	//pLsBoxOther->GetItemHeight(20);
	pList = (CListBox *)this->GetDlgItem(IDC_LIST_PROJECT);
	pListOther = (CListBox *)this->GetDlgItem(IDC_LIST_OTHER);

	// �������� � ��������� ������ ���������
	pStatusBar = new CStatusBarCtrl();
	pStatusBar->Create(WS_CHILD | WS_VISIBLE | LBS_MULTIPLESEL | CBRS_BOTTOM, CRect(2, 0, 576, 388), this, AFX_IDW_STATUS_BAR);

	m_subMenu.LoadMenuW(IDR_CONTEXT_MENU);

	// ������ ������ ��� ����� ����������� ����.  ����� ������ ��� �������������,
	//  ���� ������� ���� ���������� �� �������� ����������
	SetIcon(m_hIcon, TRUE);			// ������� ������
	SetIcon(m_hIcon, FALSE);		// ������ ������

	// TODO: �������� �������������� �������������

	m_AccelTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));

	return TRUE;  // ������� �������� TRUE, ���� ����� �� ������� �������� ����������
}

void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam) {
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else {
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ��� ���������� ������ ����������� � ���������� ���� ����� ��������������� ����������� ���� �����,
//  ����� ���������� ������.  ��� ���������� MFC, ������������ ������ ���������� ��� �������������,
//  ��� ������������� ����������� ������� ��������.

void CMainDlg::OnPaint() {
	if (IsIconic()) {
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
	else {
		CDialog::OnPaint();
	}
}

// ������� �������� ��� ������� ��� ��������� ����������� ������� ��� �����������
//  ���������� ����.
HCURSOR CMainDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}

#pragma region MenuButton

// �������� �������
void CMainDlg::OnOpenProject()  {
	CString folder;
	pProj.clear();
	OpenFile(pList, folder, pProj);
	pProj.empty() ? AfxMessageBox(L"��������� �� �������!", MB_ICONWARNING) : piv.Open(pProj, folder);
	logicMenu();
}

// �������� ����������
void CMainDlg::OnPivOpen() {
	CString folder;
	OpenFile(pListOther, folder, pOther);
	piv.setPathToSave(folder);
	pOther.empty() ? AfxMessageBox(L"��������� �� �������!", MB_ICONWARNING) : piv.Add(pOther);
	logicMenu();
}

// ���������� ����������
void CMainDlg::OnPivRefresh() {
	CPoint point;
	GetCursorPos(&point);

	CRect rect;		// ������ �������� ����������� ����
	this->GetWindowRect(&rect);

	CPoint pointWnd;	// ���������� ������������ ����
	pointWnd.x = point.x - rect.left;	// ����� ����: 298
	pointWnd.y = point.y - rect.top;	// ������� ������ ���� �� Y: �� 80 �� 340

	vector <CString> path;
	if (pointWnd.x < CENTER)
		getFileForRefresh(pList, pProj, path);
	else
		getFileForRefresh(pListOther, pOther, path);
	path.empty() ? AfxMessageBox(L"��������� ��� ���������� �� �������!") : piv.Refresh(path);
}

// �������� ���������
void CMainDlg::OnPivClose() {
	vector <CString> del;					// ���� ���� ��������� ���
	int nCount = pListOther->GetSelCount();	// ���������� ���������� ���������.
	CArray<int> sel;

	sel.SetSize(nCount);
	pListOther->GetSelItems(nCount, sel.GetData());

	for (size_t i = 0; i < sel.GetSize(); i++) {	
		del.push_back(pOther[sel[i]]);
		pOther.erase(pOther.begin() + sel[i]);
		pListOther->DeleteString(sel[i]);
	}
	(del.empty()) ? AfxMessageBox(L"����� ��� �������� �� �������!") : piv.Close(del);
	logicMenu();
}

//���������� ���������� ��� ������� � txt
void CMainDlg::OnPivSetFolder() {
	CEdit* edt = (CEdit *)(this->GetDlgItem(IDC_PATH));
	CString folder;
	folder = getFolder();
	piv.setPathToSave(folder);
	edt->SetWindowTextW(folder);
}

// ������� ����� �� ���������
void CMainDlg::OnOtherReport() {
	CEdit* edit = (CEdit *)this->GetDlgItem(IDC_PATH);
	CString pathFile;
	edit->GetWindowTextW(pathFile);
	pathFile.Format(L"%s\\Artefacts\\Other\\�����.html", pathFile);
	ShellExecute(0, L"Open", pathFile, NULL, NULL, SW_NORMAL);
}

// ������� ����� �� �������
void CMainDlg::OnProjectReport() {
	CEdit* edit = (CEdit *)this->GetDlgItem(IDC_PATH);
	CString pathFile;
	edit->GetWindowTextW(pathFile);
	pathFile.Format(L"%s\\Artefacts\\Project\\�����.html", pathFile);
	ShellExecute(0, L"Open", pathFile, NULL, NULL, SW_NORMAL);
}

// ������� ����� � �������
void CMainDlg::OnPivRepFolder() {
	CEdit* edit = (CEdit *)this->GetDlgItem(IDC_PATH);
	CString folder;
	edit->GetWindowTextW(folder);
	folder.Format(L"%s\\Artefacts", folder);
	ShellExecute(0, L"Explore", folder, NULL, NULL, SW_NORMAL);
}

// ������� txt �����
void CMainDlg::OnPivTxtOpen() {
	CEdit* edit = (CEdit *)this->GetDlgItem(IDC_PATH);
	CString folder;
	edit->GetWindowTextW(folder);
	folder.Format(L"%s\\%s\\Text", folder, L"Artefacts");
	ShellExecute(0, L"Explore", folder, NULL, NULL, SW_NORMAL);
}

// ���������� � ����������
void CMainDlg::OnAppInform() {
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// ������� ��� ���������
void CMainDlg::OnPivCloseAll() {
	pListOther->ResetContent();
	pOther.clear();
	pOther.shrink_to_fit();
	piv.Close();
}

#pragma endregion

// �������� ������
void CMainDlg::OpenFile(CListBox* list, CString& folder, vector <CString>& path) {
	CWnd* TheWindow = GetActiveWindow();
	CFileDialog openDialog(true, NULL, NULL, OFN_ALLOWMULTISELECT + OFN_HIDEREADONLY, NULL, TheWindow);

	openDialog.m_ofn.lpstrFilter = _T("Excel files(*.xls;*.xlsx)\0*.xls;*.xlsx\0All files(*.*)\0*.*\0\0");
	openDialog.m_ofn.lpstrTitle = TEXT("�������� ��� ��������� �������");

	CEdit* edt = (CEdit *)(this->GetDlgItem(IDC_PATH));
	edt->GetWindowTextW(folder);

	if (folder.IsEmpty()) {
		folder = getFolder();
		edt->SetWindowTextW(folder);
	}

	CButton *pCheck = (CButton*)GetDlgItem(IDC_CHECK_NUMPK);
	(pCheck == BST_UNCHECKED) ? piv.setStatusNumPK(false) : piv.setStatusNumPK(true);

	if (openDialog.DoModal() == IDOK)
		readPath(openDialog, list, path);
}

// �������� ���� ����� ��� ����������
CString CMainDlg::getFolder() {
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

	if (pidl) {
		SHGetPathFromIDList(pidl, szDisplayName);
		CEdit* edt = (CEdit *)(this->GetDlgItem(IDC_PATH));
		edt->SetWindowTextW(szDisplayName);
		folder = szDisplayName;
	}
	
	return folder;
}

// ��������� ������ ����� �� ����������� ����
void CMainDlg::readPath(const CFileDialog& dlg, CListBox* list, vector <CString>& path) {
	POSITION pos = dlg.GetStartPosition();

	if (list == pList)
		list->ResetContent();

	while (pos) {
		CString fullPath = dlg.GetNextPathName(pos);
		CString name = dlg.GetFileName();

		if (name == L"") {
			CString _path = dlg.GetPathName();
			CString tmp = fullPath;
			tmp.Delete(0, _path.GetLength() + 1);
			name = tmp;
		}
		AddPath(fullPath, name, list, path);	// ���������� �����
	}
}

// ��������� ����� ������
bool CMainDlg::AddPath(const CString& fullPath, const CString& name, CListBox* list, vector <CString>& path) {
	bool result = true;

	for (size_t i = 0; i < path.size(); i++)
		if (fullPath.Compare(path[i]) == 0)
			result = false;
	
	if (result) {
		path.push_back(fullPath);
		list->AddString(name);
	}
	return result;
}

// ������ ������ ��� ����������
void CMainDlg::getFileForRefresh(CListBox* list, const vector <CString>& from, vector <CString>& path) {
	int nCount = list->GetSelCount();	// ���������� ���������� ���������.
	CArray<int> sel;

	sel.SetSize(nCount);
	list->GetSelItems(nCount, sel.GetData());

	for (size_t i = 0; i < sel.GetSize(); i++)
		path.push_back(from[sel[i]]);
}

// ��������� ������������ ����
void CMainDlg::OnContextMenu(CWnd* pWnd, CPoint point) {
	CMenu* menu;	// ��������� �� �������
	CRect rect;		// ������ �������� ����������� ����
	this->GetWindowRect(&rect);	

	CPoint pointWnd;	// ���������� ������������ ����
	pointWnd.x = point.x - rect.left;	// ����� ����: 298
	pointWnd.y = point.y - rect.top;	// ������� ������ ���� �� Y: �� 80 �� 340

	pointWnd.x < CENTER ? menu = m_subMenu.GetSubMenu(0) : menu = m_subMenu.GetSubMenu(1); // ����� �������

	if (pointWnd.y > 80 && pointWnd.y < 340)
		menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, point.x, point.y, this);
}

// ����������� ���� � �������
void CMainDlg::logicMenu() {
	CMenu* m_Menu = GetMenu();
	CMenu* menu;

	// ��������� ������ ����
	if (pList->GetCount() > 0 || pListOther->GetCount() > 0) {
		m_Menu->EnableMenuItem(ID_REP_FOLDER, MFS_ENABLED);
		m_Menu->EnableMenuItem(ID_TXT_OPEN, MFS_ENABLED);
	}
	else {
		m_Menu->EnableMenuItem(ID_REP_FOLDER, MFS_GRAYED);
		m_Menu->EnableMenuItem(ID_TXT_OPEN, MFS_GRAYED);
	}

	// ��������� ������� ��� "��� �������"
	menu = m_subMenu.GetSubMenu(0);
	if (pList->GetCount() > 0) {
		menu->EnableMenuItem(ID_PIV_REFRESH, MFS_ENABLED);
		menu->EnableMenuItem(ID_TXT_OPEN, MFS_ENABLED);
		menu->EnableMenuItem(ID_PROJECT_REPORT, MFS_ENABLED);
	}
	else {
		menu->EnableMenuItem(ID_PIV_REFRESH, MFS_GRAYED);
		menu->EnableMenuItem(ID_TXT_OPEN, MFS_GRAYED);
		menu->EnableMenuItem(ID_PROJECT_REPORT, MFS_GRAYED);
	}

	// ��������� ������� ��� "��������� ���"
	menu = m_subMenu.GetSubMenu(1);
	if (pListOther->GetCount() > 0) {
		m_Menu->EnableMenuItem(ID_PIV_CLOSE, MFS_ENABLED);
		m_Menu->EnableMenuItem(ID_PIV_CLOSE_ALL, MFS_ENABLED);
		menu->EnableMenuItem(ID_PIV_CLOSE, MFS_ENABLED);
		menu->EnableMenuItem(ID_PIV_CLOSE_ALL, MFS_ENABLED);
		menu->EnableMenuItem(ID_PIV_REFRESH, MFS_ENABLED);
		menu->EnableMenuItem(ID_REP_OPEN, MFS_ENABLED);
		menu->EnableMenuItem(ID_TXT_OPEN, MFS_ENABLED);
	}
	else {
		m_Menu->EnableMenuItem(ID_PIV_CLOSE, MFS_GRAYED);
		m_Menu->EnableMenuItem(ID_PIV_CLOSE_ALL, MFS_GRAYED);
		menu->EnableMenuItem(ID_PIV_CLOSE, MFS_GRAYED);
		menu->EnableMenuItem(ID_PIV_CLOSE_ALL, MFS_GRAYED);
		menu->EnableMenuItem(ID_PIV_REFRESH, MFS_GRAYED);
		menu->EnableMenuItem(ID_REP_OPEN, MFS_GRAYED);
		menu->EnableMenuItem(ID_TXT_OPEN, MFS_GRAYED);
	}
}
