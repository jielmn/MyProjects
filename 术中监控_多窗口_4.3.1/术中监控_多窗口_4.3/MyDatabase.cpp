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
#define  PATIENT_INFO                   "patientinfo"

// 病人的非温度数据
#define  PATIENT_DATA                   "patientdata"


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

	CreateTable(PATIENT_INFO, 
		"tag_id        CHAR(16)    NOT NULL         PRIMARY KEY," \
		"sex           int," \
	    "age           int," \
	    "outpatient_no varchar(20)," \
	    "hospital_admission_no varchar(20), " \
	    "in_hospital_date int," \
	    "medical_department varchar(20), " \
	    "ward varchar(20), " \
	    "bed_no varchar(20), " \
	    "surgery_date int" );

	CreateTable(PATIENT_DATA, 
		"tag_id      CHAR(16)     NOT NULL, " \
	    "date        int          NOT NULL, " \
	    "pulse_1     int, " \
		"pulse_2     int, " \
		"pulse_3     int, " \
		"pulse_4     int, " \
		"pulse_5     int, " \
		"pulse_6     int, " \
		"breath_1    int, " \
		"breath_2    int, " \
		"breath_3    int, " \
		"breath_4    int, " \
		"breath_5    int, " \
		"breath_6    int, " \
		"defecate    int, " \
		"urine       int, " \
		"total_income  int, " \
		"total_output  int, " \
		"blood_pressure int, " \
		"weight int, " \
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
	// 删除一周前的温度
	char szSql[8192];
	time_t today_zero_time = GetTodayZeroTime();
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 6;  // 一周前的开始位置

	// 删除一周前的温度数据
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
void  CMySqliteDatabase::TagBindingGrid(const CBindingTagGrid * pParam, std::string & old_tagid) {
	char sql[8192];
	int  ret = 0;

	// 先删除tag_id对应的grid index记录
	SNPRINTF(sql, sizeof(sql), "DELETE FROM %s WHERE tag_id='%s' ", GRID_BINDING_TABLE, pParam->m_szTagId);
	sqlite3_exec(m_db, sql, 0, 0, 0);

	SNPRINTF(sql, sizeof(sql), "SELECT * FROM  %s WHERE grid_index=%d ", GRID_BINDING_TABLE, pParam->m_nGridIndex);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
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

	char sql[8192];
	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s a INNER JOIN %s b " 
		"on a.tag_id = b.tag_id WHERE a.tag_id='%s' ", PATIENT_INFO, TAGS_TABLE, pParam->m_szTagId);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	DWORD  dwValue = 0;

	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);

	// 如果存在
	if (nrow > 0) {
		STRNCPY( pRet->m_szTagId, azResult[ncolumn + 0], MAX_TAG_ID_LENGTH );

		int col = 1;
		DWORD  dwValue = 0;

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

		if (azResult[ncolumn + col])
			sscanf_s(azResult[ncolumn + col], "%lu", &dwValue);
		pRet->m_surgery = dwValue;
		col+=2;

		STRNCPY(pRet->m_szPName, azResult[ncolumn + col], MAX_TAG_PNAME_LENGTH);
	}
	sqlite3_free_table(azResult);
}