// PIVDlg.cpp : файл реализации
//

#include "stdafx.h"
#include "PIVDlg.h"

#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Диалоговое окно CPIVDlg
//#define RECT_MOVE 779

struct ThreadData
{
	CPIVDlg* dlg;
	HWND hWnd;
};

static UINT BASED_CODE indicators[] =
{
	ID_INDICATOR_DLL,
	//ID_INDICATOR_MOUSE
};

const UINT WM_EXCHANGE_DLL = ::RegisterWindowMessage(_T("WM_EXCHANGE_DLL"));	// Событие обмена сообщением

ThreadData mData;


#pragma region EXCHANGE
// Логирование
DWORD WINAPI WaitThread(LPVOID lpParam) {
	CoInitialize(NULL);
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	ThreadData* data = static_cast<ThreadData*>(lpParam);
	Waiting(*data->dlg, reinterpret_cast<HWND>(data->hWnd));
	CoUninitialize();
	return 0;
}

void Waiting(CPIVDlg& dlg, HWND hWnd)
{
	ASSERT(hWnd != NULL);
	dlg.ReceiveMessage(hWnd);
}

void CPIVDlg::ReceiveMessage(HWND hWnd)
{
	while (1)
	{
		CString temp = piv.GetStatus();

		if (temp.Compare(logdll::stThread[logdll::end]) == 0)
			break;

		::SendMessage(hWnd, WM_EXCHANGE_DLL, 0, reinterpret_cast<LPARAM>(&temp));

		if (!piv.IsUpdate())
		{
			RefreshList();
			UpdateMenu();
		}
		Sleep(200);
	}
}

// Обновление статус-бара
LRESULT CPIVDlg::OnReceiveMessage(WPARAM, LPARAM lParam)
{
	CString* pString = reinterpret_cast<CString*>(lParam);
	ASSERT(pString != NULL);
	m_StatusBar.SetPaneText(0, *pString);
	return 0;
}
#pragma endregion

CPIVDlg::CPIVDlg(CWnd* pParent /*=NULL*/) : CDialogEx(IDD_PIVAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPIVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER, browser);
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
	ON_COMMAND(ID_OPEN_FOLDER, &CPIVDlg::OnOpenFolder)
	ON_COMMAND(ID_OPEN_PIV, &CPIVDlg::OnOpenPiv)
	ON_COMMAND(ID_CLOSE_PIV, &CPIVDlg::OnClosePiv)
	ON_COMMAND(ID_CLOSE_ALL, &CPIVDlg::OnCloseAll)
	ON_CBN_SELCHANGE(IDC_CMB_MODE, &CPIVDlg::OnCbnSelchangeCombo)
	ON_BN_CLICKED(IDC_BTN_HOME, &CPIVDlg::OnBnClickedBtnHome)
	ON_BN_CLICKED(IDC_BTN_PREV, &CPIVDlg::OnBnClickedBtnPrev)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPIVDlg::OnBnClickedBtnNext)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CPIVDlg::OnBnClickedBtnRefresh)
	ON_BN_CLICKED(IDC_BTN_SETTING, &CPIVDlg::OnBnClickedSetting)
	ON_BN_CLICKED(IDC_SHOW_REP, &CPIVDlg::OnBnClickedShowRep)
	ON_REGISTERED_MESSAGE(WM_EXCHANGE_DLL, &CPIVDlg::OnReceiveMessage)
	//ON_WM_MOUSEMOVE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// Обработчики сообщений CPIVDlg
BOOL CPIVDlg::OnInitDialog()
{
	//_crtBreakAlloc = /*587*/879627;
	CDialogEx::OnInitDialog();
	// TODO: Добавление пункта "О программе..." в системное меню.
	// IDM_ABOUTBOX должен быть в пределах системной команды.
	/*ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
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
	}*/
		
	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// Добавление меню и комбо-бокса
	m_contextMenu.LoadMenuW(IDR_CONTEXT_MENU);
	CComboBox* m_Cmb = (CComboBox*)GetDlgItem(IDC_CMB_MODE);
	m_Cmb->AddString(L"Проект");
	m_Cmb->AddString(L"Отдельные ПИВ");
	m_Cmb->SetCurSel(1);

	// Добавить путь отчетов
	CEdit* m_Edt = (CEdit*)GetDlgItem(IDC_EDIT_PATH);
	m_Edt->SetWindowTextW(piv.GetPath());
	
	// Установка иконок на кнопочки
	CButton* m_Btn = (CButton*)GetDlgItem(IDC_BTN_HOME);
	HICON hBmp  = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_HOME));
	m_Btn->SetIcon(hBmp);

	m_Btn = (CButton*)GetDlgItem(IDC_BTN_NEXT);
	hBmp = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_REDO));
	m_Btn->SetIcon(hBmp);

	m_Btn = (CButton*)GetDlgItem(IDC_BTN_PREV);
	hBmp = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_UNDO));
	m_Btn->SetIcon(hBmp);

	m_Btn = (CButton*)GetDlgItem(IDC_BTN_REFRESH);
	hBmp = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_REFRESH));
	m_Btn->SetIcon(hBmp);

	m_Btn = (CButton*)GetDlgItem(IDC_BTN_SETTING);
	hBmp = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SETTING));
	m_Btn->SetIcon(hBmp);

	m_ListBox = (CListBox*)GetDlgItem(IDC_LISTBOX);	

	CRect table_rect;
	CWnd *pReport = GetDlgItem(IDC_REPORT_TABLE);
	pReport->GetWindowRect(&table_rect);
	this->ScreenToClient(&table_rect);

	// Скрыть окошечко показа отчета
	CRect rect;
	this->GetWindowRect(&rect);
	CWnd* pWnd = this->GetWindow(IDR_MAINFRAME);
	this->SetWindowPos(pWnd, 0, 0, rect.Width() - table_rect.Width() - 9, rect.Height(), SWP_NOMOVE | SWP_NOZORDER);

	// Инициализация индикатора
	GetClientRect(&rect);
	m_StatusBar.Create(this);
	m_StatusBar.SetIndicators(indicators, 1);
	GetClientRect(&rect);
	m_StatusBar.SetPaneInfo(0, ID_INDICATOR_DLL, SBPS_NORMAL, rect.Width());
	//TODO: Решить проблему медленного обновления курсора мышки в статус баре
	//m_StatusBar.SetPaneInfo(1, ID_INDICATOR_MOUSE, SBPS_NORMAL, 0);	
	m_StatusBar.GetStatusBarCtrl().SetBkColor(RGB(180, 180, 180));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_INDICATOR_DLL);

	BrowserNavigate();

	// Инициализация потока обмена сообщений между DLL и приложением
	mData.dlg = this;
	mData.hWnd = GetSafeHwnd();
	hWait = CreateThread(NULL, 0, WaitThread, &mData, 0, 0);

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CPIVDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		// TODO: Тут было окошко информации 
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
	CComboBox* m_Cmb = (CComboBox*)GetDlgItem(IDC_CMB_MODE);
	
	m_Cmb->GetCurSel() == PROJECT ? menu = m_contextMenu.GetSubMenu(PROJECT) : menu = m_contextMenu.GetSubMenu(OTHER);	// Выбор подменю
	menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, point.x, point.y, this);
}

// Обработка движения мыши
/*void CPIVDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CString move;
	move.Format(L"X=%d; Y=%d", point.x, point.y);
	m_StatusBar.SetPaneText(1, move);
}*/

#pragma region Handlers
// Открытие проекта
void CPIVDlg::OnOpenProj()
{
	vector <CString> path;
	OpenFile(path);

	if (!path.empty())
	{
		pathProj.clear();
		pathProj.shrink_to_fit();
		pathProj = path;
		piv.Open(pathProj);
	}
	else
		piv.WriteLog(L"Протоколы для открытия не выбраны");
}

// Открытие остальных 
void CPIVDlg::OnOpenPiv()
{
	vector <CString> path;
	OpenFile(path);

	if (!path.empty())
	{
		pathOther.reserve(pathOther.size() + path.size());
		pathOther.insert(pathOther.end(), path.begin(), path.end());
		piv.Add(path);
	}
	else
		piv.WriteLog(L"Протоколы для открытия не выбраны");
}

// Обновление протоколов
void CPIVDlg::OnRefreshPiv()
{
	int nCount = m_ListBox->GetSelCount();			
	CArray<int> sel;
	vector <CString> forRef;
	CComboBox* m_Cmb = (CComboBox*)GetDlgItem(IDC_CMB_MODE);
	
	sel.SetSize(nCount);
	m_ListBox->GetSelItems(nCount, sel.GetData());
	for (int i = 0; i < sel.GetSize(); i++)
		m_Cmb->GetCurSel() == PROJECT ? forRef.push_back(pathProj[i]) : forRef.push_back(pathOther[i]);

	!forRef.empty() ? piv.Refresh(forRef) : piv.WriteLog(L"Протоколы для обновления не выбраны");
}

// Смена рабочей директории
void CPIVDlg::OnChangeFolder()
{
	CEdit* m_Edit = (CEdit*)GetDlgItem(IDC_EDIT_PATH);
	CString folder = GetFolder();

	if (!folder.IsEmpty())
	{
		piv.SetPathToSave(folder);
		m_Edit->SetWindowTextW(piv.GetPath());
	}
	else
		piv.WriteLog(L"Папка для сохранения артефактов не изменена");
}

// Открыть отчет в браузере
void CPIVDlg::OnOpenReport()
{
	CComboBox* m_Cmb = (CComboBox*)GetDlgItem(IDC_CMB_MODE);
	CString path;
	if (m_Cmb->GetCurSel() == PROJECT)
		path = piv.GetProjectPath();
	if (m_Cmb->GetCurSel() == OTHER)
		path = piv.GetOtherPath() + L"\\" + REPORT_NAME;
	ShellExecute(0, L"Open", path, NULL, NULL, SW_NORMAL);
}

// Открытие папки с артефактами
void CPIVDlg::OnOpenFolder()
{
	CString path = piv.GetPath();
	ShellExecute(0, L"Explore", piv.GetPath(), NULL, NULL, SW_NORMAL);
}

// Закрытие одного или нескольких выбранных протоколов
void CPIVDlg::OnClosePiv()
{
	vector <CString> forDel;
	int nCount = m_ListBox->GetSelCount();
	CArray<int> sel;
	
	sel.SetSize(nCount);
	m_ListBox->GetSelItems(nCount, sel.GetData());
	for (int i = 0; i < sel.GetSize(); i++)
	{
		forDel.push_back(pathOther[sel[i]]);
		pathOther.erase(pathOther.begin() + sel[i]);
	}

	!forDel.empty() ? piv.Close(forDel) : piv.WriteLog(L"Протоколы для закрытия не выбраны");
}

// Закрытие всех протоколов
void CPIVDlg::OnCloseAll()
{
	pathOther.clear();
	pathOther.shrink_to_fit();
	piv.Close();
}

// Изменение области работы (работа с проектом, работа с отдельными файлами)
void CPIVDlg::OnCbnSelchangeCombo()
{
	RefreshList();
	SetNumericPIV();
	BrowserNavigate();
}

// Показывать отчет рядом со списком открытых протоколов или нет
void CPIVDlg::OnBnClickedShowRep()
{
	CWnd* pWnd = this->GetWindow(IDR_MAINFRAME);
	CButton* chk = (CButton*)GetDlgItem(IDC_SHOW_REP);
	CRect rect;

	CRect table_rect;
	CWnd *pReport = GetDlgItem(IDC_REPORT_TABLE);
	pReport->GetWindowRect(&table_rect);
	this->ScreenToClient(&table_rect);

	this->GetWindowRect(&rect);
	chk->GetCheck() == BST_CHECKED ? this->SetWindowPos(pWnd, 0, 0, rect.Width() + 9 + table_rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOZORDER) :
		this->SetWindowPos(pWnd, 0, 0, rect.Width() - table_rect.Width() - 9, rect.Height(), SWP_NOMOVE | SWP_NOZORDER);
	GetClientRect(&rect);
	m_StatusBar.SetPaneInfo(0, ID_INDICATOR_DLL, SBPS_NORMAL, rect.Width());
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_INDICATOR_DLL);
}
#pragma endregion

#pragma region SUB_FUNC
// Открытие файлов
void  CPIVDlg::OpenFile(vector <CString>& path)
{
	CWnd* TheWindow = GetActiveWindow();
	CFileDialog openDialog(true, NULL, NULL, OFN_ALLOWMULTISELECT + OFN_HIDEREADONLY, NULL, TheWindow);
	openDialog.m_ofn.lpstrFilter = _T("Excel files(*.xls;*.xlsx;*.xlsb)\0*.xls;*.xlsx;*.xlsb\0All files(*.*)\0*.*\0\0");
	openDialog.m_ofn.lpstrTitle = TEXT("Выберите протоколы для обработки");

	CString folder;
	this->GetDlgItem(IDC_EDIT_PATH)->GetWindowTextW(folder);

	if (folder.IsEmpty())
	{
		folder = GetFolder();
		this->GetDlgItem(IDC_EDIT_PATH)->SetWindowTextW(folder);
	}

	if (openDialog.DoModal() == IDOK)
		ReadPath(openDialog, path);
}

// Выбор папки
CString CPIVDlg::GetFolder()
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

// Считывание путей файлов
void CPIVDlg::ReadPath(const CFileDialog& dlg, vector <CString>& path)
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

// Извлечение имени файла из его пути
CString CPIVDlg::NameFromPath(const CString& path)
{
	int startPos = path.ReverseFind(L'\\') + 1;
	CString result = path.Mid(startPos, path.GetLength());
	startPos = result.ReverseFind(L'.');
	result.Delete(startPos, result.GetLength() - startPos);
	return result;
}
#pragma endregion

#pragma region Browser
void CPIVDlg::OnBnClickedBtnHome()
{
	BrowserNavigate();
}


void CPIVDlg::OnBnClickedBtnPrev()
{
	browser.GoBack();
}


void CPIVDlg::OnBnClickedBtnNext()
{
	browser.GoForward();
}


void CPIVDlg::OnBnClickedBtnRefresh()
{
	browser.Refresh();
}

void CPIVDlg::BrowserNavigate()
{
	CComboBox* m_Cmb = (CComboBox*)GetDlgItem(IDC_CMB_MODE);
	CString path;
	if (m_Cmb->GetCurSel() == PROJECT)
		path.Format(L"%s\\%s", piv.GetProjectPath(), REPORT_NAME);
	if (m_Cmb->GetCurSel() == OTHER)
		path.Format(L"%s\\%s", piv.GetOtherPath(), REPORT_NAME);

	browser.Navigate(path, NULL, NULL, NULL, NULL);
}
#pragma endregion

#pragma region Settings
// Открыть окно настроек
void CPIVDlg::OnBnClickedSetting()
{
	settings = piv.GetSettings();
	CSettingDlg stgDlg(this);

	if (stgDlg.DoModal() == IDOK)
	{
		stgDlg.SetParameters();
		piv.SetSettings(settings);
	}
}
#pragma endregion

// Обновление листа
void CPIVDlg::RefreshList()
{
	m_ListBox->ResetContent();
	SetNumericPIV();
	CComboBox* m_Combo = (CComboBox*)GetDlgItem(IDC_CMB_MODE);

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
	CComboBox* m_Combo = (CComboBox*)GetDlgItem(IDC_CMB_MODE);

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
	CComboBox* m_Combo = (CComboBox*)GetDlgItem(IDC_CMB_MODE);
	if (m_Combo->GetCurSel() == PROJECT)
	{
		CMenu* menu = m_contextMenu.GetSubMenu(0);
		if (m_ListBox->GetCount() > 0)
		{
			//OnBnClickedBtnRefresh();
			menu->EnableMenuItem(ID_REFRESH_PIV, MFS_ENABLED);
			menu->EnableMenuItem(ID_OPEN_REPORT, MFS_ENABLED);
		}
		else
		{
			menu->EnableMenuItem(ID_REFRESH_PIV, MFS_GRAYED);
			menu->EnableMenuItem(ID_OPEN_FOLDER, MFS_GRAYED);
		}
	}

	if (m_Combo->GetCurSel() == OTHER)
	{
		CMenu* menu = m_contextMenu.GetSubMenu(1);
		if (m_ListBox->GetCount() > 0)
		{
			//OnBnClickedBtnRefresh();
			menu->EnableMenuItem(ID_CLOSE_PIV, MFS_ENABLED);
			menu->EnableMenuItem(ID_CLOSE_ALL, MFS_ENABLED);
			menu->EnableMenuItem(ID_REFRESH_PIV, MFS_ENABLED);
			menu->EnableMenuItem(ID_OPEN_REPORT, MFS_ENABLED);
		}
		else
		{
			menu->EnableMenuItem(ID_CLOSE_PIV, MFS_GRAYED);
			menu->EnableMenuItem(ID_CLOSE_ALL, MFS_GRAYED);
			menu->EnableMenuItem(ID_REFRESH_PIV, MFS_GRAYED);
			menu->EnableMenuItem(ID_OPEN_REPORT, MFS_GRAYED);
		}
	}
}

// Закрытие окна
void CPIVDlg::OnClose()
{
	piv.WriteLog(logdll::stThread[logdll::end]);
	WaitForSingleObject(hWait, INFINITE);

	hWait != 0 ? CloseHandle(hWait) : AfxMessageBox(L"Поток логирования закрылся где-то раньше");
	CDialogEx::OnClose();
}
