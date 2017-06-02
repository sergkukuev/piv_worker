#include "stdafx.h"
#include "RepTxt.h"

// ������ ��������� txt ������
void CRepTxt::Generate(vector <bookData> books, CString pathToSave, bool bNumPK)
{
	// �������� ����������
	pathToSave.Format(_T("%s\\Text"), pathToSave);
	CreateDirectory(pathToSave, NULL);

	CString filePath = pathToSave;
	filePath.Format(_T("%s\\_ProtocolMain.txt"), pathToSave);

	// �������� �������� ����� ��� ������
	ofstream mainFile;
	mainFile.open(pathToSave);

	// ����� �� ������
	for (size_t iBook = 0; iBook < books.size(); iBook++)
	{
		// ����� �� ���������
		for (size_t iSheet = 0; iSheet < books[iBook].sheets.size(); iSheet++)
		{
			if (!books[iBook].sheets[iSheet].bError)	// ���� ��� �� �������� ������
			{
				ofstream tmpFile;
				CString name;
				int NP = books[iBook].sheets[iSheet].iFieldNP;
				
				// �������� txt ����� ��� ������
				name.Format(_T("NP_%d_%s.txt"), NP, books[iBook].sheets[iSheet].name);
				filePath.Format(_T("%s\\%s"), pathToSave, name);
				tmpFile.open(filePath);
				mainFile << "#include \""; mainFile << CT2A(name); mainFile << "\"\n";

				for (list <signalData>::iterator it = signal_set(iBook, iSheet).begin(); it != signal_set(iBook, iSheet).end(); it++)
				{
					sheetInfo info;
					info.NP = books[iBook].sheets[iSheet].iFieldNP;
					info.NumPk = books[iBook].sheets[iSheet].iNumPK;
					info.bNumPk = bNumPK;
					WriteParameter(tmpFile, it, info);	// ������ ���������
				}
				tmpFile.close();
			}
			else continue;
		}
	}
	mainFile.close();
}

// ������ ������� � txt ����
void CRepTxt::WriteParameter(ofstream& file, list <signalData>::iterator it, sheetInfo info)
{
	CString buffer = it->sTitleParamField[0];

	if (buffer.Find(_T("������")) == -1)
	{
		buffer.Format(_T("PAR=%s\n"), it->sTitleParamField[1]);	// ������ ����������� �������
		file << CT2A(buffer);

		buffer.Replace(_T("\""), _T("\\\""));
		buffer.Format(_T("\t NAME=\"%s\"\n"), it->sTitleParamField[0]); // ������������ �������
		file << CT2A(buffer);

		if (!it->sDimensionField.IsEmpty())
		{
			buffer.Format(_T("\t UNIT=\"%s\"\n"), it->sDimensionField);	// �����������
			file << CT2A(buffer);
		}

		// ��������� ������
		if (info.NumPk != -1 && info.bNumPk)
			buffer.Format(_T("\t SET=%d:%d\n"), info.NumPk, info.NP);
		else
			buffer.Format(_T("\t SET=%d\n"), info.NP);
		file << CT2A(buffer);

		int max = (it->iBit[1] == 0 ? it->iBit[0] : it->iBit[1]);

		if (it->b2NumWordField)	// ������ ������ ����� � �����
		{
			file << "\t MERGE\n";
			buffer.Format(_T("\t\t WDADDR = %d,%d,%d\n"), it->iNumWord[0], it->iBit[0], max);
			file << CT2A(buffer);
			buffer.Format(_T("\t\t WDADDR = %d,%d,%d\n"), it->iNumWord[1], it->iBit[0], max);
			file << CT2A(buffer);
			file << "\t END_MERGE\n";
		}
		else
		{
			buffer.Format(_T("\t\t WDADDR = %d,%d,%d\n"), it->iNumWord[0], it->iBit[0], max);
			file << CT2A(buffer);
		}

		if (it->dMinMaxCsrVal[1] != 0 && it->dMinMaxCsrVal[2] != 0)	// ���, ����, ���
		{
			file << "\t VALDESCR\n";
			file << (it->bCommentField ? "\t\t SIGNED\n" : "\t\t UNSIGNED\n");

			buffer = it->sMinMaxCsrValField[0];
			buffer.Replace(_T(","), _T("."));
			buffer.Format(_T("\t\t MIN = %s\n"), buffer);
			file << CT2A(buffer);

			buffer = it->sMinMaxCsrValField[1];
			buffer.Replace(_T(","), _T("."));
			buffer.Format(_T("\t\t MAX = %s\n"), buffer);
			file << CT2A(buffer);

			buffer = it->sMinMaxCsrValField[2];
			buffer.Replace(_T(","), _T("."));
			buffer.Format(_T("\t\t MSB = %s\n"), buffer);
			file << CT2A(buffer);

			file << "\t END_VALDESCR\n";
		}

		if (!it->sCommentField.IsEmpty())	// ������ ������������
		{
			file << "\t COMMENT\n";
	
			buffer = it->sCommentField;
			buffer.Replace(_T("\""), _T("\\\""));
			buffer.Replace(_T("\n"), _T("\"\n\t\t\""));
			buffer.Format(_T("\t\t\"%s\"\n"), buffer);

			file << CT2A(buffer);
			file << "\t END_COMMENT\n";
		}
		file << "END_PAR \n\n";
	}
}