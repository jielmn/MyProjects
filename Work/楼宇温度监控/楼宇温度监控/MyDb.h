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
	// szTableName: Ҫ�����ı���
	// szSql: create table (...)���������������
	BOOL  CreateTable(const char * szTableName, const char * szSql);

private:
	sqlite3 *                         m_db;
};