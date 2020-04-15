#pragma once

#include "sqlite3.h"
#include "common.h"

class CMySqliteDatabase {
public:
	CMySqliteDatabase();
	int InitDb();
	int DeinitDb();

	BOOL AddClass(const CAddClassParam * pParam);
	void GetAllClasses(std::vector<DWORD> & vRet);
	void GetRoomData(const CGetRoomParam * pParam, std::vector<DeskItem*> & vRet);
	void ModifyDesk(const CModifyDeskParam * pParam);
	void EmptyDesk(const CEmptyDeskParam * pParam);

private:
	// szTableName: 要创建的表名
	// szSql: create table (...)，括号里面的内容
	BOOL  CreateTable(const char * szTableName, const char * szSql);

private:
	sqlite3 *                         m_db;
};