#pragma once

#include "sqlite3.h"
#include "common.h"

class CMySqliteDatabase  {
public:
	CMySqliteDatabase();
	int InitDb();
	int DeinitDb();

	void  SaveSurTemp(CSaveSurTempParam * pParam);
	// 保存手持温度数据
	void  SaveHandTemp(CSaveHandTempParam * pParam);
	// 根据TagID查询温度数据
	void  QueryTempByTag(const char * szTagId, std::vector<TempItem*> & vRet);
	// 保存注释
	void  SaveRemark(const CSaveRemarkParam * pParam);
	// 根据TagID查询Tag的病人姓名
	void  QueryTagPNameByTagId(const char * szTagId, char * szPName, DWORD  dwPNameLen);

private:
	sqlite3 *                         m_db;

private:
	// szTableName: 要创建的表名
	// szSql: create table (...)，括号里面的内容
	void  CreateTable(const char * szTableName, const char * szSql);
	// 删除过时的温度数据，Tag数据
	void  PruneOldData();
};