
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
	CPIVWorker piv;			// Переменная для работы с протоколами
	CString folder;			// Путь для отчета
	vector <CString> path;	// Пути файлов
	HANDLE hWait;	// Поток для доступности потока класса CPIVWorker
	int cmd = 0;	// Номер команды для меню: 1 - открыть, 2 - анализировать, 3 - закрыть, 4 - генерировать отчет

	CListBox* lsBox;	// Лист бокс
	void setMenu(int command);	// Установка параметров меню
	void AddPath(CString fullPath, CString name);	// Добавление путей файлов

protected:
	HACCEL m_AccelTable;		// Таблица акселераторов
	HICON m_hIcon;				// Иконка

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	friend void waitThread(CMainDlg& object);	// Дружественная функция для ожидания окончания потока

public:
	void logicMenu(int command);		// Логика работы меню
	afx_msg void OnPivOpen();			// Открытие протоколов
	afx_msg void OnPivClose();			// Закрытие протокола
	afx_msg void OnPivCloseAll();		// Закрыть все протоколы
	afx_msg void OnPivAnalyze();		// Анализировать протоколы
	afx_msg void OnPivReport();			// Открыть отчет
	afx_msg void OnPivRepFolder();		// Открыть папку с отчетом
	afx_msg void OnPivTxtGenerate();	// Генерировать txt отчет
	afx_msg void OnAppInform();			// Информация о проекте
};
