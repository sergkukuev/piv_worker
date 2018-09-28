#include "stdafx.h"
#include "Data.h"


CData::CData()	{	}

CData::~CData()	{	}

CString CData::Name() { return name; }

void CData::SetName(const CString& name) { this->name = name; }

std::vector<CData::Sheet>& CData::Sheets() { return this->sheets; }