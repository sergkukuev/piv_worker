#include "stdafx.h"

#include <fstream>
#include "Report.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// одного 
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

// нескольких
bool CReport::GetTxt(list <bookData>& books)
{
	bool result = true;
	
	// Обход по книгам
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

// Генерация txt протоколов
void CReport::Generate(const bookData& book)
{
	logger >> L"Создание txt-файлов протокола \"" + book.name + L"\"...";
	CString path = settings.GetPath();
	path.Format(L"%s%s\\%s", path, settings.folders[text], book.name);
	if (!CreateDir(path))
		throw FolderNotCreated(path);

	// Логирование настроек для генерации текстовых данных
	CString mode;
	settings.GetGenTxt() ? mode = L"включено" : mode = L"отключено";
	logger >> L"Создание txt-файлов при наличии ошибок на листе: " + mode;
	settings.GetNumPk() ? mode = L"включено" : mode = L"отключено";
	logger >> L"Подстановка номера подкадра в номер набора: " + mode;
	// Открытие главного файла для записи
	CString filePath;
	filePath.Format(L"%s\\_ProtocolMain.txt", path);
	ofstream mainFile;
	mainFile.open(filePath);
	// Обход по страницам
	int cNP = 1, cNumWord = 1;	// CRUTCH: Отдельные счетчики для arinc сигналов для подстановки в номер слова
	for (size_t i = 0; i < book.sheets.size(); i++, cNP++, cNumWord = 1)
	{
		// Если нет на странице ошибок
		if (!book.sheets[i].error || settings.GetGenTxt())
		{
			CString name;
			sheetInfo info;
			info.arinc = book.sheets[i].arinc;
			info.arinc ? info.np = cNP : info.np = book.sheets[i].np.value;
			info.pk = book.sheets[i].pk;
			info.name = book.sheets[i].name;

			// Создание txt файла для записи
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
				WriteTxtParam(tmpFile, book.sheets[i].signals[j], info, cNumWord);	// Запись параметра
			}
			tmpFile.close();
		}
		else
			logger >> L" На листе \"" + book.sheets[i].name + L"\" имеются ошибки, txt-файл не создан";
	}
	mainFile.close();
}

// Запись одного набора данных из таблицы в txt файл
void CReport::WriteTxtParam(ofstream& file, const signalData& signal, const sheetInfo& info, const int& arincNum)
{
	CString buffer = signal.title[0];

	if (buffer.Find(RESERVE_SIGNAL) != -1 || (settings.GetProject() == project::kprno35 && dwPart::checkLow(buffer)))
		return;

	bool dwKprno = WriteParamTitle(file, signal, info);
	if (!signal.dimension.IsEmpty())
	{
		buffer.Format(L"\tUNIT=\"%s\"\n", signal.dimension);	// Размерности
		file << CT2A(buffer);
	}

	// Установка набора
	info.pk > 0 && settings.GetNumPk() ? buffer.Format(L"\tSET=%d:%d\n", info.pk, info.np) : buffer.Format(L"\tSET=%d\n", info.np);
	file << CT2A(buffer);

	// Установка адреса параметра
	paramInfo par;
	par.arinc = info.arinc;
	par.arincNum = arincNum;
	par.dwKprno = dwKprno;
	WriteParamAdr(file, signal, par);

	// Запись комментариев
	if (!signal.comment.IsEmpty() && settings.GetProject() != project::kprno35)
	{
		file << "\tCOMMENT\n";
		buffer = signal.comment;
		buffer.Replace(L"\"", L"");
		buffer.Replace(L"\n", L"\"\n\t\t\"");
		buffer.Insert(0, L"\t\t\"");
		buffer.Insert(buffer.GetLength(), L"\"\n");
		file << CT2A(buffer);
		file << "\tEND_COMMENT\n";
	}
	file << "END_PAR\n\n";
}

// Запись наименования параметра
bool CReport::WriteParamTitle(ofstream& file, const signalData& signal, const sheetInfo& info)
{
	CString buffer = signal.title[1];
	buffer.Remove(L' ');
	if (signal.repWord)	// Параметр повторяется в других листах, необходимо добавить в имя номер подкадра
	{
		int index = 1;
		if (info.pk != stats::empty && info.pk != stats::failed)
			index = info.pk;
		else if (info.np != stats::empty && info.np != stats::failed)
			index = info.np;
		buffer.Format(L"%s_%d", buffer, index);
		// Запись операции
		CString msg;
		msg.Format(L"Параметр %s (лист \"%s\") не уникален, к значению добавлено \"_%d\"", signal.title[1], info.name, index);
		logger >> msg;
	}

	buffer.Format(L"PAR=%s\n", buffer);	// Запись обозначения сигнала
	file << CT2A(buffer);

	buffer = signal.title[0];
	bool dwKprno = false;
	if (settings.GetProject() == project::kprno35)
		dwKprno = dwPart::deleleHight(buffer);
	buffer.Replace(L"\"", L"\\\"");
	buffer.Format(L"\tNAME=\"%s\"\n", buffer); // Наименования сигнала
	file << CT2A(buffer);
	return dwKprno;
}

// Запись адреса параметра
void CReport::WriteParamAdr(ofstream& file, const signalData& signal, const paramInfo& param)
{
	CString buffer;
	int max = (signal.bit.value[1] == -1 ? signal.bit.value[0] : signal.bit.value[1]);

	// Запись номера слова и битов
	if (signal.numWord.value.size() == 2)
	{
		file << "\tMERGE\n";
		buffer.Format(L"\t\tWDADDR = %d,%d,%d\n", signal.numWord.value[0], signal.bit.value[0], max);
		file << CT2A(buffer);
		max = (signal.bit.value[3] == -1) ? signal.bit.value[2] : signal.bit.value[3];
		buffer.Format(L"\t\tWDADDR = %d,%d,%d\n", signal.numWord.value[1], signal.bit.value[2], max);
		file << CT2A(buffer);
		file << "\tEND_MERGE\n";
	}
	else if (param.dwKprno && signal.part != nullptr)
	{
		file << "\tMERGE\n";
		buffer.Format(L"\t\tWDADDR = %d,%d,%d\n", signal.numWord.value[0], signal.bit.value[0], max);
		file << CT2A(buffer);
		max = (signal.part->bit.value[1] == -1) ? signal.part->bit.value[0] : signal.part->bit.value[1];
		buffer.Format(L"\t\tWDADDR = %d,%d,%d\n", signal.part->numWord.value[0], signal.part->bit.value[0], max);
		file << CT2A(buffer);
		file << "\tEND_MERGE\n";
	}
	else
	{
		param.arinc ? buffer.Format(L"\tWDADDR = %d,%d,%d\n", param.arincNum, signal.bit.value[0], max) :
			buffer.Format(L"\tWDADDR = %d,%d,%d\n", signal.numWord.value[0], signal.bit.value[0], max);
		file << CT2A(buffer);
	}

	// Мин, макс, цср
	if (signal.max.value != DBL_MIN && signal.csr.value != DBL_MIN)
	{
		file << "\tVALDESCR\n";
		signal.bitSign ? file << "\t\tSIGNED\n" : file << "\t\tUNSIGNED\n";
		IsInt(signal.min.value) ? buffer.Format(L"\t\tMIN = %.0lf\n", signal.min.value) : buffer.Format(L"\t\tMIN = %lf\n", signal.min.value);
		file << CT2A(buffer);
		IsInt(signal.max.value) ? buffer.Format(L"\t\tMAX = %.0lf\n", signal.max.value) : buffer.Format(L"\t\tMAX = %lf\n", signal.max.value);
		file << CT2A(buffer);
		IsInt(signal.csr.value) ? buffer.Format(L"\t\tMSB = %.0lf\n", signal.csr.value) : buffer.Format(L"\t\tMSB = %lf\n", signal.csr.value);
		file << CT2A(buffer);
		file << "\tEND_VALDESCR\n";
	}
}

// Проверка на int значение
bool CReport::IsInt(const double& numeric)
{
	int whole = (int)numeric;
	double fract = numeric - whole;
	bool result;
	fract == 0 ? result = true : result = false;
	return result;
}