#pragma once

#include "sqlite3.h"

class CMySqliteDatabase  {
public:
	CMySqliteDatabase();
	int InitDb();
	int DeinitDb();

private:
	sqlite3 *           m_db;
};