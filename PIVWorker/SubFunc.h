#pragma once

#include <cstring>
#include <vector>
#include <map>

using namespace std;

// Обозначения старшей и младшей частей в протоколах КПРНО35
namespace dwPart
{
	// Размеры массивов должны быть одинаковы
	const vector <CString> low = { L"(мл.ч)", L"(мл.ч.)", L"(мл. ч)", L"(мл. ч.)" };
	const vector <CString> hight = { L"(ст.ч)", L"(ст.ч.)", L"(ст. ч)", L"(ст. ч.)" };
	const vector <CString> hight2 = { L"(cт.ч)", L"(cт.ч.)", L"(cт. ч)", L"(cт. ч.)" }; // Версия старшей части с английской 'c'

																						// Проверка на присутствии в строке (мл.ч.)
	static bool checkLow(const CString& str)
	{
		for (size_t i = 0; i < low.size(); i++)
			if (str.Find(low[i]) != -1)
				return true;
		return false;
	}
	// Проверка на присутствии в строке (ст.ч.)
	static bool checkHight(const CString& str)
	{
		for (size_t i = 0; i < hight.size(); i++)
			if (str.Find(hight[i]) != -1 || str.Find(hight2[i]) != -1)
				return true;
		return false;
	}
	// Удаление (ст.ч.) - для генерации txt файлов
	static bool deleleHight(CString& str)
	{
		for (size_t i = 0; i < hight.size(); i++)
		{
			int index = str.Find(hight[i]);
			int index2 = str.Find(hight2[i]);

			if (index != -1)
			{
				str.Delete(index, hight[i].GetLength());
				str.TrimRight();
				return true;
			}
			else if (index2 != -1)
			{
				str.Delete(index2, hight2[i].GetLength());
				str.TrimRight();
				return true;
			}
		}
		return false;
	}
}

// Преобразование кириллицы в латиницу
namespace CyrToLat
{
	// Карта соответствий кириллицы (1) и латиницы (2)
	// лмнопрстуфхцчшщьыъэюя
	const map <wchar_t, wchar_t> ctl = { { L'а', L'a' },{ L'А', L'A' },
	{ L'В', L'B' },{ L'е', L'e' },{ L'Е', L'E' },{ L'ё', L'e' },
	{ L'Ё', L'E' },{ L'к', L'k' },{ L'К', L'K' },{ L'М', L'M' },
	{ L'Н', L'H' },{ L'о', L'o' },{ L'О', L'O' },{ L'р', L'p' },
	{ L'Р', L'P' },{ L'с', L'c' },{ L'С', L'C' },{ L'Т', L'T' },
	{ L'у', L'y' },{ L'У', L'Y' },{ L'х', L'x' },{ L'Х', L'X' } };

	static void CyrToLat(CString& str)
	{
		return;
	}
}
