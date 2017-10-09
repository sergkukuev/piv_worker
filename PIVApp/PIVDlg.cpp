
// PIVDlg.cpp : файл реализации
//

#include "stdafx.h"
#include "PIVApp.h"
#include "PIVDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
protected:
	DECLARE_MESSAGE_MAP()
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


// диалоговое окно CPIVDlg

CPIVDlg::CPIVDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PIVAPP_DIALOG, pParent)
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
	ON_COMMAND(ID_CLOSE_PROJ, &CPIVDlg::OnCloseProj)
	ON_CBN_SELCHANGE(IDC_COMBO, &CPIVDlg::OnCbnSelchangeCombo)
	ON_BN_CLICKED(IDC_BTN_HOME, &CPIVDlg::OnBnClickedBtnHome)
	ON_BN_CLICKED(IDC_BTN_PREV, &CPIVDlg::OnBnClickedBtnPrev)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPIVDlg::OnBnClickedBtnNext)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CPIVDlg::OnBnClickedBtnRefresh)
END_MESSAGE_MAP()


// обработчики сообщений CPIVDlg

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
	m_Edt->SetWindowTextW(reports);
	CreateDirectory(reports, NULL);

	// Инициализация листа и меню
	m_ListBox = (CListBox*)GetDlgItem(IDC_LISTBOX);
	menuLogic();

	// Скрыть окошечко показа отчета
	CWnd* pWnd = this->GetWindow(IDR_MAINFRAME);
	this->SetWindowPos(pWnd, 0, 0, 280, 527, SWP_NOMOVE | SWP_NOZORDER);

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

BEGIN_EVENTSINK_MAP(CPIVDlg, CDialogEx)
	ON_EVENT(CPIVDlg, IDC_TBTN_REPORT, 2, CPIVDlg::OnChangeTbtnReport, VTS_NONE)
END_EVENTSINK_MAP()

// Событие изменение нажатия кнопки отображения отчета
void CPIVDlg::OnChangeTbtnReport()
{
	CWnd* pWnd = this->GetWindow(IDR_MAINFRAME);
	SHOW_REPORT ? this->SetWindowPos(pWnd, 0, 0, 1059, 527, SWP_NOMOVE | SWP_NOZORDER) :
		this->SetWindowPos(pWnd, 0, 0, 280, 527, SWP_NOMOVE | SWP_NOZORDER);			// ВЫЯВИТЬ РАЗМЕРЫ ОКНА ПРОГРАММНО, А НЕ ПОДГОНЯТЬ
	// Изменить статус показа
	SHOW_REPORT ? SHOW_REPORT = false : SHOW_REPORT = true;
}

void CPIVDlg::OnOpenProj()
{
	// TODO: добавьте свой код обработчика команд
}


void CPIVDlg::OnRefreshPiv()
{
	// TODO: добавьте свой код обработчика команд
}


void CPIVDlg::OnChangeFolder()
{
	CEdit* m_Edit = (CEdit*)GetDlgItem(IDC_EDIT_PATH);
	CString folder = getFolder();
	//piv.setPathToSave(folder);
	m_Edit->SetWindowTextW(folder);
}


void CPIVDlg::OnOpenReport()
{
	// TODO: добавьте свой код обработчика команд
}


void CPIVDlg::OnFolderTxt()
{
	// TODO: добавьте свой код обработчика команд
}


void CPIVDlg::OnOpenPiv()
{
	// TODO: добавьте свой код обработчика команд
}


void CPIVDlg::OnClosePiv()
{
	// TODO: добавьте свой код обработчика команд
}


void CPIVDlg::OnCloseAll()
{
	// TODO: добавьте свой код обработчика команд
}


void CPIVDlg::OnCloseProj()
{
	// TODO: добавьте свой код обработчика команд
}

#pragma endregion

void CPIVDlg::menuLogic()
{
	CMenu* menu = m_contextMenu.GetSubMenu(0);
	if (m_ListBox->GetCount() > 0) 
	{
		menu->EnableMenuItem(ID_REFRESH_PIV, MFS_ENABLED);
		menu->EnableMenuItem(ID_FOLDER_TXT, MFS_ENABLED);
		menu->EnableMenuItem(ID_OPEN_REPORT, MFS_ENABLED);
		menu->EnableMenuItem(ID_CLOSE_PROJ, MFS_ENABLED);
	}
	else 
	{
		menu->EnableMenuItem(ID_REFRESH_PIV, MFS_GRAYED);
		menu->EnableMenuItem(ID_FOLDER_TXT, MFS_GRAYED);
		menu->EnableMenuItem(ID_OPEN_REPORT, MFS_GRAYED);
		menu->EnableMenuItem(ID_CLOSE_PROJ, MFS_GRAYED);
	}		
	
	menu = m_contextMenu.GetSubMenu(1);
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

void CPIVDlg::OnCbnSelchangeCombo()
{
	// TODO: добавьте свой код обработчика уведомлений
}


void CPIVDlg::OnBnClickedBtnHome()
{
	// TODO: добавьте свой код обработчика уведомлений
}


void CPIVDlg::OnBnClickedBtnPrev()
{
	// TODO: добавьте свой код обработчика уведомлений
}


void CPIVDlg::OnBnClickedBtnNext()
{
	// TODO: добавьте свой код обработчика уведомлений
}


void CPIVDlg::OnBnClickedBtnRefresh()
{
	// TODO: добавьте свой код обработчика уведомлений
}
