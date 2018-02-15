#pragma once

#include "afxdialogex.h"

const UINT WM_EXCHANGE_DLL = ::RegisterWindowMessage(_T("WM_EXCHANGE_DLL"));	// Событие обмена сообщением
const UINT WM_SETTINGS_DLL = ::RegisterWindowMessage(_T("WM_SETTINGS_DLL"));	// Событие сохранения настроек