#include "MyDb.h"

#define  DB_NAME                          "primary_school.db"

#define  CLASSES_TABLE                    "classes"
#define  ROOMS_TABLE                      "rooms"

CMySqliteDatabase::CMySqliteDatabase() {
	m_db = 0;
}

int CMySqliteDatabase::InitDb() {
	int ret = sqlite3_open(DB_NAME, &m_db);
	if (ret != 0) {
		return -1;
	}

	CreateTable( CLASSES_TABLE,
		"id         INTEGER        NOT NULL  PRIMARY KEY" );             // 年级 << 16 | 班级

	CreateTable( ROOMS_TABLE,
		"class_id     INTEGER        NOT NULL," \
		"position     INTEGER        NOT NULL," \
		"name         VARCHAR(16)    NOT NULL," \
		"sex          int            NOT NULL," \
		"tag_id       CHAR(16)       NULL      UNIQUE," \
		"temperature  int            NOT NULL," \
		"time         int            NOT NULL," \
		"PRIMARY KEY(class_id,  position)");

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

BOOL  CMySqliteDatabase::AddClass(const CAddClassParam * pParam) {

	char  szSql[8192];
	DWORD  dwNo = MAKELONG(pParam->m_dwClass, pParam->m_dwGrade);
	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s WHERE id=%lu", 
		CLASSES_TABLE, dwNo );

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	//char *zErrMsg = 0;          // 错误描述
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	sqlite3_free_table(azResult);

	if (0 == nrow) {
		SNPRINTF(szSql, sizeof(szSql), "INSERT INTO %s VALUES (%lu) ", CLASSES_TABLE, dwNo);
		sqlite3_exec(m_db, szSql, 0, 0, 0);
		return TRUE;
	}
	// 已经存在
	else {
		return FALSE;
	}


	return FALSE;
}

void CMySqliteDatabase::GetAllClasses(std::vector<DWORD> & vRet, std::map<std::string, DWORD> & BindingTags) {
	char  szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s", CLASSES_TABLE);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	for ( int i = 0; i < nrow; i++ ) {
		DWORD dwNo = GetIntFromDb(azResult[(i + 1)*ncolumn + 0]);
		vRet.push_back(dwNo);
	}
	sqlite3_free_table(azResult);


	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s", ROOMS_TABLE);

	nrow = 0;
	ncolumn = 0;
	azResult = 0;

	BindingTags.clear();

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		char * szTagId = azResult[ncolumn + 4];
		if ( szTagId ) {
			WORD wNo  = (WORD)GetIntFromDb(azResult[ncolumn + 0]);
			WORD wPos = (WORD)GetIntFromDb(azResult[ncolumn + 1]);
			BindingTags.insert( std::make_pair(szTagId, MAKELONG(wPos, wNo)));
		}		
	}
	sqlite3_free_table(azResult);
}

void  CMySqliteDatabase::GetRoomData(const CGetRoomParam * pParam, std::vector<DeskItem*> & vRet) {
	char  szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s WHERE class_id = %lu", ROOMS_TABLE, pParam->m_dwNo);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果

	char szName[256];
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		DWORD  dwPos = GetIntFromDb(azResult[(i + 1)*ncolumn + 1]);
		DWORD  dwRow = HIWORD(dwPos);
		DWORD  dwCol = LOWORD(dwPos);

		if (0 == dwRow || dwRow > g_data.m_dwRoomRows) {
			continue;
		}

		if (0 == dwCol || dwCol > g_data.m_dwRoomCols) {
			continue;
		}

		DeskItem * pItem = new DeskItem;
		memset(pItem, 0, sizeof(DeskItem));

		pItem->nRow = dwRow;
		pItem->nCol = dwCol;
		pItem->bValid = TRUE;

		GetStrFromdDb(szName, sizeof(szName), azResult[(i + 1)*ncolumn +2]);
		Utf8ToAnsi(pItem->szName, sizeof(pItem->szName), szName);
		
		pItem->nSex = GetIntFromDb(azResult[(i + 1)*ncolumn + 3]);
		GetStrFromdDb(pItem->szTagId, sizeof(pItem->szTagId), azResult[(i + 1)*ncolumn + 4]);
		pItem->nTemp = GetIntFromDb(azResult[(i + 1)*ncolumn + 5]);
		pItem->time  = (time_t)GetIntFromDb(azResult[(i + 1)*ncolumn + 6]);

		vRet.push_back(pItem);
	}
	sqlite3_free_table(azResult);
}

void  CMySqliteDatabase::ModifyDesk(const CModifyDeskParam * pParam) {
	char  szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s WHERE class_id = %lu AND position=%lu", 
		ROOMS_TABLE, pParam->m_dwNo, pParam->m_dwPos);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	sqlite3_free_table(azResult);

	char szName[256];
	char szTemp[256];
	StrReplaceAll(szTemp, sizeof(szTemp), pParam->m_szName, "'", "''");
	AnsiToUtf8(szName, sizeof(szName), szTemp);

	// 如果有
	if (nrow > 0) {
		SNPRINTF(szSql, sizeof(szSql), "UPDATE %s SET name='%s', sex=%d, tag_id=null,"
			" temperature=0, time=0 WHERE class_id = %lu AND position=%lu", 
			ROOMS_TABLE, szName, pParam->m_nSex, pParam->m_dwNo, pParam->m_dwPos);
		sqlite3_exec(m_db, szSql, 0, 0, 0);
	}
	else {
		SNPRINTF(szSql, sizeof(szSql), "INSERT INTO %s VALUES (%d, %d, '%s', %d, null, 0, 0) ", 
			ROOMS_TABLE, pParam->m_dwNo, pParam->m_dwPos, szName, pParam->m_nSex);
		sqlite3_exec(m_db, szSql, 0, 0, 0);
	}
}

void  CMySqliteDatabase::EmptyDesk(const CEmptyDeskParam * pParam) {
	char  szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE class_id=%lu AND position=%lu",
		ROOMS_TABLE, pParam->m_dwNo, pParam->m_dwPos);
	sqlite3_exec(m_db, szSql, 0, 0, 0);
}

void  CMySqliteDatabase::DeleteClass(const CDeleteClassParam * pParam) {
	char  szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE class_id=%lu",
		ROOMS_TABLE, pParam->m_dwNo);
	sqlite3_exec(m_db, szSql, 0, 0, 0);

	SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE id=%lu",
		CLASSES_TABLE, pParam->m_dwNo);
	sqlite3_exec(m_db, szSql, 0, 0, 0);
}

void  CMySqliteDatabase::ExchangeDesk(const CExchangeDeskParam * pParam, DeskItem & desk1, DeskItem & desk2) {
	char  szSql[8192];
	int nrow = 0, ncolumn = 0;
	char **azResult = 0;      
	char  szName[256];

	memset(&desk1, 0, sizeof(DeskItem));
	memset(&desk2, 0, sizeof(DeskItem));

	// 取得第一个desk数据
	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s WHERE class_id=%lu AND position=%lu", 
		ROOMS_TABLE, pParam->m_dwNo, pParam->m_dwPos1);
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	if ( nrow > 0 ) {
		desk1.bValid = TRUE;
		GetStrFromdDb(szName, sizeof(szName), azResult[ncolumn + 2]);
		Utf8ToAnsi(desk1.szName, sizeof(desk1.szName), szName);
		desk1.nSex = GetIntFromDb(azResult[ncolumn + 3]);
		GetStrFromdDb(desk1.szTagId, sizeof(desk1.szTagId), azResult[ncolumn + 4]);
		desk1.nTemp = GetIntFromDb(azResult[ncolumn + 5]);
		desk1.time = (time_t)GetIntFromDb(azResult[ncolumn + 6]);
	}
	sqlite3_free_table(azResult);

	// 取得第二个desk数据
	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s WHERE class_id=%lu AND position=%lu",
		ROOMS_TABLE, pParam->m_dwNo, pParam->m_dwPos2);
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	if (nrow > 0) {
		desk2.bValid = TRUE;
		GetStrFromdDb(szName, sizeof(szName), azResult[ncolumn + 2]);
		Utf8ToAnsi(desk2.szName, sizeof(desk2.szName), szName);
		desk2.nSex = GetIntFromDb(azResult[ncolumn + 3]);
		GetStrFromdDb(desk2.szTagId, sizeof(desk2.szTagId), azResult[ncolumn + 4]);
		desk2.nTemp = GetIntFromDb(azResult[ncolumn + 5]);
		desk2.time = (time_t)GetIntFromDb(azResult[ncolumn + 6]);
	}
	sqlite3_free_table(azResult);

	// desk1 有记录
	if ( desk1.bValid ) {
		if ( desk2.bValid ) {
			SNPRINTF(szSql, sizeof(szSql), "UPDATE %s set position=0"
				" WHERE class_id=%lu AND position=%lu",
				ROOMS_TABLE, pParam->m_dwNo, pParam->m_dwPos1);
			sqlite3_exec(m_db, szSql, 0, 0, 0);

			SNPRINTF(szSql, sizeof(szSql), "UPDATE %s set position=%d"
				" WHERE class_id=%lu AND position=%lu",
				ROOMS_TABLE, pParam->m_dwPos1, pParam->m_dwNo, pParam->m_dwPos2);
			sqlite3_exec(m_db, szSql, 0, 0, 0);

			SNPRINTF(szSql, sizeof(szSql), "UPDATE %s set position=%d"
				" WHERE class_id=%lu AND position=0",
				ROOMS_TABLE, pParam->m_dwPos2, pParam->m_dwNo);
			sqlite3_exec(m_db, szSql, 0, 0, 0);
		}
		else {
			SNPRINTF(szSql, sizeof(szSql), "UPDATE %s set position=%d"
				" WHERE class_id=%lu AND position=%lu",
				ROOMS_TABLE, pParam->m_dwPos2, pParam->m_dwNo, pParam->m_dwPos1);
			sqlite3_exec(m_db, szSql, 0, 0, 0);
		}
	}
	// desk1 没有记录
	else {
		if ( desk2.bValid ) {
			SNPRINTF(szSql, sizeof(szSql), "UPDATE %s set position=%d"
				" WHERE class_id=%lu AND position=%lu",
				ROOMS_TABLE, pParam->m_dwPos1, pParam->m_dwNo, pParam->m_dwPos2);
			sqlite3_exec(m_db, szSql, 0, 0, 0);
		}
	}

	desk1.nRow = HIWORD(pParam->m_dwPos2);
	desk1.nCol = LOWORD(pParam->m_dwPos2);
	desk2.nRow = HIWORD(pParam->m_dwPos1);
	desk2.nCol = LOWORD(pParam->m_dwPos1);
}

void  CMySqliteDatabase::BindingTag2Desk(const CBindingTagParam * pParam, 
	                         std::map<std::string, DWORD> & BindingTags) {

	char  szSql[8192];
	int nrow = 0, ncolumn = 0;
	char **azResult = 0;
	std::map<std::string, DWORD>::iterator it;

	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s WHERE tag_id='%s'", ROOMS_TABLE, pParam->m_szTagId);
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	sqlite3_free_table(azResult);

	if ( nrow > 0 ) {
		SNPRINTF(szSql, sizeof(szSql), "UPDATE %s SET tag_id=null WHERE tag_id='%s'", 
			ROOMS_TABLE, pParam->m_szTagId);
		sqlite3_exec(m_db, szSql, 0, 0, 0);

		it = BindingTags.find(pParam->m_szTagId);
		if ( it != BindingTags.end() ) {
			BindingTags.erase(it);
		}
	}

	SNPRINTF(szSql, sizeof(szSql), "UPDATE %s SET tag_id='%s' WHERE class_id=%d AND position=%d",
		ROOMS_TABLE, pParam->m_szTagId, pParam->m_dwNo, pParam->m_dwPos);
	sqlite3_exec(m_db, szSql, 0, 0, 0);

	WORD a = (WORD)pParam->m_dwNo;
	WORD b = (WORD)pParam->m_dwPos;
	BindingTags.insert(std::make_pair(pParam->m_szTagId, MAKELONG(b,a) ));

}