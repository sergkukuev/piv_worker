
// PIVDlg.h : файл заголовка
//

#pragma once
#include "afxwin.h"
#include <vector>

#include "PIVApp.h"
#include "SettingDlg.h"
#include "WebBrowser.h"
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
	CMenu m_contextMenu;		// Контекстное меню
	CListBox* m_ListBox;		// Список открытых протоколов
	CStatusBar m_StatusBar;
	WebBrowser browser;
	HICON m_hIcon;
	HANDLE hWait;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	// Логирование
	friend void Waiting(CPIVDlg& dlg, HWND);
	void ReceiveMessage(HWND);

	DECLARE_MESSAGE_MAP()
private:
	CPIV piv;	// Класс для работы с DLL по обработке протоколов
	vector <CString> pathProj;		// Пути файлов проекта
	vector <CString> pathOther;	// Пути отдельных файлов
	
	void OpenFile(vector <CString>& path);	// Открытие файлов
	CString GetFolder();	// Выбор папки
	void ReadPath(const CFileDialog& dlg, vector <CString>& path);	// Считывание путей файлов
	CString NameFromPath(const CString& path); // Имя файла из его пути

	// Взаимодействие с диалоговым окном
	void RefreshList();		// Обновление ListBox
	void SetNumericPIV();	// Установка количество считанных ПИВ
	void UpdateMenu();		// Обновление меню
	void BrowserNavigate();	// Установка пути отчета для встроенного браузера
public:
	stgParams settings;	// Структура настроек

	// Обработчики событий
	afx_msg void OnOpenProj();			// Открытие проекта
	afx_msg void OnRefreshPiv();		// Обновление выбранных файлов
	afx_msg void OnChangeFolder();		// Смена директории
	afx_msg void OnOpenReport();		// Открыть отчет в окне
	afx_msg void OnOpenFolder();		// Открыть папку с артефактами
	afx_msg void OnOpenPiv();			// Открыть отдельно протоколы
	afx_msg void OnClosePiv();			// Закрыть выбранные протоколы
	afx_msg void OnCloseAll();			// Закрыть все
	afx_msg void OnCbnSelchangeCombo();	// Смена режима работы (проект, отдельные протоколы)
	afx_msg void OnBnClickedSetting();	// Открыть окно настройки
	afx_msg void OnBnClickedShowRep();	// Отображение отчета
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg LRESULT OnReceiveMessage(WPARAM, LPARAM lParam);	// Отображение статуса DLL

	//	Работа с браузером
	afx_msg void OnBnClickedBtnHome();
	afx_msg void OnBnClickedBtnPrev();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedBtnRefresh();
	afx_msg void OnClose();
};
