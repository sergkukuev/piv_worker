
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
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// ���������� ���� CMainDlg
struct MyData
{
	CMainDlg* object;
};

MyData mD;

// ����� ��� �������� ����������� ������ ������ CPIVWorker
DWORD WINAPI CheckThread(LPVOID lpParam)
{
	CoInitialize(NULL);
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	MyData* myD = static_cast<MyData*>(lpParam);

	waitThread(*(myD->object));

	CoUninitialize();

	return 0;
}

// �������� ����������� ������ ������ CPIVWorker
void waitThread(CMainDlg& object)
{
	WaitForSingleObject(object.piv.primary, INFINITE);
}

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PIVWORKERAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_PIV_OPEN, &CMainDlg::OnPivOpen)
	ON_COMMAND(ID_PIV_CLOSE, &CMainDlg::OnPivClose)
	ON_COMMAND(ID_PIV_CLOSE_ALL, &CMainDlg::OnPivCloseAll)
	ON_COMMAND(ID_REP_OPEN, &CMainDlg::OnPivReport)
	ON_COMMAND(ID_REP_FOLDER, &CMainDlg::OnPivRepFolder)
	ON_COMMAND(ID_TXT_OPEN, &CMainDlg::OnPivTxtOpen)
	ON_COMMAND(ID_OPEN_PROJECT, &CMainDlg::OnOpenProject)
	ON_COMMAND(ID_APP_INFORM, &CMainDlg::OnAppInform)
END_MESSAGE_MAP()


// ����������� ��������� CMainDlg

BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	// ������ ������ ��� ����� ����������� ����.  ����� ������ ��� �������������,
	//  ���� ������� ���� ���������� �� �������� ����������
	SetIcon(m_hIcon, TRUE);			// ������� ������
	SetIcon(m_hIcon, FALSE);		// ������ ������

	// TODO: �������� �������������� �������������

	m_AccelTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));

	return TRUE;  // ������� �������� TRUE, ���� ����� �� ������� �������� ����������
}

void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ��� ���������� ������ ����������� � ���������� ���� ����� ��������������� ����������� ���� �����,
//  ����� ���������� ������.  ��� ���������� MFC, ������������ ������ ���������� ��� �������������,
//  ��� ������������� ����������� ������� ��������.

void CMainDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// ������� �������� ��� ������� ��� ��������� ����������� ������� ��� �����������
//  ���������� ����.
HCURSOR CMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#pragma region MenuButton

// �������� �������
void CMainDlg::OnOpenProject() 
{
	CWnd* TheWindow = GetActiveWindow();
	CFileDialog openDialog(true, NULL, NULL, OFN_ALLOWMULTISELECT + OFN_HIDEREADONLY, NULL, TheWindow);

	openDialog.m_ofn.lpstrFilter = _T("Excel files(*.xls;*.xlsx)\0*.xls;*.xlsx\0All files(*.*)\0*.*\0\0");
	openDialog.m_ofn.lpstrTitle = TEXT("�������� ��� ��������� �������");

	CEdit* edt = (CEdit *)(this->GetDlgItem(IDC_PATH));
	CString folder;
	edt->GetWindowTextW(folder);

	if (folder.IsEmpty())
		folder = getFolder();

	CButton *pCheck = (CButton*)GetDlgItem(IDC_CHECK_NUMPK);
	(pCheck == BST_UNCHECKED) ? piv.setStatusNumPK(false) : piv.setStatusNumPK(true);

	if (openDialog.DoModal() == IDOK)
		readPath(openDialog, pList);

	path.empty() ? AfxMessageBox(L"��������� �� �������!") : piv.Open(path, folder);
}
// �������� ����������
void CMainDlg::OnPivOpen()
{
	CWnd* TheWindow = GetActiveWindow();
	CFileDialog openDialog(true, NULL, NULL, OFN_ALLOWMULTISELECT + OFN_HIDEREADONLY, NULL, TheWindow);

	openDialog.m_ofn.lpstrFilter = _T("Excel files(*.xls;*.xlsx)\0*.xls;*.xlsx\0All files(*.*)\0*.*\0\0");
	openDialog.m_ofn.lpstrTitle = TEXT("�������� ��� ��������� �������");

	CButton *pCheck = (CButton*)GetDlgItem(IDC_CHECK_NUMPK);
	(pCheck == BST_UNCHECKED) ? piv.setStatusNumPK(false) : piv.setStatusNumPK(true);

	if (openDialog.DoModal() == IDOK)
		readPath(openDialog, pListOther);

	path.empty() ? AfxMessageBox(L"��������� �� �������!") : piv.Add(path);
}

// �������� ���������
void CMainDlg::OnPivClose()
{
	vector <CString> del;	// ���� ���� ��������� ���
	int nCount = pList->GetSelCount();	// �������� ������� ���� ��������� ���������.
	CArray<int> sel;

	sel.SetSize(nCount);
	pList->GetSelItems(nCount, sel.GetData());

	for (size_t i = 0; i < sel.GetSize(); i++)
	{	
		del.push_back(path[sel[i]]);
		path.erase(path.begin() + sel[i]);
		pList->DeleteString(sel[i]);
	}
	(del.empty()) ? AfxMessageBox(L"����� ��� �������� �� �������!") : piv.Close(del);
}

// ������� �����
void CMainDlg::OnPivReport()
{
	BROWSEINFO	bi;
	TCHAR	szDisplayName[MAX_PATH];
	LPITEMIDLIST	pidl;

	ZeroMemory(&bi, sizeof(bi));
	bi.hwndOwner = NULL;
	bi.pszDisplayName = szDisplayName;
	bi.lpszTitle = TEXT("�������� ����� ��� ���������� ������");
	bi.ulFlags = BIF_NEWDIALOGSTYLE; //or BIF_VALIDATE//BIF_RETURNONLYFSDIRS;
	pidl = SHBrowseForFolder(&bi);

	if (pidl)
	{
		SHGetPathFromIDList(pidl, szDisplayName);
		CEdit* edit = (CEdit *)this->GetDlgItem(IDC_PATH);
		edit->SetWindowTextW(szDisplayName);
	}

	piv.setPathToSave(szDisplayName);
}

// ������� ����� � �������
void CMainDlg::OnPivRepFolder()
{
	CEdit* edit = (CEdit *)this->GetDlgItem(IDC_PATH);
	CString folder;
	edit->GetWindowTextW(folder);
	ShellExecute(0, L"Explore", folder, NULL, NULL, SW_NORMAL);
}

// ������� txt �����
void CMainDlg::OnPivTxtOpen()
{
	CEdit* edit = (CEdit *)this->GetDlgItem(IDC_PATH);
	CString folder;
	edit->GetWindowTextW(folder);
	ShellExecute(0, L"Explore", folder, NULL, NULL, SW_NORMAL);

	folder.Format(L"%s\\Text", folder);

	//CButton *pCheck = (CButton*)GetDlgItem(IDC_CHECK_NUMPK);
	//(pCheck == BST_UNCHECKED) ? piv.setStatusNumPK(false) : piv.setStatusNumPK(true);
}

// ���������� � ����������
void CMainDlg::OnAppInform()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// ������� ��� ���������
void CMainDlg::OnPivCloseAll()
{
	pList->ResetContent();
	piv.Close();
}

#pragma endregion

// �������� ���� ����� ��� ����������
CString CMainDlg::getFolder() {
	BROWSEINFO	bi;
	TCHAR	szDisplayName[MAX_PATH];
	LPITEMIDLIST	pidl;

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
	}

	return szDisplayName;
}

// ��������� ������ ����� �� ����������� ����
void CMainDlg::readPath(const CFileDialog& dlg, CListBox* list) {
	POSITION pos = dlg.GetStartPosition();
	vector <CString> result;

	if (list == pList)
		list->ResetContent();

	while (pos) {
		CString fullPath = dlg.GetNextPathName(pos);
		CString name = dlg.GetFileName();

		if (name == L"")
		{
			CString _path = dlg.GetPathName();
			CString tmp = fullPath;
			tmp.Delete(0, _path.GetLength() + 1);
			name = tmp;
		}
		AddPath(fullPath, name, list);	// ���������� �����
	}
}

// ��������� ����� ������
void CMainDlg::AddPath(CString fullPath, CString name, CListBox* list)
{
	bool result = true;

	if (!path.empty())
		for (size_t i = 0; i < path.size(); i++)
			if (fullPath.Compare(path[i]) == 0)
				result = false;
	
	if (result)
	{
		path.push_back(fullPath);
		list->AddString(name);
	}
}