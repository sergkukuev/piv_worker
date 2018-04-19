#pragma once

#include <cstring>
#include <vector>

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
namespace myctl
{
	// Карта соответствий кириллицы (1) и латиницы (2)
	// лмнопрстуфхцчшщьыъэюя
	/*const map <wchar_t, wchar_t> ctl = { { L'а', L'a' },{ L'А', L'A' },
	{ L'В', L'B' },{ L'е', L'e' },{ L'Е', L'E' },{ L'ё', L'e' },
	{ L'Ё', L'E' },{ L'к', L'k' },{ L'К', L'K' },{ L'М', L'M' },
	{ L'Н', L'H' },{ L'о', L'o' },{ L'О', L'O' },{ L'р', L'p' },
	{ L'Р', L'P' },{ L'с', L'c' },{ L'С', L'C' },{ L'Т', L'T' },
	{ L'у', L'y' },{ L'У', L'Y' },{ L'х', L'x' },{ L'Х', L'X' } };*/

	// Приведение букв кириллицы к похожим по виду буквам латинского алфавита
	static void Cyr2Lat(CString& result)
	{
		CString temp = result;
		for (int i = 0; i < result.GetLength(); i++)
		{
			wchar_t symbol;
			switch (temp[i])
			{
			case L'а': symbol = L'a'; break;
			case L'А': symbol = L'A'; break;
			case L'б': symbol = L'b'; break;
			case L'В': symbol = L'B'; break;
			case L'г': symbol = L'r'; break;
			case L'е': symbol = L'e'; break;
			case L'Е': symbol = L'E'; break;
			case L'ё': symbol = L'e'; break;
			case L'Ё': symbol = L'E'; break;
			case L'И': symbol = L'N'; break;
			case L'к': symbol = L'k'; break;
			case L'К': symbol = L'K'; break;
			case L'м': symbol = L'm'; break;
			case L'М': symbol = L'M'; break;
			case L'о': symbol = L'o'; break;
			case L'О': symbol = L'O'; break;
			case L'п': symbol = L'n'; break;
			case L'р': symbol = L'p'; break;
			case L'Р': symbol = L'P'; break;
			case L'с': symbol = L'c'; break;
			case L'С': symbol = L'C'; break;
			case L'Т': symbol = L'T'; break;
			case L'у': symbol = L'y'; break;
			case L'У': symbol = L'Y'; break;
			case L'х': symbol = L'x'; break;
			case L'Х': symbol = L'X'; break;
			default: symbol = temp[i];	// Буква ни на что не похожа, оставляем как есть
			// Непохожие буквы кириллицы
			/*case L'Б': return L'B';
			case L'в': return L'v';
			case L'Г': return L'G';
			case L'д': return L'd';
			case L'Д': return L'D';
			case L'ж': return L'z';
			case L'Ж': return L'z';
			case L'З': return L'Z';
			case L'з': return L'z';
			case L'и': return L'';
			case L'й': return L'';
			case L'Й': return L'';
			case L'л': return L'l';
			case L'Л': return L'L';
			case L'т': return L't';
			case L'П': return L'N';
			case L'ф': return L'';
			case L'Ф': return L'';
			case L'ц': return L'';
			case L'Ц': return L'';
			case L'ч': return L'';
			case L'Ч': return L'';
			case L'ш': return L'';
			case L'Ш': return L'';
			case L'щ': return L'';
			case L'Щ': return L'';
			case L'ъ': return L'';
			case L'Ъ': return L'';
			case L'ы': return L'';
			case L'Ы': return L'';
			case L'ь': return L'';
			case L'Ь': return L'';
			case L'э': return L'';
			case L'Э': return L'';
			case L'ю': return L'';
			case L'Ю': return L'';
			case L'я': return L'';
			case L'Я': return L'';*/
			}
			temp.Replace(temp[i], symbol);
		}
		result = temp;
	}
}
