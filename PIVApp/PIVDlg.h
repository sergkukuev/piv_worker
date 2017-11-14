
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
	CMenu m_contextMenu;		// Контекстное меню
	CListBox* m_ListBox;		// Список открытых протоколов
	CStatusBarCtrl* m_StatusBar;
	HICON m_hIcon;
	HANDLE hWait;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	// Логирование
	friend void Waiting(CPIVDlg& app);
	void ReceiveMessage();

	DECLARE_MESSAGE_MAP()
private:
	CPIV piv;	// Класс для работы с DLL по обработке протоколов
	std::vector <CString> pathProj;		// Пути файлов проекта
	std::vector <CString> pathOther;	// Пути отдельных файлов

	CString getFolder();	// Выбор папки
	void OpenFile(CString& folder, vector <CString>& path);			// Открытие файлов
	void readPath(const CFileDialog& dlg, vector <CString>& path);	// Считывание путей файлов
	CString NameFromPath(const CString& path); // Имя файла из его пути

	// Взаимодействие с диалоговым окном
	void RefreshList();		// Обновление ListBox
	void SetNumericPIV();	// Установка количество считанных ПИВ
	void UpdateMenu();		// Обновление меню
public:
	// Обработчики событий
	afx_msg void OnOpenProj();			// Открытие проекта
	afx_msg void OnRefreshPiv();		// Обновление выбранных файлов
	afx_msg void OnChangeFolder();		// Смена директории
	afx_msg void OnOpenReport();		// Открыть отчет в окне
	afx_msg void OnFolderTxt();			// Открыть папку с тхт файлами
	afx_msg void OnOpenPiv();			// Открыть отдельно пив
	afx_msg void OnClosePiv();			// Закрыть выбранные пив
	afx_msg void OnCloseAll();			// Закрыть все
	afx_msg void OnCbnSelchangeCombo();	// Смена режима работы (проект, отдельные ПИВ)
	afx_msg void OnBnClickedSetting();	// Открыть окно настройки
	afx_msg void OnBnClickedShowRep();	// Отображение отчета

	//	Работа с браузером
	afx_msg void OnBnClickedBtnHome();
	afx_msg void OnBnClickedBtnPrev();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedBtnRefresh();
};
