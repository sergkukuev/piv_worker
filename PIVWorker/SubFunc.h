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
			case L'�': symbol = L'a';
			case L'�': symbol = L'A';
			case L'�': symbol = L'b';
			case L'�': symbol = L'B';
			case L'�': symbol = L'r';
			case L'�': symbol = L'e';
			case L'�': symbol = L'E';
			case L'�': symbol = L'e';
			case L'�': symbol = L'E';
			case L'�': symbol = L'N';
			case L'�': symbol = L'k';
			case L'�': symbol = L'K';
			case L'�': symbol = L'm';
			case L'�': symbol = L'M';
			case L'�': symbol = L'o';
			case L'�': symbol = L'O';
			case L'�': symbol = L'n';
			case L'�': symbol = L'p';
			case L'�': symbol = L'P';
			case L'�': symbol = L'c';
			case L'�': symbol = L'C';
			case L'�': symbol = L'T';
			case L'�': symbol = L'y';
			case L'�': symbol = L'Y';
			case L'�': symbol = L'x';
			case L'�': symbol = L'X';
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
