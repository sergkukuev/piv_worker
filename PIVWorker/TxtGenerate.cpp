#include "stdafx.h"

#include <fstream>

#include "Report.h"
#include "SubFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ������ 
bool CReport::GetTxt(const bookData& book)
{
	try 
	{
		Generate(book);
	}
	catch (MyException& exc)
	{
		logger >> exc.GetMsg();
		return false;
	}
	return true;
}

// ����������
bool CReport::GetTxt(list <bookData>& books)
{
	bool result = true;
	
	// ����� �� ������
	for (list <bookData>::iterator it = books.begin(); it != books.end(); it++)
	{
		try
		{
			Generate(*it);
		}
		catch (MyException& exc)
		{
			logger >> exc.GetMsg();
			result = false;
		}
	}
	return result;
}

// ��������� txt ����������
void CReport::Generate(const bookData& book)
{
	logger >> L"�������� txt-������ ��������� \"" + book.name + L"\"...";
	CString path = settings.GetPath();
	path.Format(L"%s%s\\%s", path, settings.folders[text], book.name);
	if (!CreateDir(path))
		throw FolderNotCreated(path);

	// ����������� �������� ��� ��������� ��������� ������
	CString mode;
	settings.GetParHide() ? mode = L"��������" : mode = L"���������";
	logger >> L"�������� txt-������ ��� ������� ������ �� �����: " + mode;
	settings.GetNumPk() ? mode = L"��������" : mode = L"���������";
	logger >> L"����������� ������ �������� � ����� ������: " + mode;
	// �������� �������� ����� ��� ������
	CString filePath;
	filePath.Format(L"%s\\_ProtocolMain.txt", path);
	ofstream mainFile;
	mainFile.open(filePath);
	// ����� �� ���������
	int cNP = 1, cNumWord = 1;	// CRUTCH: ��������� �������� ��� arinc �������� ��� ����������� � ����� �����
	for (size_t i = 0; i < book.sheets.size(); i++, cNP++, cNumWord = 1)
	{
		CString name;
		sheetInfo info;
		info.arinc = book.sheets[i].arinc;
		info.arinc ? info.np = cNP : info.np = book.sheets[i].np.value;
		info.pk = book.sheets[i].pk;
		info.name = book.sheets[i].name;

		// �������� txt ����� ��� ������
		name.Format(L"NP_%d_%s.txt", info.np, book.sheets[i].name);
		filePath.Format(L"%s\\%s", path, name);
		ofstream tmpFile(filePath);
		mainFile << "#include \"";
		mainFile << CT2A(name);
		mainFile << "\"\n";

		for (size_t j = 0; j < book.sheets[i].signals.size(); j++)
		{
			if (j != 0 && info.arinc)	
				if (book.sheets[i].signals[j].numWord.value[0] != book.sheets[i].signals[j - 1].numWord.value[0])
					cNumWord++;
			WriteTxtParam(tmpFile, book.sheets[i].signals[j], info, cNumWord);	// ������ ���������
		}
		tmpFile.close();
	}
	mainFile.close();
}

// ������ ������ ������ ������ �� ������� � txt ����
void CReport::WriteTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info, const int& arincNum)
{
	CString buffer = signal.title[0];

	if (buffer.Find(RESERVE_SIGNAL) != -1 || (settings.GetProject() == project::kprno35 && dwPart::checkLow(buffer)))
		return;
	/*if (signal.error && !settings.GetParHide())
	{
		logger >> L"�������� " + signal.title[1] + L" (���� \"" + info.name + L"\") �� ������������";
		return;
	}*/

	bool dwKprno = WriteParamTitle(file, signal, info);
	bool bParHide = (signal.error && settings.GetParHide()) ? true : false; // ������ ���������� �����, ����� � ���� ���� ������, � ����� ���� "�������� ������"
	// �����������
	if (!signal.dimension.IsEmpty())
	{
		buffer.Remove(L'\n');
		buffer.Remove(L'\t');
		buffer.Format(L"\tUNIT=\"%s\"", signal.dimension);	
		WriteFile(file, buffer, bParHide);
	}

	// ��������� ������
	info.pk > 0 && settings.GetNumPk() ? buffer.Format(L"\tSET=%d:%d", info.pk, info.np) : buffer.Format(L"\tSET=%d", info.np);	// CRUTCH: ��������� �������� � ����� ������
	WriteFile(file, buffer, bParHide);

	// ��������� ������ ���������
	paramInfo par;
	par.arinc = info.arinc;
	par.arincNum = arincNum;
	par.dwKprno = dwKprno;
	WriteParamAdr(file, signal, par);

	// CRUTCH: ������ ������������
	if (!signal.comment.IsEmpty() && settings.GetProject() != project::kprno35)
	{
		bool bComHide = settings.GetComHide() || bParHide;
		WriteFile(file, L"\tCOMMENT", bComHide);
		buffer = signal.comment;
		buffer.Remove(L'\"');
		buffer.Insert(0, L"\t\t\"");
		buffer.Replace(L"\n", L"\"\n\t\t\"");	// ������� ������ � �������� �� ��� ���������
		buffer.Insert(buffer.GetLength(), L"\"");
		WriteFile(file, buffer, bComHide);
		WriteFile(file, L"\tEND_COMMENT", bComHide);
	}
	WriteFile(file, L"END_PAR\n", bParHide);
}

// ������ ������������ ���������
bool CReport::WriteParamTitle(ofstream& file, const signalData& signal, const sheetInfo& info)
{
	CString buffer = signal.title[1];
	buffer.Remove(L' ');
	if (signal.repWord)	// �������� ����������� � ������ ������, ���������� �������� � ��� ����� ��������
	{
		int index = 1;
		if (info.pk != stats::empty && info.pk != stats::failed)
			index = info.pk;
		else if (info.np != stats::empty && info.np != stats::failed)
			index = info.np;
		buffer.Format(L"%s_%d", buffer, index);
		// ������ ��������
		CString msg;
		msg.Format(L"�������� %s (���� \"%s\") �� ��������, � �������� ��������� \"_%d\"", signal.title[1], info.name, index);
		logger >> msg;
	}
	// ������ ���������� �����, ����� � ���� ���� ������, � ����� ���� "�������� ������"
	bool bParHide = (signal.error && settings.GetParHide()) ? true : false;
	// ������ ����������� �������
	buffer.Remove(L'\n');
	buffer.Remove(L'\t');
	buffer.Remove(L' ');
	WriteFile(file, L"PAR=" + buffer, bParHide);	// TODO: ������� �������������� ��������� � ��������
	//buffer.Format(L"%s%s\n", L"PAR=", buffer);	// TODO: � ��������� ������� ����� CString.Format() ������ ������������ ������

	// ������������ �������
	buffer = signal.title[0];
	bool dwKprno = false;
	if (settings.GetProject() == project::kprno35)
		dwKprno = dwPart::deleleHight(buffer);
	buffer.Replace(L"\"", L"\\\"");
	buffer.Replace(L"\n", L" ");
	buffer.Replace(L"\t", L" ");
	//buffer.Format(L"\tNAME=\"%s\"\n", buffer);
	WriteFile(file, L"\tNAME=\"" + buffer + L"\"", bParHide);
	return dwKprno;
}

// ������ ������ ���������
void CReport::WriteParamAdr(ofstream& file, const signalData& signal, const paramInfo& param)
{
	CString buffer;
	bool bParHide = (signal.error && settings.GetParHide()) ? true : false; // ������ ���������� �����, ����� � ���� ���� ������, � ����� ���� "�������� ������"
	int max = (signal.bit.value[1] == -1 ? signal.bit.value[0] : signal.bit.value[1]);

	// ������ ������ ����� � �����
	if (signal.numWord.value.size() == 2 || (param.dwKprno && signal.part != nullptr))
	{
		WriteFile(file, L"\tMERGE", bParHide);
		// ������������ ������ ����� ����
		buffer.Format(L"\t\tWDADDR = %d,%d,%d", signal.numWord.value[0], signal.bit.value[0], max);
		WriteFile(file, buffer, bParHide);
		// ������������ ������ ����� ����
		if (signal.numWord.value.size() == 2)
		{
			max = (signal.bit.value[3] == -1) ? signal.bit.value[2] : signal.bit.value[3];
			buffer.Format(L"\t\tWDADDR = %d,%d,%d", signal.numWord.value[1], signal.bit.value[2], max);
		}
		else if (param.dwKprno && signal.part != nullptr)
		{
			max = (signal.part->bit.value[1] == -1) ? signal.part->bit.value[0] : signal.part->bit.value[1];
			buffer.Format(L"\t\tWDADDR = %d,%d,%d", signal.part->numWord.value[0], signal.part->bit.value[0], max);
		}
		else
			buffer.Empty();
		WriteFile(file, buffer, bParHide);
		WriteFile(file, L"\tEND_MERGE", bParHide);
	}
	else
	{
		param.arinc ? buffer.Format(L"\tWDADDR = %d,%d,%d", param.arincNum, signal.bit.value[0], max) :
			buffer.Format(L"\tWDADDR = %d,%d,%d", signal.numWord.value[0], signal.bit.value[0], max);
		WriteFile(file, buffer, bParHide);
	}

	// ���, ����, ���
	if (signal.max.value != DBL_MIN && signal.csr.value != DBL_MIN)
	{
		WriteFile(file, L"\tVALDESCR", bParHide); 
		signal.bitSign ? WriteFile(file, L"\t\tSIGNED", bParHide) : WriteFile(file, L"\t\tUNSIGNED", bParHide);
		IsInt(signal.min.value) ? buffer.Format(L"\t\tMIN = %.0lf", signal.min.value) : buffer.Format(L"\t\tMIN = %lf", signal.min.value);
		WriteFile(file, buffer, bParHide);
		IsInt(signal.max.value) ? buffer.Format(L"\t\tMAX = %.0lf", signal.max.value) : buffer.Format(L"\t\tMAX = %lf", signal.max.value);
		WriteFile(file, buffer, bParHide);
		IsInt(signal.csr.value) ? buffer.Format(L"\t\tMSB = %.0lf", signal.csr.value) : buffer.Format(L"\t\tMSB = %lf", signal.csr.value);
		WriteFile(file, buffer, bParHide);
		WriteFile(file, L"\tEND_VALDESCR", bParHide);
	}
}

// ������ ������ � ����
void CReport::WriteFile(ofstream& file, CString text, bool hide)
{
	if (hide)	// ������������ ������
	{
		text.Insert(0, L"//");
		text.Replace(L"\n", L"\n//");
	}
	text.Insert(text.GetLength(), L"\n");	// ��������� � ����� ������ ������������ �������� �� ����� ������
	file << CT2A(text);
}

// �������� �� int ��������
bool CReport::IsInt(const double& numeric)
{
	int whole = (int)numeric;
	double fract = numeric - whole;
	bool result;
	fract == 0 ? result = true : result = false;
	return result;
}