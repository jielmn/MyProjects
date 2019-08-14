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

// 上一次的术中tag
#define  LAST_SUR_TAGS                   "lastsurtags"
/*
    bed_id        int          NOT NULL        PRIMARY KEY
	tag_id        CHAR(16)     NOT NULL
*/

// 病人基础信息(病人的id以Tag id计算)
#define  PATIENT_INFO_TABLE                   "pinfo"

// 病人事件(手术，请假等)
#define  PATIENT_EVENT_TABLE                  "pevent"

// 病人的非温度数据
#define  PATIENT_DATA_TABLE                   "pdata"


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

	CreateTable(LAST_SUR_TAGS,
		"bed_id        int          NOT NULL        PRIMARY KEY," \
		"tag_id        CHAR(16)     NOT NULL");

	CreateTable(PATIENT_INFO_TABLE,
		"tag_id        CHAR(16)    NOT NULL         PRIMARY KEY," \
		"sex           int," \
	    "age           int," \
	    "outpatient_no varchar(20)," \
	    "hospital_admission_no varchar(20), " \
	    "in_hospital_date int," \
		"out_hospital_date int," \
	    "medical_department varchar(20), " \
	    "ward varchar(20), " \
	    "bed_no varchar(20), " \
		"medical_department2 varchar(20), " \
		"ward2 varchar(20), " \
		"bed_no2 varchar(20)" );

	CreateTable(PATIENT_EVENT_TABLE,
		"id          INTEGER      PRIMARY KEY     AUTOINCREMENT," \
		"tag_id      CHAR(16)     NOT NULL, " \
		"type        int          NOT NULL," \
		"date_1      int          NOT NULL," \
		"date_2      int" );

	CreateTable(PATIENT_DATA_TABLE,
		"tag_id      CHAR(16)     NOT NULL, " \
	    "date        int          NOT NULL, " \
	    "pulse_1     int, " \
		"pulse_2     int, " \
		"pulse_3     int, " \
		"pulse_4     int, " \
		"pulse_5     int, " \
		"pulse_6     int, " \
		"breath_1    varchar(20), " \
		"breath_2    varchar(20), " \
		"breath_3    varchar(20), " \
		"breath_4    varchar(20), " \
		"breath_5    varchar(20), " \
		"breath_6    varchar(20), " \
		"defecate    varchar(20), " \
		"urine       varchar(20), " \
		"total_income  varchar(20), " \
		"total_output  varchar(20), " \
		"blood_pressure varchar(20), " \
		"weight varchar(20), " \
		"irritability varchar(20), PRIMARY KEY(tag_id, date) ");
	

	// 删除过时的旧数据
	PruneOldData();

	// 删除超出的绑定
	char sql[8192];
	SNPRINTF( sql, sizeof(sql), "DELETE FROM %s WHERE grid_index > %lu ", GRID_BINDING_TABLE, 
		      g_data.m_CfgData.m_dwLayoutGridsCnt );
	sqlite3_exec(m_db, sql, 0, 0, 0);

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
	// 删除3个月前的温度数据
	char szSql[8192];
	time_t today_zero_time = GetTodayZeroTime();
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 89;

	SNPRINTF(szSql, sizeof(szSql), "delete from %s where time < %lu", TEMP_TABLE, (DWORD)tWeekBegin);
	int ret = sqlite3_exec( m_db, szSql, 0, 0, &zErrMsg);
	assert(0 == ret);

	// 删除三个月前的tag数据
	//time_t tThreeMonthAgo = today_zero_time - 3600 * 24 * 89;
	//SNPRINTF(szSql, sizeof(szSql), "delete from %s where time < %lu", TAGS_TABLE, (DWORD)tThreeMonthAgo);
	//ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
	//assert(0 == ret);
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

// 保存手持温度数据
void  CMySqliteDatabase::SaveHandTemp(CSaveHandTempParam * pParam) {
	char sql[8192];
	int  ret = 0;
	int  nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;           // 二维数组存放结果

	SNPRINTF(sql, sizeof(sql), "insert into %s values (null, '%s', %lu, %lu, '', 1, '%s' )",
		TEMP_TABLE, pParam->m_item.m_szTagId, pParam->m_item.m_dwTemp, (DWORD)pParam->m_item.m_time,
		pParam->m_item.m_szReaderId);
	ret = sqlite3_exec(m_db, sql, 0, 0, 0);
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

	sscanf_s(azResult[ncolumn], "%lu", &pParam->m_item.m_dwDbId);
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
		//strncpy_s(pItem->m_szTagId, azResult[(i + 1)*ncolumn + 1], sizeof(pItem->m_szTagId));
		sscanf_s(azResult[(i + 1)*ncolumn + 2], "%lu", &pItem->m_dwTemp);
		sscanf_s(azResult[(i + 1)*ncolumn + 3], "%lu", &dwValue);
		pItem->m_time = (time_t)dwValue;
		strncpy_s(pItem->m_szRemark, azResult[(i + 1)*ncolumn + 4], sizeof(pItem->m_szRemark));
		strncpy_s(pItem->m_szReaderId, azResult[(i + 1)*ncolumn + 6], sizeof(pItem->m_szReaderId));

		vRet.push_back(pItem);
	}
	sqlite3_free_table(azResult);
}

// 保存注释
void  CMySqliteDatabase::SaveRemark(const CSaveRemarkParam * pParam) {
	char sql[8192];
	char szRemark[256];
	StrReplaceAll(szRemark, sizeof(szRemark), pParam->m_szRemark, "'", "''");

	SNPRINTF( sql, sizeof(sql), "UPDATE %s set remark='%s' where id=%lu", TEMP_TABLE, szRemark, pParam->m_dwDbId );
	int ret = sqlite3_exec(m_db, sql, 0, 0, 0);
	if (0 != ret) {
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "保存注释到数据库失败!\n");
		return;
	}
}

// 根据TagID查询Tag的病人姓名
void  CMySqliteDatabase::QueryTagPNameByTagId(const char * szTagId, char * szPName, DWORD  dwPNameLen) {
	assert(szPName && dwPNameLen > 0);

	char szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "select * from %s where tag_id = '%s' ", TAGS_TABLE, szTagId );

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	DWORD  dwValue = 0;

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	if ( nrow > 0 ) {
		STRNCPY( szPName, azResult[ncolumn + 1], dwPNameLen );
	}
	else {
		szPName[0] = '\0';
	}
	sqlite3_free_table(azResult);
}

// 保存最后的术中tag id
void CMySqliteDatabase::SaveLastSurTagId(const CSaveLastSurTagId * pParam) {

	char szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "select * from %s where bed_id = %lu", LAST_SUR_TAGS, (DWORD)pParam->m_wBedId );

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	DWORD  dwValue = 0;

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	sqlite3_free_table(azResult);

	// 如果有记录
	if ( nrow > 0 ) {
		SNPRINTF(szSql, sizeof(szSql), "UPDATE %s set tag_id = '%s' WHERE bed_id = %lu ",
			LAST_SUR_TAGS, pParam->m_szTagId, (DWORD)pParam->m_wBedId);
		int ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
		if (0 != ret) {
			g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "更新最后的术中tagid失败!\n");
			return;
		}
	}
	// 没有记录 
	else {
		SNPRINTF(szSql, sizeof(szSql), "INSERT INTO %s VALUES(%lu,'%s') ",
			LAST_SUR_TAGS, (DWORD)pParam->m_wBedId, pParam->m_szTagId);
		int ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
		if (0 != ret) {
			g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "更新最后的术中tagid失败!\n");
			return;
		}
	}
	
}

// 获取最后一次的术中TagId
void CMySqliteDatabase::GetAllLastSurTags(std::vector<LastSurTagItem *> & vRet) {
	char szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "select * from %s", LAST_SUR_TAGS);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	DWORD  dwValue = 0;

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		LastSurTagItem * pItem = new LastSurTagItem;
		memset(pItem, 0, sizeof(LastSurTagItem));

		DWORD  dwId;
		sscanf_s(azResult[(i + 1)*ncolumn + 0], "%lu", &dwId);
		pItem->m_wBedId = (WORD)dwId;

		STRNCPY(pItem->m_szTagId, azResult[(i + 1)*ncolumn + 1], MAX_TAG_ID_LENGTH);

		vRet.push_back(pItem);
	}
	sqlite3_free_table(azResult);
}

// 获取所有的手持Tag温度数据
void  CMySqliteDatabase::GetAllHandTagTempData(std::vector<HandTagResult *> & vHandTagRet) {
	time_t today_zero_time = GetTodayZeroTime();
	// 一周前0点时分
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 6;

	char szSql[8192];
	SNPRINTF( szSql, sizeof(szSql), "SELECT tag_id FROM %s WHERE time >= %lu AND type = 1 GROUP BY tag_id ORDER BY time DESC ",
		      TEMP_TABLE, (DWORD)tWeekBegin );

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	//char *szErrMsg = 0;         // 错误描述
	DWORD  dwValue  = 0;


	/*****  获取所有的手持tag id  *****/
	int ret = sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		HandTagResult * pItem = new HandTagResult;
		memset(pItem, 0, sizeof(HandTagResult));

		STRNCPY(pItem->m_szTagId, azResult[(i + 1)*ncolumn + 0], MAX_TAG_ID_LENGTH);
		vHandTagRet.push_back(pItem);

		QueryTagPNameByTagId(pItem->m_szTagId, pItem->m_szTagPName, MAX_TAG_PNAME_LENGTH);
		pItem->m_nBindingGridIndex = QueryBindingGridIndexByTagId(pItem->m_szTagId);
	}
	sqlite3_free_table(azResult);

	/*****  获取所有的手持tag温度数据  *****/
	std::vector<HandTagResult *>::iterator  it;
	for ( it = vHandTagRet.begin(); it != vHandTagRet.end(); ++it ) {
		HandTagResult * pItem = *it;

		char szSql[8192];
		SNPRINTF( szSql, sizeof(szSql), "SELECT * FROM %s WHERE time >= %lu AND tag_id='%s' ORDER BY time",
			      TEMP_TABLE, (DWORD)tWeekBegin, pItem->m_szTagId );

		nrow = 0;
		ncolumn = 0;           // 查询结果集的行数、列数
		azResult = 0;          // 二维数组存放结果
		pItem->m_pVec = new vector<TempItem *>;

		sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
		for (int i = 0; i < nrow; i++) {
			TempItem * pSubItem = new TempItem;
			memset(pSubItem, 0, sizeof(TempItem));

			sscanf_s(azResult[(i + 1)*ncolumn + 0], "%lu", &pSubItem->m_dwDbId);
			strncpy_s(pSubItem->m_szTagId, azResult[(i + 1)*ncolumn + 1], sizeof(pSubItem->m_szTagId));
			sscanf_s(azResult[(i + 1)*ncolumn + 2], "%lu", &pSubItem->m_dwTemp);
			sscanf_s(azResult[(i + 1)*ncolumn + 3], "%lu", &dwValue);
			pSubItem->m_time = (time_t)dwValue;
			strncpy_s(pSubItem->m_szRemark, azResult[(i + 1)*ncolumn + 4], sizeof(pSubItem->m_szRemark));
			strncpy_s(pSubItem->m_szReaderId, azResult[(i + 1)*ncolumn + 6], sizeof(pSubItem->m_szReaderId));

			pItem->m_pVec->push_back(pSubItem);
		}
		sqlite3_free_table(azResult);
	}
}

// 保存Tag和窗格绑定
void  CMySqliteDatabase::TagBindingGrid( const CBindingTagGrid * pParam, std::string & old_tagid,
	                                     int & old_grid_index ) {
	char sql[8192];
	int  ret = 0;

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果

	old_tagid = "";
	old_grid_index = 0;

	// 先删除tag_id对应的grid index记录
	//SNPRINTF(sql, sizeof(sql), "DELETE FROM %s WHERE tag_id='%s' ", GRID_BINDING_TABLE, pParam->m_szTagId);
	//sqlite3_exec(m_db, sql, 0, 0, 0);

	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE tag_id='%s' ", GRID_BINDING_TABLE, pParam->m_szTagId);
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);

	// 如果该tag已经存在绑定
	if ( nrow > 0 ) {
		sscanf_s(azResult[ncolumn + 0], "%d", &old_grid_index);
		// 一个Tag不能重新绑定同样的grid
		assert(old_grid_index != pParam->m_nGridIndex);
		if (old_grid_index == pParam->m_nGridIndex) {
			return;
		}

		// 再删除该绑定记录
		SNPRINTF(sql, sizeof(sql), "DELETE FROM %s WHERE tag_id='%s' ", GRID_BINDING_TABLE, pParam->m_szTagId);
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}

	sqlite3_free_table(azResult);
	nrow = ncolumn = 0;
	azResult = 0;


	SNPRINTF(sql, sizeof(sql), "SELECT * FROM  %s WHERE grid_index=%d ", GRID_BINDING_TABLE, pParam->m_nGridIndex);
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	// 如果存在
	if (nrow > 0) {
		char  szTagId[MAX_TAG_ID_LENGTH];
		STRNCPY( szTagId, azResult[ncolumn + 1], MAX_TAG_ID_LENGTH );
		old_tagid = szTagId;

		SNPRINTF( sql, sizeof(sql), "UPDATE %s SET tag_id = '%s' WHERE grid_index = %d ", GRID_BINDING_TABLE,
			pParam->m_szTagId, pParam->m_nGridIndex);
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}
	// 不存在
	else {
		SNPRINTF(sql, sizeof(sql), "INSERT INTO %s values ( %d, '%s' ) ", GRID_BINDING_TABLE,
			pParam->m_nGridIndex, pParam->m_szTagId);
		sqlite3_exec(m_db, sql, 0, 0, 0);
		old_tagid = "";
	}
	sqlite3_free_table(azResult);
}

// 根据TagID查询绑定的grid index(Return: 0 notfound, > 0 found)
int  CMySqliteDatabase::QueryBindingGridIndexByTagId(const char * szTagId) {
	assert(szTagId);

	char szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "select * from %s where tag_id = '%s' ", GRID_BINDING_TABLE, szTagId);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	DWORD  dwValue = 0;

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	if (nrow > 0) {
		sscanf_s(azResult[ncolumn + 0], "%lu", &dwValue);
	}
	sqlite3_free_table(azResult);

	return (int)dwValue;
}

// 删除格子的手持Tag绑定
void  CMySqliteDatabase::RemoveGridBinding(const CRemoveGridBindingParam * pParam) {
	char sql[8192];
	int  ret = 0;

	// 先删除tag_id对应的grid index记录
	SNPRINTF(sql, sizeof(sql), "DELETE FROM %s WHERE grid_index=%d ", GRID_BINDING_TABLE, pParam->m_nGridIndex);
	sqlite3_exec(m_db, sql, 0, 0, 0);
}

// 保存Tag的patient name
void  CMySqliteDatabase::SaveTagPName(const CSaveTagPNameParam * pParam) {
	char sql[8192];
	char szName[256];
	int  ret = 0;
	time_t now = time(0);

	StrReplaceAll(szName, sizeof(szName), pParam->m_szPName, "'", "''");

	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE tag_id='%s' ", TAGS_TABLE, pParam->m_szTagId);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	// 如果存在
	if (nrow > 0) {
		SNPRINTF(sql, sizeof(sql), "UPDATE %s SET patient_name = '%s', time=%lu WHERE tag_id = '%s' ", 
			TAGS_TABLE, szName, (DWORD)now, pParam->m_szTagId );
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}
	// 不存在
	else {
		SNPRINTF(sql, sizeof(sql), "INSERT INTO %s values ( '%s', '%s', %lu ) ", TAGS_TABLE,
			pParam->m_szTagId, szName, (DWORD)now );
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}
	sqlite3_free_table(azResult);
}

// 查询tag绑定的grid index
int CMySqliteDatabase::QueryBindingIndexByTag(const char * szTagId) {
	assert(szTagId);

	char sql[8192];
	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE tag_id='%s' ", GRID_BINDING_TABLE, szTagId);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	DWORD  dwValue = 0;

	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);

	// 如果存在
	if (nrow > 0) {
		sscanf_s(azResult[ncolumn], "%lu", &dwValue);
	}
	sqlite3_free_table(azResult);

	return dwValue;
}

// 查询PatientInfo
void  CMySqliteDatabase::QueryPatientInfo(const CQueryPatientInfoParam * pParam, PatientInfo * pRet) {
	memset(pRet, 0, sizeof(PatientInfo));

	char sql[8192];
	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE tag_id='%s' ", PATIENT_INFO_TABLE, pParam->m_szTagId);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	DWORD  dwValue = 0;

	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);

	// 如果存在
	if (nrow > 0) {
		int col = 1;

		if ( azResult[ncolumn + col] )
			sscanf_s( azResult[ncolumn + col], "%d", &pRet->m_sex );
		col++;

		if (azResult[ncolumn + col])
			sscanf_s(azResult[ncolumn + col], "%d", &pRet->m_age);
		col++;

		if (azResult[ncolumn + col])
			STRNCPY(pRet->m_szOutpatientNo, azResult[ncolumn + col], MAX_OUTPATIENT_NO_LENGTH);
		col++;

		if (azResult[ncolumn + col])
			STRNCPY(pRet->m_szHospitalAdmissionNo, azResult[ncolumn + col], MAX_HOSPITAL_ADMISSION_NO_LENGTH);
		col++;

		if (azResult[ncolumn + col])
			sscanf_s(azResult[ncolumn + col], "%lu", &dwValue);
		pRet->m_in_hospital = dwValue;
		col++;

		if (azResult[ncolumn + col])
			STRNCPY(pRet->m_szMedicalDepartment, azResult[ncolumn + col], MAX_MEDICAL_DEPARTMENT_LENGTH);
		col++;

		if (azResult[ncolumn + col])
			STRNCPY(pRet->m_szWard, azResult[ncolumn + col], MAX_WARD_LENGTH);
		col++;

		if (azResult[ncolumn + col])
			STRNCPY(pRet->m_szBedNo, azResult[ncolumn + col], MAX_BED_NO_LENGTH);
		col++;

		//if (azResult[ncolumn + col])
		//	sscanf_s(azResult[ncolumn + col], "%lu", &dwValue);
		//pRet->m_surgery = dwValue;
		//col++;
	}
	sqlite3_free_table(azResult);


	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE tag_id='%s' ", TAGS_TABLE, pParam->m_szTagId);
	nrow = ncolumn = 0;           // 查询结果集的行数、列数
	azResult = 0;                 // 二维数组存放结果

	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	// 如果存在
	if (nrow > 0) {
		STRNCPY(pRet->m_szTagId, azResult[ncolumn + 0], MAX_TAG_ID_LENGTH);
		STRNCPY(pRet->m_szPName, azResult[ncolumn + 1], MAX_TAG_PNAME_LENGTH);
	}
	sqlite3_free_table(azResult);
}

// 保存info
void CMySqliteDatabase::SavePatientInfo(const CSavePatientInfoParam * pParam) {
	char sql[8192];
	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE tag_id='%s' ", 
		PATIENT_INFO_TABLE, pParam->m_info.m_szTagId );

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	DWORD  dwValue = 0;

	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	sqlite3_free_table(azResult);

	PatientInfo info;
	memcpy(&info, &pParam->m_info, sizeof(PatientInfo));

	StrReplaceAll( info.m_szOutpatientNo, MAX_OUTPATIENT_NO_LENGTH, 
		           pParam->m_info.m_szOutpatientNo, "'", "''" );

	StrReplaceAll(info.m_szHospitalAdmissionNo, MAX_HOSPITAL_ADMISSION_NO_LENGTH,
		pParam->m_info.m_szHospitalAdmissionNo, "'", "''");

	StrReplaceAll(info.m_szMedicalDepartment, MAX_MEDICAL_DEPARTMENT_LENGTH,
		pParam->m_info.m_szMedicalDepartment, "'", "''");

	StrReplaceAll(info.m_szWard, MAX_WARD_LENGTH, pParam->m_info.m_szWard, "'", "''");

	StrReplaceAll(info.m_szBedNo, MAX_BED_NO_LENGTH, pParam->m_info.m_szBedNo, "'", "''");

	//// 如果存在
	//if (nrow > 0) {
	//	//SNPRINTF(sql, sizeof(sql), "UPDATE %s set sex=%d, age=%d, outpatient_no='%s', "
	//	//	"hospital_admission_no='%s', in_hospital_date=%lu, medical_department='%s', "
	//	//	"ward='%s', bed_no='%s', surgery_date=%lu WHERE tag_id='%s' ", 
	//	//	PATIENT_INFO_TABLE, info.m_sex, info.m_age, info.m_szOutpatientNo,
	//	//	info.m_szHospitalAdmissionNo, (DWORD)info.m_in_hospital, info.m_szMedicalDepartment,
	//	//	info.m_szWard, info.m_szBedNo, info.m_szTagId );
	//	sqlite3_exec(m_db, sql, 0, 0, 0);		
	//}
	//else {
	//	//SNPRINTF(sql, sizeof(sql), "INSERT INTO %s VALUES ('%s', %d, %d, '%s', '%s',"
	//	//	"%lu, '%s', '%s', '%s', %lu ); ",
	//	//	PATIENT_INFO_TABLE, info.m_szTagId, info.m_sex, info.m_age, info.m_szOutpatientNo,
	//	//	info.m_szHospitalAdmissionNo, (DWORD)info.m_in_hospital, info.m_szMedicalDepartment,
	//	//	info.m_szWard, info.m_szBedNo );
	//	sqlite3_exec(m_db, sql, 0, 0, 0);
	//}
}


// 查询PatientData
void CMySqliteDatabase::QueryPatientData(const CQueryPatientDataParam * pParam, 
	                   PatientData * pData, DWORD dwSize) {	
	assert(dwSize >= 7);
	memset(pData, 0, sizeof(PatientData) * 7);

	time_t  tFirstZeroTime = GetAnyDayZeroTime(pParam->m_tFirstDay);

	char sql[8192];
	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果

	for ( int i = 0; i < 7; i++ ) {
		time_t tDate = tFirstZeroTime + 3600 * 24 * i;

		SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE tag_id='%s' AND date=%lu", 
			PATIENT_DATA_TABLE, pParam->m_szTagId, (DWORD)(tDate) );

		nrow = ncolumn = 0;
		azResult = 0;

		sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
		// 如果存在
		if (nrow > 0) {
			int col = 2;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_pulse[0]);
			//col++;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_pulse[1]);
			//col++;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_pulse[2]);
			//col++;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_pulse[3]);
			//col++;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_pulse[4]);
			//col++;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_pulse[5]);
			//col++;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_breath[0]);
			//col++;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_breath[1]);
			//col++;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_breath[2]);
			//col++;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_breath[3]);
			//col++;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_breath[4]);
			//col++;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_breath[5]);
			//col++;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_defecate);
			//col++;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_urine);
			//col++;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_income);
			//col++;

			//if (azResult[ncolumn + col])
			//	sscanf_s(azResult[ncolumn + col], "%d", &pData[i].m_output);
			//col++;

			//if (azResult[ncolumn + col])
			//	STRNCPY(pData[i].m_szBloodPressure, azResult[ncolumn + col], MAX_BLOOD_PRESSURE_LENGTH);
			//col++;

			//if (azResult[ncolumn + col])
			//	STRNCPY(pData[i].m_szWeight, azResult[ncolumn + col], MAX_WEIGHT_LENGTH);
			//col++;

			//if (azResult[ncolumn + col])
			//	STRNCPY(pData[i].m_szIrritability, azResult[ncolumn + col], MAX_IRRITABILITY_LENGTH);
			//col++;
		}
		sqlite3_free_table(azResult);


		// 获取温度数据
		SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE tag_id='%s' "
			"AND time>=%lu and time<%lu", TEMP_TABLE, pParam->m_szTagId, 
			(DWORD)(tDate), (DWORD)(tDate+3600*24) );

		nrow = ncolumn = 0;
		azResult = 0;

		sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
		for (int j = 0; j < nrow; j++) {
			DWORD dwTemp = 0;
			DWORD dwTime = 0;

			sscanf_s(azResult[(j + 1)*ncolumn + 2], "%lu", &dwTemp);
			sscanf_s(azResult[(j + 1)*ncolumn + 3], "%lu", &dwTime);

			DWORD dwTimeDiff = dwTime - (DWORD)tDate;
			DWORD dwTimeZone = 0;
			if (dwTimeDiff > 0) {
				dwTimeZone = (dwTimeDiff - 1) / (3600 * 4);
			}

			assert(dwTimeZone < 6);
			if (dwTimeZone < 6) {
				pData[i].m_temp[dwTimeZone] = dwTemp;
			}
		}
		sqlite3_free_table(azResult);
	}	
}

// 保存data
void CMySqliteDatabase::SavePatientData(const CSavePatientDataParam * pParam) {
	PatientData data;
	memcpy(&data, &pParam->m_data, sizeof(PatientData));
	data.m_date = GetAnyDayZeroTime(pParam->m_data.m_date); 

	StrReplaceAll(data.m_szBloodPressure, MAX_BLOOD_PRESSURE_LENGTH,
		pParam->m_data.m_szBloodPressure, "'", "''");

	StrReplaceAll(data.m_szWeight, MAX_WEIGHT_LENGTH,
		pParam->m_data.m_szWeight, "'", "''");

	StrReplaceAll(data.m_szIrritability, MAX_IRRITABILITY_LENGTH, 
		pParam->m_data.m_szIrritability, "'", "''"); 

	char sql[8192];
	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE tag_id='%s' AND date=%lu ",
		PATIENT_DATA_TABLE, data.m_szTagId, (DWORD)data.m_date );

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	DWORD  dwValue = 0;

	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	sqlite3_free_table(azResult);	

	// 如果存在
	//if (nrow > 0) {
	//	SNPRINTF(sql, sizeof(sql), "UPDATE %s set pulse_1 = %d, pulse_2 = %d, pulse_3 = %d, " 
	//		"pulse_4 = %d, pulse_5 = %d, pulse_6 = %d, breath_1 = %d, breath_2 = %d, " 
	//		"breath_3 = %d, breath_4 = %d, breath_5 = %d, breath_6 = %d, defecate = %d, " 
	//		"urine = %d, total_income = %d, total_output = %d, blood_pressure = '%s', " 
	//		"weight = '%s', irritability = '%s' WHERE tag_id = '%s' AND date = %lu ",
	//		PATIENT_DATA_TABLE, data.m_pulse[0], data.m_pulse[1],
	//		data.m_pulse[2], data.m_pulse[3], data.m_pulse[4], data.m_pulse[5], data.m_breath[0],
	//		data.m_breath[1], data.m_breath[2], data.m_breath[3], data.m_breath[4],
	//		data.m_breath[5], data.m_defecate, data.m_urine, data.m_income, data.m_output,
	//		data.m_szBloodPressure, data.m_szWeight, data.m_szIrritability, data.m_szTagId,
	//		(DWORD)data.m_date );
	//	sqlite3_exec(m_db, sql, 0, 0, 0);
	//}
	//else {
	//	SNPRINTF(sql, sizeof(sql), "INSERT INTO %s VALUES ('%s', %lu, %d, %d, %d, %d, %d, %d, "
	//		"%d, %d, %d, %d, %d, %d,  %d, %d, %d, %d, '%s', '%s',  '%s' ); ",
	//		PATIENT_DATA_TABLE, data.m_szTagId, (DWORD)data.m_date, data.m_pulse[0], data.m_pulse[1],
	//		data.m_pulse[2], data.m_pulse[3], data.m_pulse[4], data.m_pulse[5], data.m_breath[0],
	//		data.m_breath[1], data.m_breath[2], data.m_breath[3], data.m_breath[4],
	//		data.m_breath[5], data.m_defecate, data.m_urine, data.m_income, data.m_output,
	//		data.m_szBloodPressure, data.m_szWeight, data.m_szIrritability );
	//	sqlite3_exec(m_db, sql, 0, 0, 0);
	//}
}