#pragma once

#include "sqlite3.h"
#include "common.h"

class CMySqliteDatabase  {
public:
	CMySqliteDatabase();
	int InitDb();
	int DeinitDb();

	void  SaveSurTemp(CSaveSurTempParam * pParam);
	// 根据TagID查询温度数据
	void  QueryTempByTag(const char * szTagId, std::vector<TempItem*> & vRet);

private:
	sqlite3 *                         m_db;

private:
	// szTableName: 要创建的表名
	// szSql: create table (...)，括号里面的内容
	void  CreateTable(const char * szTableName, const char * szSql);
	// 删除过时的温度数据，Tag数据
	void  PruneOldData();
};