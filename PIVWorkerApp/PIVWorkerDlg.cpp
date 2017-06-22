
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
	
	object.logicMenu();
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
	ON_COMMAND(ID_PIV_ANALYZE, &CMainDlg::OnPivAnalyze)
	ON_COMMAND(ID_PIV_REPORT, &CMainDlg::OnPivReport)
	ON_COMMAND(ID_PIV_REP_FOLDER, &CMainDlg::OnPivRepFolder)
	ON_COMMAND(ID_PIV_TXT_GENERATE, &CMainDlg::OnPivTxtGenerate)
	ON_COMMAND(ID_APP_INFORM, &CMainDlg::OnAppInform)
	ON_COMMAND(ID_PIV_CLOSE_ALL, &CMainDlg::OnPivCloseAll)
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
	lsBox = new CListBox();
	lsBox->Create(WS_CHILD | WS_VISIBLE | LBS_MULTIPLESEL, CRect(10, 30, 295, 300), this, 0x118);
	lsBox->GetItemHeight(20);

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

// �������� ����������
void CMainDlg::OnPivOpen()
{
	CWnd* TheWindow = GetActiveWindow();
	CFileDialog openDialog(true, NULL, NULL, OFN_ALLOWMULTISELECT + OFN_HIDEREADONLY, NULL, TheWindow);

	openDialog.m_ofn.lpstrFilter = _T("Excel files(*.xls;*.xlsx)\0*.xls;*.xlsx\0All files(*.*)\0*.*\0\0");
	openDialog.m_ofn.lpstrTitle = TEXT("�������� ��������� ��� �������");

	if (openDialog.DoModal() == IDOK)
	{
		POSITION ps = openDialog.GetStartPosition();	// �������� ��������� �������
		while (ps) //���� ���� ��������� �����
		{
			CString fullPath = openDialog.GetNextPathName(ps);
			CString name = openDialog.GetFileName();
			
			if (name == L"")
			{
				CString _path = openDialog.GetPathName();
				CString tmp = fullPath;
				tmp.Delete(0, _path.GetLength() + 1);
				name = tmp;
			}

			AddPath(fullPath, name);	// ���������� �����
		}
	}

	if (path.empty())
		AfxMessageBox(_T("������ ���������� ��� �������� ����!"));
	else
	{
		piv.ReadExcel(path);
		setMenu(1);
	}
}

// �������� ���������
void CMainDlg::OnPivClose()
{
	/*vector <CString> del;	// ���� ���� ��������� ���
	int nCount = lsBox->GetSelCount();	// �������� ������� ���� ��������� ���������.
	CArray<int, int> sel;

	sel.SetSize(nCount);
	lsBox->GetSelItems(nCount, sel.GetData());

	for (size_t i = 0; i < sel.GetSize(); i++)
	{
		del.push_back(path[i]);
		path.erase(path.begin() + i);
		lsBox->DeleteString(i);
	}
	piv.CloseExcel(del);
	setMenu(3);*/
}

// ������������� ���������
void CMainDlg::OnPivAnalyze()
{
	//piv.TestExcel();
	//setMenu(2);
}

// ������� �����
void CMainDlg::OnPivReport()
{
	/*BROWSEINFO	bi;
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
		folder = szDisplayName;
	}

	piv.Report(folder);	// ��������� ������
	setMenu(4);*/
}

// ������� ����� � �������
void CMainDlg::OnPivRepFolder()
{
	//ShellExecute(0, L"Explore", folder, NULL, NULL, SW_NORMAL);
}

// ������������ txt �����
void CMainDlg::OnPivTxtGenerate()
{
	/*BROWSEINFO	bi;
	TCHAR	szDisplayName[MAX_PATH];
	LPITEMIDLIST	pidl;

	ZeroMemory(&bi, sizeof(bi));
	bi.hwndOwner = NULL;
	bi.pszDisplayName = szDisplayName;
	bi.lpszTitle = TEXT("�������� ����� ��� ���������� txt ������");
	bi.ulFlags = BIF_NEWDIALOGSTYLE; //or BIF_VALIDATE//BIF_RETURNONLYFSDIRS;
	pidl = SHBrowseForFolder(&bi);

	if (pidl)
	{
		SHGetPathFromIDList(pidl, szDisplayName);
		folder = szDisplayName;
	}

	CButton *pCheck = (CButton*)GetDlgItem(IDC_CHECK_NUMPK);

	if (pCheck == BST_UNCHECKED)
		piv.setStatusNumPK(false);
	else
		piv.setStatusNumPK(true);

	piv.CreateTxt(folder);*/
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
	/*vector <CString> tmp = path;
	path.clear();
	lsBox->ResetContent();
	piv.CloseExcel(tmp);
	setMenu(3);*/
}

#pragma endregion

// ��������� ����� ������
void CMainDlg::AddPath(CString fullPath, CString name)
{
	bool result = true;

	if (!path.empty())
		for (size_t i = 0; i < path.size(); i++)
			if (fullPath.Compare(path[i]) == 0)
				result = false;
	
	if (result)
	{
		path.push_back(fullPath);
		lsBox->AddString(name);
	}
}

// ������ ������ ���� ����������
void CMainDlg::logicMenu()
{
	CMenu* pMenu = GetMenu();

	if (command == 1)	// ������� ������� ��������
	{
		pMenu->EnableMenuItem(ID_PIV_ANALYZE, MFS_ENABLED);
		pMenu->EnableMenuItem(ID_PIV_CLOSE, MFS_ENABLED);
		pMenu->EnableMenuItem(ID_PIV_CLOSE_ALL, MFS_ENABLED);
	}
	else if (command == 2)	// ������� �������������
	{
		pMenu->EnableMenuItem(ID_PIV_REPORT, MFS_ENABLED);
		pMenu->EnableMenuItem(ID_PIV_TXT_GENERATE, MFS_ENABLED);
	}
	else if (command == 3)	// ������� �������� / ������� ���
	{
		if (path.empty())
		{
			pMenu->EnableMenuItem(ID_PIV_ANALYZE, MFS_GRAYED);
			pMenu->EnableMenuItem(ID_PIV_CLOSE, MFS_GRAYED);
			pMenu->EnableMenuItem(ID_PIV_CLOSE_ALL, MFS_GRAYED);

			pMenu->EnableMenuItem(ID_PIV_REPORT, MFS_GRAYED);
			pMenu->EnableMenuItem(ID_PIV_REP_FOLDER, MFS_GRAYED);
			pMenu->EnableMenuItem(ID_PIV_TXT_GENERATE, MFS_GRAYED);
		}
	}
	else if (command == 4)	// ������� �����
	{
		CString file = folder;
		file.Format(_T("%s\\�����.html"), folder);
		pMenu->EnableMenuItem(ID_PIV_REP_FOLDER, MFS_ENABLED);
	}
	else
		AfxMessageBox(L"�������������� �������!");
}

// ��������� ���������� ����
void CMainDlg::setMenu(int com)
{
	command = com;
	mD.object = this;
	hWait = CreateThread(NULL, 0, CheckThread, &mD, 0, 0);
}