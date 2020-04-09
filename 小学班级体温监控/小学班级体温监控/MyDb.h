#pragma once

#include "sqlite3.h"
#include "common.h"

class CMySqliteDatabase {
public:
	CMySqliteDatabase();
	int InitDb();
	int DeinitDb();

	BOOL AddClass(const CAddClassParam * pParam);

private:
	// szTableName: Ҫ�����ı���
	// szSql: create table (...)���������������
	BOOL  CreateTable(const char * szTableName, const char * szSql);

private:
	sqlite3 *                         m_db;
};