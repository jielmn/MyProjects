#include "MyDb.h"
#include <assert.h>

#define  DB_NAME                          "building_temp_monitor.db"

#define  FLOORS_TABLE                     "floors"
#define  DEVICES_TABLE                    "devices"

CMySqliteDatabase::CMySqliteDatabase() {
	m_db = 0;
}

int CMySqliteDatabase::InitDb() {
	int ret = sqlite3_open(DB_NAME, &m_db);
	if (ret != 0) {
		return -1;
	}

	CreateTable(FLOORS_TABLE,
		"id         INTEGER        NOT NULL  PRIMARY KEY");             // 楼层

	CreateTable(DEVICES_TABLE,
		"device_id    INTEGER        NOT NULL," \
		"floor_id     INTEGER        NOT NULL," \
		"address      VARCHAR(48)    NOT NULL," \
		"temperature  int            NOT NULL," \
		"time         int            NOT NULL," \
		"PRIMARY KEY(device_id)");

	return 0; 
}

int CMySqliteDatabase::DeinitDb() {
	return sqlite3_close(m_db);
}

// szTableName: 要创建的表名
// szSql: create table (...)，括号里面的内容
BOOL  CMySqliteDatabase::CreateTable(const char * szTableName, const char * szSql) {
	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	//char *zErrMsg = 0;          // 错误描述
	char buf[8192];

	// 查看温度表是否存在，如果不存在，则创建
	SNPRINTF(buf, sizeof(buf), "select name from sqlite_master where name = '%s';", szTableName);
	sqlite3_get_table(m_db, buf, &azResult, &nrow, &ncolumn, 0);

	BOOL  bCreated = FALSE;
	// 表不存在，则创建表
	if (0 == nrow) {
		SNPRINTF(buf, sizeof(buf), "CREATE TABLE %s (%s);", szTableName, szSql);
		int ret = sqlite3_exec(m_db, buf, 0, 0, 0);
		if (ret != 0) {
			sqlite3_free_table(azResult);
			return bCreated;
		}
		bCreated = TRUE;
	}
	sqlite3_free_table(azResult);
	return bCreated;
}

BOOL  CMySqliteDatabase::AddFloor(const CAddFloorParam * pParam) {
	char  szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s WHERE id=%d",
		FLOORS_TABLE, pParam->m_nFloor);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
								  //char *zErrMsg = 0;          // 错误描述
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	sqlite3_free_table(azResult);

	if (0 == nrow) {
		SNPRINTF(szSql, sizeof(szSql), "INSERT INTO %s VALUES (%d) ", FLOORS_TABLE, pParam->m_nFloor);
		sqlite3_exec(m_db, szSql, 0, 0, 0);
		return TRUE;
	}

	return FALSE;
}

void  CMySqliteDatabase::GetAllFloors(std::vector<FloorStatus *> & vRet) {
	char  szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s", FLOORS_TABLE);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		FloorStatus * pItem = new FloorStatus;
		pItem->m_nFloor = GetIntFromDb(azResult[(i + 1)*ncolumn + 0]);
		pItem->m_bWarning = GetFloorWarning(pItem->m_nFloor);
		vRet.push_back(pItem);
	}
	sqlite3_free_table(azResult);
}

void  CMySqliteDatabase::GetDevicesByFloor( const CGetDevicesParam * pParam, 
	                                        std::vector<DeviceItem*> & vRet ) {

	char  szSql[8192];
	SNPRINTF( szSql, sizeof(szSql), "SELECT * FROM %s WHERE floor_id = %d", 
		      DEVICES_TABLE, pParam->m_nFloor );

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果

	char szAddr[256];
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		DeviceItem * pItem = new DeviceItem;
		memset(pItem, 0, sizeof(DeviceItem));

		pItem->nDeviceNo = GetIntFromDb(azResult[(i + 1)*ncolumn + 0]);
		GetStrFromdDb(szAddr, sizeof(szAddr), azResult[(i + 1)*ncolumn + 2]);
		Utf8ToAnsi(pItem->szAddr, sizeof(pItem->szAddr), szAddr);
		pItem->nTemp = GetIntFromDb(azResult[(i + 1)*ncolumn + 3]);
		pItem->time  = (time_t)GetIntFromDb(azResult[(i + 1)*ncolumn + 4]);

		vRet.push_back(pItem);
	}
	sqlite3_free_table(azResult);


}

BOOL  CMySqliteDatabase::AddDevice(const CAddDeviceParam * pParam) {

	char  szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s WHERE device_id=%d",
		DEVICES_TABLE, pParam->m_nDeviceId);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
								  //char *zErrMsg = 0;          // 错误描述
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	sqlite3_free_table(azResult);

	char szAddr[256];
	char szTemp[256];
	StrReplaceAll(szTemp, sizeof(szTemp), pParam->m_szDeviceAddr, "'", "''");
	AnsiToUtf8(szAddr, sizeof(szAddr), szTemp);

	if (0 == nrow) {
		SNPRINTF( szSql, sizeof(szSql), "INSERT INTO %s VALUES (%d, %d, '%s', 0, 0) ", 
			      DEVICES_TABLE, pParam->m_nDeviceId, pParam->m_nFloor, szAddr );
		sqlite3_exec(m_db, szSql, 0, 0, 0);
		return TRUE;
	}

	return FALSE;
}

BOOL  CMySqliteDatabase::DeleteDevice(const CDelDeviceParam * pParam) {
	char  szSql[8192];
	SNPRINTF( szSql, sizeof(szSql), "DELETE FROM %s WHERE device_id=%d",
		      DEVICES_TABLE, pParam->m_nDeviceId);
	sqlite3_exec(m_db, szSql, 0, 0, 0);
	return TRUE;
}

BOOL  CMySqliteDatabase::DeleteFloor(const CDelFloorParam * pParam) {

	char  szSql[8192];
	SNPRINTF( szSql, sizeof(szSql), "DELETE FROM %s WHERE floor_id=%d",
		      DEVICES_TABLE, pParam->m_nFloor );
	sqlite3_exec(m_db, szSql, 0, 0, 0);

	SNPRINTF( szSql, sizeof(szSql), "DELETE FROM %s WHERE id=%d",
		      FLOORS_TABLE, pParam->m_nFloor);
	sqlite3_exec(m_db, szSql, 0, 0, 0);

	return TRUE;
}

BOOL  CMySqliteDatabase::SaveTemp(const TempItem * pItem, int & nFloor, BOOL & bWarning) {

	char  szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s WHERE device_id=%d",
		DEVICES_TABLE, pItem->m_nDeviceId);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	if ( nrow > 0 ) {
		nFloor = GetIntFromDb(azResult[ ncolumn + 1] );
	}
	else {
		nFloor = 0;
	}
	sqlite3_free_table(azResult);

	if ( nFloor == 0 ) {
		return FALSE;
	}

	// UPDATE
	SNPRINTF(szSql, sizeof(szSql), "UPDATE %s SET temperature=%d, time=%lu WHERE device_id=%d ",
		DEVICES_TABLE, pItem->m_dwTemp, (DWORD)pItem->m_time, pItem->m_nDeviceId );
	sqlite3_exec(m_db, szSql, 0, 0, 0);

	// 计算楼层告警
	SNPRINTF( szSql, sizeof(szSql), "SELECT * FROM %s WHERE floor_id = %d",
		      DEVICES_TABLE, nFloor );	

	nrow     = 0;
	ncolumn  = 0;
	azResult = 0;
	bWarning = FALSE;

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		int nTemp = GetIntFromDb(azResult[(i + 1)*ncolumn + 3]);
		if (nTemp >= g_data.m_nHighTemp) {
			bWarning = TRUE;
			break;
		}
	}
	sqlite3_free_table(azResult);

	return TRUE;
}

BOOL  CMySqliteDatabase::GetFloorWarning(int nFloor) {
	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	char  szSql[8192];
	BOOL bWarning = FALSE;

	// 计算楼层告警
	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s WHERE floor_id = %d",
		DEVICES_TABLE, nFloor);

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		int nTemp = GetIntFromDb(azResult[(i + 1)*ncolumn + 3]);
		if (nTemp >= g_data.m_nHighTemp) {
			bWarning = TRUE;
			break;
		}
	}
	sqlite3_free_table(azResult);

	return bWarning;
}