#pragma once

#include "sqlite3.h"
#include "common.h"

class CMySqliteDatabase {
public:
	CMySqliteDatabase();
	int InitDb();
	int DeinitDb();

	BOOL  AddFloor(const CAddFloorParam * pParam);
	void  GetAllFloors(std::vector<int> & vRet);
	void  GetDevicesByFloor(const CGetDevicesParam * pParam, std::vector<DeviceItem*> & vRet);

private:
	// szTableName: 要创建的表名
	// szSql: create table (...)，括号里面的内容
	BOOL  CreateTable(const char * szTableName, const char * szSql);

private:
	sqlite3 *                         m_db;
};