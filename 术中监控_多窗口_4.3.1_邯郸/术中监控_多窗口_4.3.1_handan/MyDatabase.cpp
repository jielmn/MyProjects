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
#define  PATIENT_INFO_TABLE                   "pinfo0"

// 病人事件(手术，请假等)
#define  PATIENT_EVENT_TABLE                  "pevent"

// 病人的非温度数据
#define  PATIENT_DATA_TABLE                   "pdata"

// 全局配置数据库
#define  GLOBAL_CONFIG_TABLE                  "config"
#define  VERSION_ID                           100
#define  CUR_DB_VERSION                       1
#define  CUR_DB_DESCRIPTION                   "2.1.9"


class FindPEventObj {
public:
	FindPEventObj(int nId) : m_nId(nId) {

	}

	bool operator() (PatientEvent * p) {
		if (0 == p->m_nId)
			return false;

		if (p->m_nId == m_nId)
			return true;
		else
			return false;
	}

private:
	int     m_nId;
};

//自定义事件排序
static bool sortHand(HandTagResult * p1, HandTagResult * p2)
{
	if (p1->m_nBindingGridIndex > 0 && p2->m_nBindingGridIndex > 0) {
		return p1->m_nBindingGridIndex < p2->m_nBindingGridIndex;
	}
	else if (p1->m_nBindingGridIndex > 0) {
		return true;
	}
	else if (p2->m_nBindingGridIndex > 0) {
		return false;
	}
	else {
		return false;
	}
}


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


	BOOL  bCreateTagsTable = 
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
	    "age           varchar(20)," \
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
		"temp_1      int, " \
		"temp_2      int, " \
		"temp_3      int, " \
		"temp_4      int, " \
		"temp_5      int, " \
		"temp_6      int, " \
		"temp_1_d    int, " \
		"temp_2_d    int, " \
		"temp_3_d    int, " \
		"temp_4_d    int, " \
		"temp_5_d    int, " \
		"temp_6_d    int, " \
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
	
	int nDbVersion = GetVersion();

	// 如果tags数据库是原有的，不是此次运行时创建的
	// 需要把名字从gbk转为utf8
	if ( !bCreateTagsTable ) {
		// 没有配置数据库，需要转换的版本
		if ( nDbVersion == 0 ) {
			ConvertTagNames();
		}
	}

	BOOL  bCreateConfig =
	CreateTable(GLOBAL_CONFIG_TABLE,
		"id          INTEGER      PRIMARY KEY," \
		"param0      int          NOT NULL,"    \
		"param1      int          NOT NULL,"    \
		"param2      varchar(512) NOT NULL" );
	UpdateConfigVersion(bCreateConfig);



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
BOOL  CMySqliteDatabase::CreateTable( const char * szTableName, const char * szSql ) {
	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	//char *zErrMsg = 0;            // 错误描述
	char buf[8192];

	// 查看温度表是否存在，如果不存在，则创建
	SNPRINTF(buf, sizeof(buf), "select name from sqlite_master where name = '%s';", szTableName);
	sqlite3_get_table( m_db, buf, &azResult, &nrow, &ncolumn, 0 );

	BOOL  bCreated = FALSE;
	// 表不存在，则创建表
	if ( 0 == nrow ) {
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

// 删除过时的温度数据，Tag数据
void  CMySqliteDatabase::PruneOldData() {
	char *zErrMsg = 0;
	char szSql[8192];
	time_t today_zero_time = GetTodayZeroTime();
	time_t tThreeMonthAgo = today_zero_time - 3600 * 24 * 730;
	//time_t tThreeMonthAgo = time(0);

	// 删除3个月前的温度数据
	SNPRINTF(szSql, sizeof(szSql), "delete from %s where time < %lu", TEMP_TABLE, (DWORD)tThreeMonthAgo);
	int ret = sqlite3_exec( m_db, szSql, 0, 0, &zErrMsg);
	assert(0 == ret);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果

	// 如果tag长时间没有用，删除相关数据 1
	SNPRINTF(szSql, sizeof(szSql), "SELECT tag_id FROM %s", TAGS_TABLE);
	std::vector<std::string> vDelTag;
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		char  szTagId[MAX_TAG_ID_LENGTH];
		STRNCPY(szTagId, azResult[(i + 1)*ncolumn + 0], MAX_TAG_ID_LENGTH);
		vDelTag.push_back(szTagId);
	}
	sqlite3_free_table(azResult);

	// 如果tag长时间没有用，删除相关数据 2
	std::vector<std::string>::iterator it;
	for (it = vDelTag.begin(); it != vDelTag.end(); ++it) {
		std::string & s = *it;

		// 查看有无温度数据
		SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s WHERE"
			" tag_id='%s' limit 0,1 ", 
			TEMP_TABLE, s.c_str() );

		nrow = ncolumn = 0;    // 查询结果集的行数、列数
		azResult = 0;          // 二维数组存放结果
		sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
		sqlite3_free_table(azResult);

		// 如果没有数据，说明该tag有三个月没有使用了
		if ( nrow <= 0 ) {
			SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE tag_id = '%s'",
				PATIENT_INFO_TABLE, s.c_str());
			ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
			assert(0 == ret);

			SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE tag_id = '%s'",
				PATIENT_EVENT_TABLE, s.c_str());
			ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
			assert(0 == ret);

			SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE tag_id = '%s'",
				PATIENT_DATA_TABLE, s.c_str());
			ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
			assert(0 == ret);

			SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE tag_id = '%s'",
				TAGS_TABLE, s.c_str());
			ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
			assert(0 == ret);

			SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE tag_id = '%s'",
				GRID_BINDING_TABLE, s.c_str());
			ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
			assert(0 == ret);

			SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE tag_id = '%s'",
				LAST_SUR_TAGS, s.c_str());
			ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
			assert(0 == ret);			
		}
	}
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
		//STRNCPY( szPName, azResult[ncolumn + 1], dwPNameLen );
		Utf8ToAnsi(szPName, dwPNameLen, azResult[ncolumn + 1]);
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
		char szTagId[MAX_TAG_ID_LENGTH];
		STRNCPY(szTagId, azResult[(i + 1)*ncolumn + 1], MAX_TAG_ID_LENGTH);
		// 是否出院
		BOOL bOutHospital = IsOutHospital(szTagId);
		if (bOutHospital) {
			continue;
		}

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
		char szTagId[MAX_TAG_ID_LENGTH];
		STRNCPY(szTagId, azResult[(i + 1)*ncolumn + 0], MAX_TAG_ID_LENGTH);
		BOOL  bOutHospital = IsOutHospital(szTagId);
		// 如果出院了，就不需要了
		if ( bOutHospital ) {
			continue;
		}

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

	// 按照绑定顺序排列
	sort(vHandTagRet.begin(), vHandTagRet.end(), sortHand);
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
	char szTemp[256];
	int  ret = 0;
	time_t now = time(0);

	StrReplaceAll(szTemp, sizeof(szTemp), pParam->m_szPName, "'", "''");
	AnsiToUtf8(szName, sizeof(szName), szTemp);

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
void  CMySqliteDatabase::QueryPatientInfo(const CQueryPatientInfoParam * pParam, 
	PatientInfo * pRet, std::vector<PatientEvent * > & vEvents) {
	memset(pRet, 0, sizeof(PatientInfo));

	char sql[8192];
	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE tag_id='%s' ", 
		     PATIENT_INFO_TABLE, pParam->m_szTagId);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	DWORD  dwValue = 0;

	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);

	// 如果存在
	if (nrow > 0) {
		int col = 1;

		pRet->m_sex = GetIntFromDb(azResult[ncolumn + col]);
		col++;

		GetStrFromdDb(pRet->m_age, MAX_AGE_LENGTH, azResult[ncolumn + col]);
		col++;

		GetStrFromdDb(pRet->m_szOutpatientNo, MAX_OUTPATIENT_NO_LENGTH, azResult[ncolumn + col]);
		col++;

		GetStrFromdDb(pRet->m_szHospitalAdmissionNo, MAX_HOSPITAL_ADMISSION_NO_LENGTH, azResult[ncolumn + col]);
		col++;

		pRet->m_in_hospital = (time_t)GetIntFromDb(azResult[ncolumn + col]);
		col++;

		pRet->m_out_hospital = (time_t)GetIntFromDb(azResult[ncolumn + col]);
		col++;

		GetStrFromdDb(pRet->m_szMedicalDepartment, MAX_MEDICAL_DEPARTMENT_LENGTH, azResult[ncolumn + col]);
		col++;

		GetStrFromdDb(pRet->m_szWard, MAX_WARD_LENGTH, azResult[ncolumn + col]);
		col++;

		GetStrFromdDb(pRet->m_szBedNo, MAX_BED_NO_LENGTH, azResult[ncolumn + col]);
		col++;

		GetStrFromdDb(pRet->m_szMedicalDepartment2, MAX_MEDICAL_DEPARTMENT_LENGTH, azResult[ncolumn + col]);
		col++;

		GetStrFromdDb(pRet->m_szWard2, MAX_WARD_LENGTH, azResult[ncolumn + col]);
		col++;

		GetStrFromdDb(pRet->m_szBedNo2, MAX_BED_NO_LENGTH, azResult[ncolumn + col]);
		col++;
	}
	sqlite3_free_table(azResult);


	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE tag_id='%s' ", TAGS_TABLE, pParam->m_szTagId);
	nrow = ncolumn = 0;           // 查询结果集的行数、列数
	azResult = 0;                 // 二维数组存放结果

	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	// 如果存在
	if (nrow > 0) {
		GetStrFromdDb(pRet->m_szTagId, MAX_TAG_ID_LENGTH, azResult[ncolumn + 0]);
		//GetStrFromdDb(pRet->m_szPName, MAX_TAG_PNAME_LENGTH, azResult[ncolumn + 1]);
		Utf8ToAnsi(pRet->m_szPName, MAX_TAG_PNAME_LENGTH, azResult[ncolumn + 1]);
	}
	sqlite3_free_table(azResult);

	// 事件
	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE tag_id='%s' order by date_1 ", 
		    PATIENT_EVENT_TABLE, pParam->m_szTagId);
	nrow = ncolumn = 0;           // 查询结果集的行数、列数
	azResult = 0;                 // 二维数组存放结果
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		PatientEvent * pEvent = new PatientEvent;
		memset(pEvent, 0, sizeof(PatientEvent));
		pEvent->m_nId    = GetIntFromDb(azResult[(i + 1)*ncolumn + 0]);
		pEvent->m_nType  = GetIntFromDb(azResult[(i + 1)*ncolumn + 2]);
		pEvent->m_time_1 = (time_t)GetIntFromDb(azResult[(i + 1)*ncolumn + 3]);
		pEvent->m_time_2 = (time_t)GetIntFromDb(azResult[(i + 1)*ncolumn + 4]);
		vEvents.push_back(pEvent);
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

	StrReplaceAll(info.m_szMedicalDepartment2, MAX_MEDICAL_DEPARTMENT_LENGTH,
		pParam->m_info.m_szMedicalDepartment2, "'", "''");

	StrReplaceAll(info.m_szWard2, MAX_WARD_LENGTH, pParam->m_info.m_szWard2, "'", "''");

	StrReplaceAll(info.m_szBedNo2, MAX_BED_NO_LENGTH, pParam->m_info.m_szBedNo2, "'", "''");

	StrReplaceAll(info.m_age, MAX_AGE_LENGTH, pParam->m_info.m_age, "'", "''");


	// 如果存在
	if (nrow > 0) {
		SNPRINTF(sql, sizeof(sql), "UPDATE %s set sex=%d, age='%s', outpatient_no='%s', "
			"hospital_admission_no='%s', in_hospital_date=%lu, out_hospital_date=%lu, "
			"medical_department='%s', ward='%s', bed_no='%s', "
			"medical_department2='%s', ward2='%s', bed_no2='%s' WHERE tag_id='%s' ",
			PATIENT_INFO_TABLE, info.m_sex, info.m_age, info.m_szOutpatientNo,
			info.m_szHospitalAdmissionNo, (DWORD)info.m_in_hospital, 
			(DWORD)info.m_out_hospital, info.m_szMedicalDepartment,
			info.m_szWard, info.m_szBedNo, info.m_szMedicalDepartment2,
			info.m_szWard2, info.m_szBedNo2, info.m_szTagId);
		sqlite3_exec(m_db, sql, 0, 0, 0);		
	}
	else {
		SNPRINTF(sql, sizeof(sql), "INSERT INTO %s VALUES ('%s', %d, '%s', '%s', '%s',"
			"%lu, %lu, '%s', '%s', '%s', '%s', '%s', '%s' ); ",
			PATIENT_INFO_TABLE, info.m_szTagId, info.m_sex, info.m_age, info.m_szOutpatientNo,
			info.m_szHospitalAdmissionNo, (DWORD)info.m_in_hospital, (DWORD)info.m_out_hospital, 
			info.m_szMedicalDepartment, info.m_szWard, info.m_szBedNo,
			info.m_szMedicalDepartment2, info.m_szWard2, info.m_szBedNo2 );
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}
}

// 保存patient events
void CMySqliteDatabase::SavePatientEvents(const CSavePatientEventsParam * pParam) {
	// 原来的事件
	char sql[8192];
	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	std::vector<PatientEvent * > vOld;
	std::vector<PatientEvent * > vUpdate;
	std::vector<PatientEvent * > vDelete;
	const std::vector<PatientEvent * > & v = pParam->m_vEvents;

	SNPRINTF( sql, sizeof(sql), "SELECT * FROM %s WHERE tag_id='%s' ",
		      PATIENT_EVENT_TABLE, pParam->m_szTagId );
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		PatientEvent * pEvent = new PatientEvent;
		memset(pEvent, 0, sizeof(PatientEvent));
		pEvent->m_nId = GetIntFromDb(azResult[(i + 1)*ncolumn + 0]);
		pEvent->m_nType = GetIntFromDb(azResult[(i + 1)*ncolumn + 2]);
		pEvent->m_time_1 = (time_t)GetIntFromDb(azResult[(i + 1)*ncolumn + 3]);
		pEvent->m_time_2 = (time_t)GetIntFromDb(azResult[(i + 1)*ncolumn + 4]);
		vOld.push_back(pEvent);
	}
	sqlite3_free_table(azResult);

	std::vector<PatientEvent * >::iterator it;
	for ( it = vOld.begin(); it != vOld.end(); ++it ) {
		PatientEvent * pItem = *it;
		std::vector<PatientEvent * >::const_iterator it_find 
			= std::find_if( v.begin(), v.end(), FindPEventObj(pItem->m_nId) );
		// 没有找到
		if (it_find == v.end()) {
			vDelete.push_back(pItem);
		}
		else {
			// 查看是否要Update
			PatientEvent * pItem_1 = *it_find;
			BOOL bNeedUpdate = FALSE;
			// 如果类型不一致
			if (pItem_1->m_nType != pItem->m_nType) {
				bNeedUpdate = TRUE;
			}
			else if (pItem_1->m_time_1 != pItem->m_time_1) {
				bNeedUpdate = TRUE;
			}
			// 如果类型一致，date1一致
			else {
				// 如果是holiday类型，date2不一致，则需要更新
				if (pItem_1->m_nType == PTYPE_HOLIDAY && pItem_1->m_time_2 != pItem->m_time_2) {
					bNeedUpdate = TRUE;
				}
			}

			if (bNeedUpdate) {
				vUpdate.push_back(pItem_1);
			}
		}
	}

	// 删除
	for (it = vDelete.begin(); it != vDelete.end(); ++it) {
		PatientEvent * pItem = *it;
		SNPRINTF(sql, sizeof(sql), "DELETE FROM %s WHERE id=%lu ",
			PATIENT_EVENT_TABLE, (DWORD)pItem->m_nId );
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}
	
	// 插入
	std::vector<PatientEvent * >::const_iterator ix;
	for (ix = v.begin(); ix != v.end(); ++ix) {
		PatientEvent * pItem = *ix;
		if ( pItem->m_nId != 0 ) {
			continue;
		}

		SNPRINTF(sql, sizeof(sql), "INSERT INTO %s VALUES(null, '%s', %d, %lu, %lu) ",
			PATIENT_EVENT_TABLE, pParam->m_szTagId, pItem->m_nType, 
			(DWORD)pItem->m_time_1, (DWORD)pItem->m_time_2 );
		sqlite3_exec(m_db, sql, 0, 0, 0);		
	}

	// 更新
	for (it = vUpdate.begin(); it != vUpdate.end(); ++it) {
		PatientEvent * pItem = *it;
		SNPRINTF(sql, sizeof(sql), "UPDATE %s SET type=%d, date_1=%lu, date_2=%lu WHERE id=%lu ",
			PATIENT_EVENT_TABLE, pItem->m_nType, (DWORD)pItem->m_time_1, 
			(DWORD)pItem->m_time_2, (DWORD)pItem->m_nId );
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}

	ClearVector(vOld);
}


// 查询PatientData
void CMySqliteDatabase::QueryPatientData(const CQueryPatientDataParam * pParam, 
	                   PatientData * pData, DWORD dwSize, std::vector<TempItem *> & vTemp ) {
	assert(dwSize >= 7);
	memset(pData, 0, sizeof(PatientData) * 7);
	ClearVector(vTemp);

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

		pData[i].m_date = tDate;

		sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
		// 如果存在
		if (nrow > 0) {
			int col = 2;

			for (int j = 0; j < 6; j++) {
				pData[i].m_temp[j] = GetIntFromDb(azResult[ncolumn + col]);
				col++;
			}

			for (int j = 0; j < 6; j++) {
				pData[i].m_descend_temp[j] = GetIntFromDb(azResult[ncolumn + col]);
				col++;
			}
			
			for (int j = 0; j < 6; j++) {
				pData[i].m_pulse[j] = GetIntFromDb(azResult[ncolumn + col]);
				col++;
			}

			for (int j = 0; j < 6; j++) {
				GetStrFromdDb(pData[i].m_breath[j], MAX_BREATH_LENGTH, azResult[ncolumn + col]);
				col++;
			}

			GetStrFromdDb(pData[i].m_defecate, MAX_DEFECATE_LENGTH, azResult[ncolumn + col]);
			col++;

			GetStrFromdDb(pData[i].m_urine, MAX_URINE_LENGTH, azResult[ncolumn + col]);
			col++;

			GetStrFromdDb(pData[i].m_income, MAX_INCOME_LENGTH, azResult[ncolumn + col]);
			col++;

			GetStrFromdDb(pData[i].m_output, MAX_OUTPUT_LENGTH, azResult[ncolumn + col]);
			col++;

			GetStrFromdDb(pData[i].m_szBloodPressure, MAX_BLOOD_PRESSURE_LENGTH, azResult[ncolumn + col]);
			col++;

			GetStrFromdDb(pData[i].m_szWeight, MAX_WEIGHT_LENGTH, azResult[ncolumn + col]);
			col++;

			GetStrFromdDb(pData[i].m_szIrritability, MAX_IRRITABILITY_LENGTH, azResult[ncolumn + col]);
			col++;
		}
		sqlite3_free_table(azResult);
	}	

	// 获取温度数据(因为有降温，所以查看范围为一周零半小时)
	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE tag_id='%s' "
		"AND time>=%lu and time<%lu order by time", TEMP_TABLE, pParam->m_szTagId,
		(DWORD)tFirstZeroTime + 3600 * 2, (DWORD)(tFirstZeroTime + 3600 * 24 * 7 + 3600 * 2 + 1800));

	nrow = ncolumn = 0;
	azResult = 0;

	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		TempItem * pTemp = new TempItem;
		memset(pTemp, 0, sizeof(TempItem));

		pTemp->m_dwDbId = GetIntFromDb(azResult[(i + 1)*ncolumn + 0]);
		pTemp->m_dwTemp = GetIntFromDb(azResult[(i + 1)*ncolumn + 2]);
		pTemp->m_time   = (time_t)GetIntFromDb(azResult[(i + 1)*ncolumn + 3]);

		vTemp.push_back(pTemp);
	}
	sqlite3_free_table(azResult);
}

// 保存data
void CMySqliteDatabase::SavePatientData(const CSavePatientDataParam * pParam) {
	PatientData data;
	memcpy(&data, &pParam->m_data, sizeof(PatientData));
	data.m_date = GetAnyDayZeroTime(pParam->m_data.m_date); 

	for (int i = 0; i < 6; i++) {
		StrReplaceAll(data.m_breath[i], MAX_BREATH_LENGTH,
			pParam->m_data.m_breath[i], "'", "''");
	}

	StrReplaceAll(data.m_defecate, MAX_DEFECATE_LENGTH,
		pParam->m_data.m_defecate, "'", "''");

	StrReplaceAll(data.m_urine, MAX_URINE_LENGTH,
		pParam->m_data.m_urine, "'", "''");

	StrReplaceAll(data.m_income, MAX_INCOME_LENGTH,
		pParam->m_data.m_income, "'", "''");

	StrReplaceAll(data.m_output, MAX_OUTPUT_LENGTH,
		pParam->m_data.m_output, "'", "''");

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
	if (nrow > 0) {
		SNPRINTF(sql, sizeof(sql), "UPDATE %s set temp_1=%d, temp_2=%d, temp_3=%d, "
			"temp_4=%d, temp_5=%d, temp_6=%d, temp_1_d=%d, temp_2_d=%d, temp_3_d=%d, "
			"temp_4_d=%d, temp_5_d=%d, temp_6_d=%d, pulse_1=%d, pulse_2=%d, pulse_3=%d, "
			"pulse_4=%d, pulse_5=%d, pulse_6=%d, breath_1='%s', breath_2='%s', "
			"breath_3='%s', breath_4='%s', breath_5='%s', breath_6='%s', defecate='%s', "
			"urine='%s', total_income='%s', total_output='%s', blood_pressure='%s', "
			"weight='%s', irritability='%s' WHERE tag_id = '%s' AND date = %lu ",
			PATIENT_DATA_TABLE, data.m_temp[0], data.m_temp[1], data.m_temp[2], 
			data.m_temp[3], data.m_temp[4], data.m_temp[5], data.m_descend_temp[0], 
			data.m_descend_temp[1], data.m_descend_temp[2], data.m_descend_temp[3], 
			data.m_descend_temp[4], data.m_descend_temp[5], data.m_pulse[0], data.m_pulse[1],
			data.m_pulse[2], data.m_pulse[3], data.m_pulse[4], data.m_pulse[5], data.m_breath[0],
			data.m_breath[1], data.m_breath[2], data.m_breath[3], data.m_breath[4],
			data.m_breath[5], data.m_defecate, data.m_urine, data.m_income, data.m_output,
			data.m_szBloodPressure, data.m_szWeight, data.m_szIrritability, data.m_szTagId,
			(DWORD)data.m_date);
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}
	else {
		SNPRINTF(sql, sizeof(sql), "INSERT INTO %s VALUES ('%s', %lu, %d, %d, %d, %d, %d, %d, "
			"%d, %d, %d, %d, %d, %d,  %d, %d, %d, %d, %d, %d,"
			"'%s', '%s', '%s', '%s', '%s', '%s',  '%s', '%s', '%s', '%s', '%s', '%s', '%s' ); ",
			PATIENT_DATA_TABLE, data.m_szTagId, (DWORD)data.m_date, 
			data.m_temp[0], data.m_temp[1], data.m_temp[2], data.m_temp[3], data.m_temp[4], data.m_temp[5],
			data.m_descend_temp[0], data.m_descend_temp[1], data.m_descend_temp[2], data.m_descend_temp[3], data.m_descend_temp[4], data.m_descend_temp[5],
			data.m_pulse[0], data.m_pulse[1], data.m_pulse[2], data.m_pulse[3], data.m_pulse[4], data.m_pulse[5], 
			data.m_breath[0], data.m_breath[1], data.m_breath[2], data.m_breath[3], data.m_breath[4], data.m_breath[5], 
			data.m_defecate, data.m_urine, data.m_income, data.m_output,
			data.m_szBloodPressure, data.m_szWeight, data.m_szIrritability);
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}
}

// 删除tag
void CMySqliteDatabase::DelTag(const CDelTag * pParam) {
	char *zErrMsg = 0;
	char szSql[8192];
	int  ret = 0;

	std::string s = pParam->m_szTagId;

	SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE tag_id = '%s'",
		PATIENT_INFO_TABLE, s.c_str());
	ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
	assert(0 == ret);

	SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE tag_id = '%s'",
		PATIENT_EVENT_TABLE, s.c_str());
	ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
	assert(0 == ret);

	SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE tag_id = '%s'",
		PATIENT_DATA_TABLE, s.c_str());
	ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
	assert(0 == ret);

	SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE tag_id = '%s'",
		TAGS_TABLE, s.c_str());
	ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
	assert(0 == ret);

	SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE tag_id = '%s'",
		GRID_BINDING_TABLE, s.c_str());
	ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
	assert(0 == ret);

	SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE tag_id = '%s'",
		LAST_SUR_TAGS, s.c_str());
	ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
	assert(0 == ret);

	SNPRINTF(szSql, sizeof(szSql), "DELETE FROM %s WHERE tag_id = '%s'",
		TEMP_TABLE, s.c_str());
	ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
	assert(0 == ret);
}

// 是否出院
BOOL CMySqliteDatabase::IsOutHospital(const char * szTagId) {
	assert(szTagId);

	char sql[8192];
	SNPRINTF(sql, sizeof(sql), "SELECT out_hospital_date FROM %s WHERE tag_id='%s' ", PATIENT_INFO_TABLE, szTagId);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	DWORD  dwValue = 0;

	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	time_t t = 0;
	// 如果存在
	if (nrow > 0) {
		t = (time_t)GetIntFromDb(azResult[ncolumn + 0]);
	}
	sqlite3_free_table(azResult);

	return t > 0 ? TRUE : FALSE;
}

// 查询住院信息
void  CMySqliteDatabase::QueryInHospital( const CQueryInHospital * pParam, 
	                                      std::vector<InHospitalItem * > & vRet ) {
	CDuiString  strSql;
	CDuiString  strClause;
	CDuiString  strItem;

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果

	TQueryInHospital query;
	memcpy(&query, &pParam->m_query, sizeof(TQueryInHospital));

	char szTemp[256];
	StrReplaceAll(szTemp, sizeof(szTemp),
		           pParam->m_query.m_szPName, "'", "''" );
	AnsiToUtf8(query.m_szPName, MAX_TAG_PNAME_LENGTH, szTemp);

	StrReplaceAll(query.m_szHospitalAdmissionNo, MAX_HOSPITAL_ADMISSION_NO_LENGTH,
		pParam->m_query.m_szHospitalAdmissionNo, "'", "''");

	StrReplaceAll(query.m_szOutpatientNo, MAX_OUTPATIENT_NO_LENGTH,
		pParam->m_query.m_szOutpatientNo, "'", "''");

	StrReplaceAll(query.m_age, MAX_AGE_LENGTH,
		pParam->m_query.m_age, "'", "''");

	/* 组建where clause */
	if (query.m_szHospitalAdmissionNo[0] != '\0') {
		strItem.Format("a.hospital_admission_no like '%%%s%%'", query.m_szHospitalAdmissionNo);
		ConcatWhereClause(strClause, strItem);
	}

	if (query.m_szOutpatientNo[0] != '\0') {
		strItem.Format("a.outpatient_no like '%%%s%%'", query.m_szOutpatientNo);
		ConcatWhereClause(strClause, strItem);
	}

	if (query.m_age[0] != '\0') {
		strItem.Format("a.age like '%%%s%%'", query.m_age);
		ConcatWhereClause(strClause, strItem);
	}

	if (query.m_sex != 0) {
		strItem.Format("a.sex=%d", query.m_sex);
		ConcatWhereClause(strClause, strItem);
	}

	strItem.Format("a.in_hospital_date >= %lu", (DWORD)query.m_in_hospital_s);
	ConcatWhereClause(strClause, strItem);

	strItem.Format("a.in_hospital_date < %lu", (DWORD)query.m_in_hospital_e);
	ConcatWhereClause(strClause, strItem);

	strItem.Format("a.out_hospital_date == 0");
	ConcatWhereClause(strClause, strItem);

	if (pParam->m_query.m_szPName[0] != '\0') {
		strItem.Format("b.patient_name like '%%%s%%'", query.m_szPName);
		ConcatWhereClause(strClause, strItem);
	}
	/* END 组建 where clause */

	strSql.Format("SELECT * FROM %s a INNER JOIN %s b ON a.tag_id = b.tag_id WHERE %s",
		PATIENT_INFO_TABLE, TAGS_TABLE, strClause);

	sqlite3_get_table(m_db, strSql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		InHospitalItem * pItem = new InHospitalItem;
		memset(pItem, 0, sizeof(InHospitalItem));
		
		GetStrFromdDb(pItem->m_age, MAX_AGE_LENGTH, azResult[(i + 1)*ncolumn + 2]);
		GetStrFromdDb(pItem->m_szOutpatientNo, MAX_OUTPATIENT_NO_LENGTH, azResult[(i + 1)*ncolumn + 3]);
		GetStrFromdDb(pItem->m_szHospitalAdmissionNo, MAX_HOSPITAL_ADMISSION_NO_LENGTH, 
			          azResult[(i + 1)*ncolumn + 4]);
		pItem->m_in_hospital = (time_t)GetIntFromDb(azResult[(i + 1)*ncolumn + 5]);
		pItem->m_sex = (int)GetIntFromDb(azResult[(i + 1)*ncolumn + 1]);
		//GetStrFromdDb(pItem->m_szPName, MAX_TAG_PNAME_LENGTH, azResult[(i + 1)*ncolumn + 14]);
		Utf8ToAnsi(pItem->m_szPName, MAX_TAG_PNAME_LENGTH, azResult[(i + 1)*ncolumn + 14]);

		char szTagId[MAX_TAG_ID_LENGTH];
		GetStrFromdDb(szTagId, MAX_TAG_ID_LENGTH, azResult[(i + 1)*ncolumn + 0]);
		QueryEventsByTag(szTagId, pItem);

		vRet.push_back(pItem);
	}
	sqlite3_free_table(azResult);
}

// 拼接Where子句
void  CMySqliteDatabase::ConcatWhereClause(CDuiString & strClause, const CDuiString & strItem) {
	if (strClause.GetLength() == 0) {
		strClause = strItem;
	}
	else {
		strClause += " AND ";
		strClause += strItem;
	}
}

// 查询事件信息
void  CMySqliteDatabase::QueryEventsByTag(const char * szTagId, InHospitalItem * pRet) {
	char sql[8192];
	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果

	// 事件
	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE tag_id='%s' order by date_1 ", 
		                       PATIENT_EVENT_TABLE, szTagId );
	pRet->m_events_cnt = 0;
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		int nType = GetIntFromDb(azResult[(i + 1)*ncolumn + 2]);
		if (nType == PTYPE_SURGERY || nType == PTYPE_BIRTH || nType == PTYPE_DEATH) {
			if (pRet->m_events_cnt < MAX_QEVENTS_COUNT) {
				pRet->m_events[pRet->m_events_cnt].m_nType = nType;
				pRet->m_events[pRet->m_events_cnt].m_time_1 = (time_t)GetIntFromDb(azResult[(i + 1)*ncolumn + 3]);
				pRet->m_events_cnt++;
			}
			else {
				break;
			}
		}
	}
	sqlite3_free_table(azResult);
}

// 更新config表里的版本号
void  CMySqliteDatabase::UpdateConfigVersion(BOOL bCreated) {
	char sql[8192];
	SNPRINTF( sql, sizeof(sql), "SELECT * FROM %s WHERE id=%d ",
		GLOBAL_CONFIG_TABLE, VERSION_ID);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果

	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	sqlite3_free_table(azResult);

	// 存在
	if (nrow > 0) {
		SNPRINTF(sql, sizeof(sql), "UPDATE %s SET param0=%d, param1=%d, param2='%s' WHERE id=%d ", 
			 GLOBAL_CONFIG_TABLE, CUR_DB_VERSION, 0, CUR_DB_DESCRIPTION, VERSION_ID );
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}
	// 不存在
	else {
		SNPRINTF(sql, sizeof(sql), "INSERT INTO %s VALUES(%d, %d, %d, '%s')",
			GLOBAL_CONFIG_TABLE, VERSION_ID, CUR_DB_VERSION, 0, CUR_DB_DESCRIPTION);
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}
}

int  CMySqliteDatabase::GetVersion() {
	char sql[8192];
	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE id=%d ",
		GLOBAL_CONFIG_TABLE, VERSION_ID);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果

	int ret = sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	int version = 0;
	if ( 0 == ret && nrow > 0 ) {
		version = GetIntFromDb(azResult[ncolumn + 1]);
	}
	sqlite3_free_table(azResult);
	return version;
}

// 转换tag名称
void  CMySqliteDatabase::ConvertTagNames() {
	char sql[8192];
	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s", TAGS_TABLE);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果

	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	for ( int i = 0; i < nrow; i++ ) {
		char szTagId[MAX_TAG_ID_LENGTH];
		char szPName[MAX_TAG_PNAME_LENGTH];
		char szNewPName[256];
		char szTemp[256];

		GetStrFromdDb(szTagId, MAX_TAG_ID_LENGTH,    azResult[(i + 1)*ncolumn + 0]);
		GetStrFromdDb(szPName, MAX_TAG_PNAME_LENGTH, azResult[(i + 1)*ncolumn + 1]);

		// 如果为空字符串
		if ( szPName[0] == '\0' )
			continue;

		StrReplaceAll(szTemp, sizeof(szTemp), szPName, "'", "''");
		AnsiToUtf8(szNewPName, sizeof(szNewPName), szTemp);
		SNPRINTF(sql, sizeof(sql), "UPDATE %s set patient_name='%s' WHERE tag_id='%s'", 
			TAGS_TABLE, szNewPName, szTagId);
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}
	sqlite3_free_table(azResult);
}