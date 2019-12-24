#pragma once

#include "sqlite3.h"
#include "common.h"

class CMySqliteDatabase  {
public:
	CMySqliteDatabase();
	int InitDb();
	int DeinitDb();

	BOOL  GetDeviceUserName(const CGetNameParam * pParam, char * szName, DWORD  dwNameSize);
	void  SaveDeviceUserName(const CSaveNameParam * pParam);

private:
	sqlite3 *                         m_db;

private:
	// szTableName: Ҫ�����ı���
	// szSql: create table (...)���������������
	BOOL  CreateTable(const char * szTableName, const char * szSql);
	// ɾ����ʱ���¶����ݣ�Tag����
	void  PruneOldData();
};