#pragma once

#include <cstring>
#include <vector>

using namespace std;

// ����������� ������� � ������� ������ � ���������� �����35
namespace dwPart
{
	// ������� �������� ������ ���� ���������
	const vector <CString> low = { L"(��.�)", L"(��.�.)", L"(��. �)", L"(��. �.)" };
	const vector <CString> hight = { L"(��.�)", L"(��.�.)", L"(��. �)", L"(��. �.)" };
	const vector <CString> hight2 = { L"(c�.�)", L"(c�.�.)", L"(c�. �)", L"(c�. �.)" }; // ������ ������� ����� � ���������� 'c'

																						// �������� �� ����������� � ������ (��.�.)
	static bool checkLow(const CString& str)
	{
		for (size_t i = 0; i < low.size(); i++)
			if (str.Find(low[i]) != -1)
				return true;
		return false;
	}
	// �������� �� ����������� � ������ (��.�.)
	static bool checkHight(const CString& str)
	{
		for (size_t i = 0; i < hight.size(); i++)
			if (str.Find(hight[i]) != -1 || str.Find(hight2[i]) != -1)
				return true;
		return false;
	}
	// �������� (��.�.) - ��� ��������� txt ������
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

// �������������� ��������� � ��������
namespace myctl
{
	// ����� ������������ ��������� (1) � �������� (2)
	// ���������������������
	/*const map <wchar_t, wchar_t> ctl = { { L'�', L'a' },{ L'�', L'A' },
	{ L'�', L'B' },{ L'�', L'e' },{ L'�', L'E' },{ L'�', L'e' },
	{ L'�', L'E' },{ L'�', L'k' },{ L'�', L'K' },{ L'�', L'M' },
	{ L'�', L'H' },{ L'�', L'o' },{ L'�', L'O' },{ L'�', L'p' },
	{ L'�', L'P' },{ L'�', L'c' },{ L'�', L'C' },{ L'�', L'T' },
	{ L'�', L'y' },{ L'�', L'Y' },{ L'�', L'x' },{ L'�', L'X' } };*/

	// ���������� ���� ��������� � ������� �� ���� ������ ���������� ��������
	static void Cyr2Lat(CString& result)
	{
		CString temp = result;
		for (int i = 0; i < result.GetLength(); i++)
		{
			wchar_t symbol;
			switch (temp[i])
			{
			case L'�': symbol = L'a'; break;
			case L'�': symbol = L'A'; break;
			case L'�': symbol = L'b'; break;
			case L'�': symbol = L'B'; break;
			case L'�': symbol = L'r'; break;
			case L'�': symbol = L'e'; break;
			case L'�': symbol = L'E'; break;
			case L'�': symbol = L'e'; break;
			case L'�': symbol = L'E'; break;
			case L'�': symbol = L'N'; break;
			case L'�': symbol = L'k'; break;
			case L'�': symbol = L'K'; break;
			case L'�': symbol = L'm'; break;
			case L'�': symbol = L'M'; break;
			case L'�': symbol = L'o'; break;
			case L'�': symbol = L'O'; break;
			case L'�': symbol = L'n'; break;
			case L'�': symbol = L'p'; break;
			case L'�': symbol = L'P'; break;
			case L'�': symbol = L'c'; break;
			case L'�': symbol = L'C'; break;
			case L'�': symbol = L'T'; break;
			case L'�': symbol = L'y'; break;
			case L'�': symbol = L'Y'; break;
			case L'�': symbol = L'x'; break;
			case L'�': symbol = L'X'; break;
			default: symbol = temp[i];	// ����� �� �� ��� �� ������, ��������� ��� ����
			// ��������� ����� ���������
			/*case L'�': return L'B';
			case L'�': return L'v';
			case L'�': return L'G';
			case L'�': return L'd';
			case L'�': return L'D';
			case L'�': return L'z';
			case L'�': return L'z';
			case L'�': return L'Z';
			case L'�': return L'z';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'l';
			case L'�': return L'L';
			case L'�': return L't';
			case L'�': return L'N';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';
			case L'�': return L'';*/
			}
			temp.Replace(temp[i], symbol);
		}
		result = temp;
	}
}
