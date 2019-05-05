#pragma once

#include "sqlite3.h"
#include "common.h"

class CMySqliteDatabase  {
public:
	CMySqliteDatabase();
	int InitDb();
	int DeinitDb();

	void  SaveSurTemp(CSaveSurTempParam * pParam);

private:
	sqlite3 *                         m_db;

private:
	// szTableName: Ҫ�����ı���
	// szSql: create table (...)���������������
	void  CreateTable(const char * szTableName, const char * szSql);
	// ɾ����ʱ���¶����ݣ�Tag����
	void  PruneOldData();
};