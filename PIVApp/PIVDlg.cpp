// PIVDlg.cpp : файл реализации
//

#include "stdafx.h"
#include "PIVApp.h"
#include "PIVDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma region ABOUT_MODAL
// Диалоговое окно CAboutDlg используется для описания сведений о приложении
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	// Реализация
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

// Диалоговое окно CPIVDlg
struct ThreadData
{
	CPIVDlg* dialog;
};

ThreadData mData;

#pragma region EXCHANGE
// Логирование
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


// Обработчики сообщений CPIVDlg

BOOL CPIVDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
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

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// Добавление меню и комбо-бокса
	m_contextMenu.LoadMenuW(IDR_CONTEXT_MENU);
	CComboBox* m_Cmb = (CComboBox*)GetDlgItem(IDC_COMBO);
	m_Cmb->AddString(L"Проект");
	m_Cmb->AddString(L"Отдельные ПИВ");
	m_Cmb->SetCurSel(1);

	// Добавить путь отчетов
	CString reports;
	GetModuleFileName(NULL, reports.GetBuffer(_MAX_PATH), _MAX_PATH);
	reports.ReleaseBuffer();
	reports.Delete(reports.ReverseFind(L'\\'), reports.GetLength());
	CEdit* m_Edt = (CEdit*)GetDlgItem(IDC_EDIT_PATH);
	piv.SetPathToSave(reports);
	m_Edt->SetWindowTextW(reports);
	
	// Установка иконок на кнопочки
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

	// Инициализация основных элементов окна
	m_ListBox = (CListBox*)GetDlgItem(IDC_LISTBOX);	
	m_StatusBar = new CStatusBarCtrl();
	m_StatusBar->Create(WS_CHILD | WS_VISIBLE | LBS_MULTIPLESEL | CBRS_BOTTOM, CRect(2, 0, 280, 527), this, AFX_IDW_STATUS_BAR);

	// Скрыть окошечко показа отчета
	CWnd* pWnd = this->GetWindow(IDR_MAINFRAME);
	this->SetWindowPos(pWnd, 0, 0, 280, 527, SWP_NOMOVE | SWP_NOZORDER);	// TODO: Выявить значение окна программно

	// Инициализация потока обмена сообщений между DLL и приложением
	mData.dialog = this;
	hWait = CreateThread(NULL, 0, WaitThread, &mData, 0, 0);

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
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

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CPIVDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CPIVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Обработка контекстного меню
void CPIVDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu* menu;	// Указатель на подменю
	CRect rect;		// Объект текущего диалогового окна
	CComboBox* m_Cmb = (CComboBox*)GetDlgItem(IDC_COMBO);
	
	m_Cmb->GetCurSel() == PROJECT ? menu = m_contextMenu.GetSubMenu(PROJECT) : menu = m_contextMenu.GetSubMenu(OTHER);	// Выбор подменю
	menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, point.x, point.y, this);
}

#pragma region Handlers
// Обновление отчетов
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

// Смена рабочей директории
void CPIVDlg::OnChangeFolder()
{
	CEdit* m_Edit = (CEdit*)GetDlgItem(IDC_EDIT_PATH);
	CString folder = getFolder();
	piv.SetPathToSave(folder);
	m_Edit->SetWindowTextW(folder);
}

// Открыть отчет в браузере
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

// Открытие папки с текстовыми документами
void CPIVDlg::OnFolderTxt()
{
	CEdit* m_Edit = (CEdit*)GetDlgItem(IDC_EDIT_PATH);
	CString folder;
	m_Edit->GetWindowTextW(folder);
	folder.Format(L"%s\\%s\\%s", folder, BASE_FOLDER, TEXT_FOLDER);
	ShellExecute(0, L"Explore", folder, NULL, NULL, SW_NORMAL);
}

// Открытие проекта
void CPIVDlg::OnOpenProj()
{
	CString folder;
	pathProj.clear();
	OpenFile(folder, pathProj);

	piv.Open(pathProj, folder);
	
	RefreshList();
}

// Открытие остальных 
void CPIVDlg::OnOpenPiv()
{
	CString folder;
	OpenFile(folder, pathOther);
	
	piv.SetPathToSave(folder);
	piv.Open(pathOther);

	RefreshList();
}

// Закрытие одного или нескольких выбранных протоколов
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

// Закрытие всех отдельных протоколов
void CPIVDlg::OnCloseAll()
{
	m_ListBox->ResetContent();
	pathOther.clear();
	pathOther.shrink_to_fit();
	piv.Close();
}

// Изменение области работы (работа с проектом, работа с отдельными файлами)
void CPIVDlg::OnCbnSelchangeCombo()
{
	RefreshList();
	SetNumericPIV();
	// Привязать смену отчета
}

void CPIVDlg::OnBnClickedSetting()
{
	// TODO: Запилить диалоговое окно настроек
}

void CPIVDlg::OnBnClickedShowRep()
{
	// TODO: добавьте свой код обработчика уведомлений
	CWnd* pWnd = this->GetWindow(IDR_MAINFRAME);
	CButton* chk = (CButton*)GetDlgItem(IDC_SHOW_REP);
	int chkw = chk->GetCheck();
	chkw == BST_CHECKED ? this->SetWindowPos(pWnd, 0, 0, 1059, 527, SWP_NOMOVE | SWP_NOZORDER) :
		this->SetWindowPos(pWnd, 0, 0, 280, 527, SWP_NOMOVE | SWP_NOZORDER);			// TODO: Выявить значение окна программно
}
#pragma endregion

// Выбор папки
CString CPIVDlg::getFolder()
{
	BROWSEINFO	bi;
	TCHAR	szDisplayName[MAX_PATH];
	LPITEMIDLIST	pidl;
	CString folder = L"";
	ZeroMemory(&bi, sizeof(bi));
	bi.hwndOwner = NULL;
	bi.pszDisplayName = szDisplayName;
	bi.lpszTitle = TEXT("Выберите папку для сохранения отчетов");
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

// Открытие файлов
void CPIVDlg::OpenFile(CString& folder, vector <CString>& path)
{
	CWnd* TheWindow = GetActiveWindow();
	CFileDialog openDialog(true, NULL, NULL, OFN_ALLOWMULTISELECT + OFN_HIDEREADONLY, NULL, TheWindow);
	openDialog.m_ofn.lpstrFilter = _T("Excel files(*.xls;*.xlsx)\0*.xls;*.xlsx\0All files(*.*)\0*.*\0\0");
	openDialog.m_ofn.lpstrTitle = TEXT("Выберите протоколы для обработки");

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

// Извлечение имени файла из его пути
CString CPIVDlg::NameFromPath(const CString& path)
{
	int startPos = path.ReverseFind(L'\\') + 1;
	return path.Mid(startPos, path.GetLength());
}

// Считывание путей файлов
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
// TODO: Запилить браузерное окно
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

// Обновление листа
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

// Установка количество считанных ПИВ
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

// Обновление меню
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
