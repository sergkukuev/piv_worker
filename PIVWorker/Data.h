#pragma once
#include <vector>

#include "DllDeclare.h"

namespace data_pw
{
	const std::vector <CString> line = { L"����", L"���" };	// ����� ��������
	enum line { NOT_DEFINED = -1, MKIO, ARINC };	// ����� ��������
	enum class status { empty = -2 /* ���������� ������ (���������) */, failed /* ��������� �������� */, opt /* ��-� ������������ */ };
	enum fields { NUMWORD, TITLE, ID, TYPE, MIN, MAX, CSR, BITS, COMMENT };	// ������� ���������� �������
}

// �����-��������� ������ � ���
class PIV_DECLARE CData
{
public:
	CData();
	~CData();

	// ����������� ���������� ��� ��������
	struct Head
	{
		CString name = L"";	// �������� �����
		int line = (int)data_pw::MKIO;			// ����� �������� (enum line)
		int np = (int)data_pw::status::empty;	// ����� ������ ���������� ��� ����� �������������� ����� (enum stats)
		int pk = (int)data_pw::status::empty;	// ����� �������� ��� ����� ��� �������� (enum stats)
		// ������� ��������, ��� ���������� ��������
		int iNp = (int)data_pw::status::empty;	// ������ ��������� ������ ������ ��� ������ �������������� �����
		int iPk = (int)data_pw::status::opt;	// ������ ��������� �������� ��� ������ ��� ��������
	};
	// ���������� ������������� ���
	struct DevRemark
	{
		int index = -1;		// ������ �������, ��� ������� ������� ����������
		CString desc = L"";	// ������ ������������
	};
	// ��� �������� ���������
	// 1. � ����� / �����
	// 2. ������������
	// 3. �������������
	// 4. �����������
	// 5. ����������� ��������
	// 6. ������������ ��������
	// 7. ���� �������� �������
	// 8. ������������ ������� 
	// 9. ����������
	struct Signal
	{
		std::vector<CString> value;	
		bool valid = true;	// ���������� ������� (true - ��� ������, false - � ��������)
	};
	// ����
	struct Sheet
	{
		Head head;	// ����� �������
		std::vector<DevRemark> remarks;	// ���������� �������������
		std::vector <Signal> signals;	// ���������
	};

	// �������� ��� �����
	CString Name();
	// ���������� ��� �����
	void SetName(const CString&);
	// �������� ������ �� ���������
	std::vector<Sheet>& Sheets();
private:
	CString name;	// �������� �����
	std::vector<Sheet> sheets;	// �������������� ������ � ������
};

