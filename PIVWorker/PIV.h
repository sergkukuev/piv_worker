#pragma once
#include "ReaderExcel.h"
#include "Test.h"

class CPIV : private CReader, private CTest
{
public:
	CPIV(const CString& path);
	~CPIV();
private:
	CString path = L"C:\\";
	CString name = L"Безымянный";
};