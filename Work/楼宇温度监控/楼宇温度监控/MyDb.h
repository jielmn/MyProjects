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
	BOOL  AddDevice(const CAddDeviceParam * pParam);
	BOOL  DeleteDevice(const CDelDeviceParam * pParam);
	BOOL  DeleteFloor(const CDelFloorParam * pParam);
	BOOL  SaveTemp(const TempItem * pItem);

private:
	// szTableName: 要创建的表名
	// szSql: create table (...)，括号里面的内容
	BOOL  CreateTable(const char * szTableName, const char * szSql);

private:
	sqlite3 *                         m_db;
};