#pragma once
#include <vector>

#include "DllDeclare.h"

namespace data_pw
{
	const std::vector <CString> line = { L"МКИО", L"РТМ" };	// Линии передачи
	enum line { NOT_DEFINED = -1, MKIO, ARINC };	// Линия передачи
	enum class status { empty = -2 /* отсутствие ячейки (параметра) */, failed /* неудалось прочесть */, opt /* эл-т необязателен */ };
	enum fields { NUMWORD, TITLE, ID, TYPE, MIN, MAX, CSR, BITS, COMMENT };	// Индексы параметров сигнала
}

// Класс-хранилище данных о ПИВ
class PIV_DECLARE CData
{
public:
	CData();
	~CData();

	// Необходимая информация над таблицей
	struct Head
	{
		CString name = L"";	// Название листа
		int line = (int)data_pw::MKIO;			// Линия передачи (enum line)
		int np = (int)data_pw::status::empty;	// Номер набора параметров или Номер индикационного кадра (enum stats)
		int pk = (int)data_pw::status::empty;	// Номер подкадра или Номер ПУИ страницы (enum stats)
		// Индексы сигналов, где обнаружены значения
		int iNp = (int)data_pw::status::empty;	// Индекс параметра номера набора или Номера индикационного кадра
		int iPk = (int)data_pw::status::opt;	// Индекс параметра подкадра или Номера ПУИ страницы
	};
	// Примечания разработчиков ПИВ
	struct DevRemark
	{
		int index = -1;		// Индекс сигнала, над которым найдено примечание
		CString desc = L"";	// Запись разработчика
	};
	// Все значение параметра
	// 1. № слова / Адрес
	// 2. Наименование
	// 3. Идентификатор
	// 4. Размерность
	// 5. Минимальное значение
	// 6. Максимальное значение
	// 7. Цена старшего разряда
	// 8. Используемые разряды 
	// 9. Примечания
	struct Signal
	{
		std::vector<CString> value;	
		bool valid = true;	// Валидность сигнала (true - без ошибок, false - с ошибками)
	};
	// Лист
	struct Sheet
	{
		Head head;	// Шапка таблицы
		std::vector<DevRemark> remarks;	// Примечания разработчиков
		std::vector <Signal> signals;	// Параметры
	};

	// Получить имя книги
	CString Name();
	// Установить имя книги
	void SetName(const CString&);
	// Получить ссылку на параметры
	std::vector<Sheet>& Sheets();
private:
	CString name;	// Название книги
	std::vector<Sheet> sheets;	// Необработанные данные с листов
};

