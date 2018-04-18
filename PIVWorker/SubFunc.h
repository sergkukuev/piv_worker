#pragma once

#include <cstring>
#include <vector>
#include <map>

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
namespace CyrToLat
{
	// ����� ������������ ��������� (1) � �������� (2)
	// ���������������������
	const map <wchar_t, wchar_t> ctl = { { L'�', L'a' },{ L'�', L'A' },
	{ L'�', L'B' },{ L'�', L'e' },{ L'�', L'E' },{ L'�', L'e' },
	{ L'�', L'E' },{ L'�', L'k' },{ L'�', L'K' },{ L'�', L'M' },
	{ L'�', L'H' },{ L'�', L'o' },{ L'�', L'O' },{ L'�', L'p' },
	{ L'�', L'P' },{ L'�', L'c' },{ L'�', L'C' },{ L'�', L'T' },
	{ L'�', L'y' },{ L'�', L'Y' },{ L'�', L'x' },{ L'�', L'X' } };

	static void CyrToLat(CString& str)
	{
		return;
	}
}
