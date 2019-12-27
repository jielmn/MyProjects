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
	void  SaveItem(const CSaveItemParam * pParam);
	void  GetData(const CGetDataParam * pParam, std::vector<DataItem *> & vHeartBeat,
		std::vector<DataItem *> & vOxy, std::vector<DataItem *> & vTemp );

private:
	sqlite3 *                         m_db;

private:
	// szTableName: 要创建的表名
	// szSql: create table (...)，括号里面的内容
	BOOL  CreateTable(const char * szTableName, const char * szSql);
	// 删除过时的温度数据，Tag数据
	void  PruneOldData();
};