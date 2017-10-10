
// PIVDlg.h : файл заголовка
//

#pragma once
#include "afxwin.h"
#include <vector>
#include "PIVWorker.h"

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
	CListBox* m_ListBox;	// Список открытых протоколов
	HICON m_hIcon;

	// Созданные функции схемы сообщений
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

	CString getFolder();	// Выбор папки
public:
	DECLARE_EVENTSINK_MAP()
	void OnChangeTbtnReport();		// Обработчик на нажатия тогл кнопочки
	afx_msg void OnOpenProj();		// Открытие проекта
	afx_msg void OnRefreshPiv();	// Обновление выбранных файлов
	afx_msg void OnChangeFolder();	// Смена директории
	afx_msg void OnOpenReport();	// Открыть отчет в окне
	afx_msg void OnFolderTxt();		// Открыть папку с тхт файлами
	afx_msg void OnOpenPiv();		// Открыть отдельно пив
	afx_msg void OnClosePiv();		// Закрыть выбранные пив
	afx_msg void OnCloseAll();		// Закрыть все
	afx_msg void OnCloseProj();		// Очистить проект	std::vector <CString> pathOther;
	afx_msg void OnCbnSelchangeCombo();
	afx_msg void OnBnClickedBtnHome();
	afx_msg void OnBnClickedBtnPrev();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedBtnRefresh();
};
