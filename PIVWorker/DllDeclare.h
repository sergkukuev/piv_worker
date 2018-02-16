#pragma once

// Определение импорта или экспорта DLL
#ifdef PIV_WORKER_EXPORTS
#define PIV_DECLARE __declspec(dllimport)
#else
#define PIV_DECLARE __declspec(dllexport)
#endif