#pragma once

#include "sqlite3.h"
#include "common.h"

class CMySqliteDatabase {
public:
	CMySqliteDatabase();
	int InitDb();
	int DeinitDb();

	int  SaveResult(const CSaveResultParam * pParam);
	int  QueryData(const CQueryDataParam * pParam, std::vector<ReaderItem*> & vRet);

private:
	// szTableName: Ҫ�����ı���
	// szSql: create table (...)���������������
	BOOL  CreateTable(const char * szTableName, const char * szSql);

private:
	sqlite3 *                         m_db;
};