#pragma once

#include <vector>
#include "sqlite3.h"
#include "LmnCommon.h"
#include "exhCommon.h"


class CMyDatabase  {
public:
	CMyDatabase();

	void OnInitDb( );
	void OnDeinitDb();

	int  AddTemp(DWORD dwTemp, time_t tDate);
	int  GetAllTemp(std::vector<TempData *> & v);
	
	static CMyDatabase *  GetInstance();

private:
	static CMyDatabase *  pInstance;

	sqlite3 *           m_db;
};