
// PIVDlg.h : файл заголовка
//

#pragma once
#include "afxwin.h"

// Значения комбо-бокса для определения вывода контекстного меню
#define PROJECT 0
#define OTHER 1

// диалоговое окно CPIVDlg
class CPIVDlg : public CDialogEx
{
// Создание
public:
	CPIVDlg(CWnd* pParent = NULL);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PIVAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	bool SHOW_REPORT = true;	// Отображение отчета
	CMenu m_contextMenu;	// Контекстное меню
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void OnChangeTbtnReport();	// Обработчик на нажатия тогл кнопочки
	CButton m_BtnHome;
};
