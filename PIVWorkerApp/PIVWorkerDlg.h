
// PIVWorkerDlg.h : файл заголовка
//

#pragma once

#include "PIVWorker.h"

// диалоговое окно CMainDlg
class CMainDlg : public CDialog
{
// Создание
public:
	CMainDlg(CWnd* pParent = NULL);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PIVWORKERAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
private:
	CPIV piv;				// Переменная для работы с протоколами
	vector <CString> path;	// Пути файлов
	
	void AddPath(CString fullPath, CString name, CListBox* list);	// Добавление путей файлов
	void readPath(const CFileDialog& dlg, CListBox* list);			// Получить вектор путей из диалогового окна
	
	void OpenFile(CListBox* list, CString& folder);	// Открытие файлов
	CString getFolder();	// Получение пути артефактов
	void logicMenu();		// Отображение меню и подменю

protected:
	CMenu m_subMenu;			// Подменю
	CStatusBarCtrl* pStatusBar;	// Статус бар
	CListBox* pList;			// Список ПИВ проекта
	CListBox* pListOther;		// Список ПИВ остальных
	HACCEL m_AccelTable;		// Таблица быстрыъ клавиш
	HICON m_hIcon;

	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);	// Обработка контекстного меню

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();	// Инициализация
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP();

public:
	// Обработчики функций
	afx_msg void OnPivSetFolder();		// Установить папку для отчетов и txt
	afx_msg void OnPivRefresh();		// Обновление отчетов
	afx_msg void OnOpenProject();		// Открытие проекта
	afx_msg void OnPivOpen();			// Открытие протоколов
	afx_msg void OnPivClose();			// Закрытие протокола
	afx_msg void OnPivCloseAll();		// Закрыть все протоколы
	afx_msg void OnOtherReport();		// Открыть отчет по остальным
	afx_msg void OnProjectReport();		// Открыть отчет по проекту
	afx_msg void OnPivRepFolder();		// Открыть папку с отчетом
	afx_msg void OnPivTxtOpen();		// Открыть txt отчет
	afx_msg void OnAppInform();			// Информация о проекте
};
