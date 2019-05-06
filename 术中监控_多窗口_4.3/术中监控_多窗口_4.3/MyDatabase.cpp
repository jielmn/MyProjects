#include "MyDatabase.h"

#define  DB_NAME                          "temp_monitor.db"

/******* 温度表 *****/
#define  TEMP_TABLE                       "temperature"
/*
	id         INTEGER        PRIMARY KEY     AUTOINCREMENT
	tag_id     CHAR(16)       NOT NULL
	temp       int            NOT NULL
	time       int            NOT NULL
	remark     varchar(28)    NOT NULL
	type       int            NOT NULL        DEFAULT 0            **** 0: 连续测温，1: 手持读卡器测温
	readerid   CHAR(16)       NOT NULL
*/

// 温度贴
#define  TAGS_TABLE                      "tags"
/*
	tag_id         CHAR(16)           NOT NULL           PRIMARY KEY
	patient_name   VARCHAR(16)        NOT NULL
	time           int                NOT NULL
*/

// 窗格绑定
#define  GRID_BINDING_TABLE              "gridbinding"
/*
	grid_index     int         NOT NULL        PRIMARY KEY
	tag_id         CHAR(16)    NOT NULL
*/

CMySqliteDatabase::CMySqliteDatabase() {
	m_db = 0;
}

int CMySqliteDatabase::InitDb() {
	int ret = sqlite3_open(DB_NAME, &m_db);
	if ( ret != 0 ) {
		return -1;
	}

	// 创建温度表
	CreateTable( TEMP_TABLE,
		"id         INTEGER        PRIMARY KEY     AUTOINCREMENT," \
		"tag_id     CHAR(16)       NOT NULL," \
		"temp       int            NOT NULL," \
		"time       int            NOT NULL," \
		"remark     varchar(28)    NOT NULL," \
		"type       int            NOT NULL        DEFAULT 0," \
	    "readerid   CHAR(16)       NOT NULL" );


	CreateTable(TAGS_TABLE,
		"tag_id         CHAR(16)           NOT NULL           PRIMARY KEY," \
		"patient_name   VARCHAR(16)        NOT NULL," \
		"time           int                NOT NULL" );

	CreateTable(GRID_BINDING_TABLE,
		"grid_index     int         NOT NULL        PRIMARY KEY," \
		"tag_id         CHAR(16)    NOT NULL" );

	// 删除过时的旧数据
	PruneOldData();

	return 0;
}

int CMySqliteDatabase::DeinitDb() {
	return sqlite3_close(m_db);
}

// szTableName: 要创建的表名
// szSql: create table (...)，括号里面的内容
void  CMySqliteDatabase::CreateTable( const char * szTableName, const char * szSql ) {
	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	//char *zErrMsg = 0;            // 错误描述
	char buf[8192];

	// 查看温度表是否存在，如果不存在，则创建
	SNPRINTF(buf, sizeof(buf), "select name from sqlite_master where name = '%s';", szTableName);
	sqlite3_get_table( m_db, buf, &azResult, &nrow, &ncolumn, 0 );

	// 表不存在，则创建表
	if ( 0 == nrow ) {
		SNPRINTF(buf, sizeof(buf), "CREATE TABLE %s (%s);", szTableName, szSql);
		int ret = sqlite3_exec(m_db, buf, 0, 0, 0);
		if (ret != 0) {
			sqlite3_free_table(azResult);
			return;
		}
	}
	sqlite3_free_table(azResult);
}

// 删除过时的温度数据，Tag数据
void  CMySqliteDatabase::PruneOldData() {
	char *zErrMsg = 0;
	// 删除一周前的温度
	char szSql[8192];
	time_t today_zero_time = GetTodayZeroTime();
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 6;  // 一周前的开始位置

	// 删除一周前的温度数据
	SNPRINTF(szSql, sizeof(szSql), "delete from %s where time < %lu", TEMP_TABLE, (DWORD)tWeekBegin);
	int ret = sqlite3_exec( m_db, szSql, 0, 0, &zErrMsg);
	assert(0 == ret);

	// 删除三个月前的tag数据
	time_t tThreeMonthAgo = today_zero_time - 3600 * 24 * 89;
	SNPRINTF(szSql, sizeof(szSql), "delete from %s where time < %lu", TAGS_TABLE, (DWORD)tThreeMonthAgo);
	ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
	assert(0 == ret);
}

void  CMySqliteDatabase::SaveSurTemp(CSaveSurTempParam * pParam) {
	char sql[8192];
	int  ret = 0;
	int  nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;           // 二维数组存放结果

	SNPRINTF(sql, sizeof(sql), "insert into %s values (null, '%s', %lu, %lu, '', 0, '%s' )", 
		TEMP_TABLE, pParam->m_item.m_szTagId, pParam->m_item.m_dwTemp, (DWORD)pParam->m_item.m_time, 
		pParam->m_item.m_szReaderId );
	ret = sqlite3_exec( m_db, sql, 0, 0, 0);
	if (0 != ret) {
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "保存数据库术中温度失败!\n");
		return;
	}

	// 获取最后插入结果的id
	SNPRINTF(sql, sizeof(sql), "select last_insert_rowid();");
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);

	if (nrow <= 0) {
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "保存数据库术中温度失败!\n");
		sqlite3_free_table(azResult);
		return;
	}

	sscanf_s( azResult[ncolumn], "%lu", &pParam->m_item.m_dwDbId );
	sqlite3_free_table(azResult);
}

void  CMySqliteDatabase::QueryTempByTag(const char * szTagId, std::vector<TempItem*> & vRet) {

	time_t today_zero_time = GetTodayZeroTime();
	// 一周前0点时分
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 6;

	char szSql[8192];
	SNPRINTF( szSql, sizeof(szSql), "select * from %s where tag_id = '%s' and time >= %lu order by time",
		      TEMP_TABLE, szTagId, (DWORD)tWeekBegin );

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	DWORD  dwValue = 0;

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		TempItem * pItem = new TempItem;
		memset(pItem, 0, sizeof(TempItem));

		sscanf_s(azResult[(i + 1)*ncolumn + 0], "%lu", &pItem->m_dwDbId);
		sscanf_s(azResult[(i + 1)*ncolumn + 2], "%lu", &pItem->m_dwTemp);
		sscanf_s(azResult[(i + 1)*ncolumn + 3], "%lu", &dwValue);
		pItem->m_time = (time_t)dwValue;
		strncpy_s(pItem->m_szRemark, azResult[(i + 1)*ncolumn + 4], sizeof(pItem->m_szRemark));

		vRet.push_back(pItem);
	}
	sqlite3_free_table(azResult);
}