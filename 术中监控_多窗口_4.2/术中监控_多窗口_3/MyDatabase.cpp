#include "MyDatabase.h"
#include <time.h>

#define  DB_NAME                          "TelemedCenter.db"
#define  TEMP_TABLE_NAME                  "temperature"
// 手持读卡器
#define  TAG_NICKNAME                     "tagname_1"
// 绑定窗格
#define  BINDING_GRID                     "BindingGrid"

CMySqliteDatabase::CMySqliteDatabase() {
	m_db = 0;
}

int CMySqliteDatabase::InitDb() {
	int ret = sqlite3_open(DB_NAME, &m_db);
	if ( ret != 0 ) {
		return -1;
	}

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	char *zErrMsg = 0;            // 错误描述
	const char * sql = 0;

	// 查看温度表是否存在，如果不存在，则创建
	sql = "select name from sqlite_master where name = '" TEMP_TABLE_NAME "';";
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);

	// 表不存在，则创建表
	if (0 == nrow) {
		sql = "CREATE TABLE " TEMP_TABLE_NAME "("  \
			"id             INTEGER PRIMARY KEY AUTOINCREMENT," \
			"tag_id         CHAR(16)    NOT NULL," \
			"temp           int         NOT NULL," \
			"time           int         NOT NULL,"  \
			"remark         varchar(28) NOT NULL," \
			"type           int         NOT NULL DEFAULT 0"              // 0 连续测温，1: 手持读卡器测温
			");";

		ret = sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
		if (ret != 0) {
			sqlite3_free_table(azResult);
			return -1;
		}
	}
	sqlite3_free_table(azResult);


	nrow = 0;
	ncolumn = 0;
	azResult = 0;
	zErrMsg = 0;
	sql = "select name from sqlite_master where name = '" TAG_NICKNAME "';";
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);

	// 表不存在，则创建表
	if (0 == nrow) {
		sql = "CREATE TABLE " TAG_NICKNAME "("  \
			"tag_id         CHAR(16)    NOT NULL PRIMARY KEY," \
			"name           VARCHAR(16) NOT NULL," \
			"time           int         NOT NULL"  \
			");";

		ret = sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
		if (ret != 0) {
			sqlite3_free_table(azResult);
			return -1;
		}
	}
	sqlite3_free_table(azResult);


	nrow = 0;
	ncolumn = 0;
	azResult = 0;
	zErrMsg = 0;
	sql = "select name from sqlite_master where name = '" BINDING_GRID "';";
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);

	// 表不存在，则创建表
	if (0 == nrow) {
		sql = "CREATE TABLE " BINDING_GRID "("  \
			"grid_index     int         NOT NULL PRIMARY KEY," \
			"tag_id         CHAR(16)    NOT NULL" \
			");";

		ret = sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
		if (ret != 0) {
			sqlite3_free_table(azResult);
			return -1;
		}
	}
	sqlite3_free_table(azResult);


	// 删除一周前的温度
	char szSql[8192];
	time_t now = time(0);

	struct tm tTmTime;
	localtime_s(&tTmTime, &now);

	tTmTime.tm_hour = 0;
	tTmTime.tm_min  = 0;
	tTmTime.tm_sec  = 0;

	time_t today_zero_time = mktime(&tTmTime);

	// 一周前的开始位置
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 6;

	// 删除一周前的温度数据
	SNPRINTF(szSql, sizeof(szSql), "delete from %s where time < %lu", TEMP_TABLE_NAME, 
		(DWORD)tWeekBegin );
	sqlite3_exec(m_db, szSql, 0, 0, &zErrMsg);

	// 删除三个月前的tag name
	time_t tThreeMonthAgo = today_zero_time - 3600 * 24 * 89;
	SNPRINTF(szSql, sizeof(szSql), "delete from %s where time < %lu", TAG_NICKNAME,
		(DWORD)tThreeMonthAgo);
	sqlite3_exec(m_db, szSql, 0, 0, &zErrMsg);

	return 0;
}

int CMySqliteDatabase::DeinitDb() {
	return sqlite3_close(m_db);
}

int  CMySqliteDatabase::SaveTemp(const CSaveTempSqliteParam * pParam) {
	char sql[8192];
	char *zErrMsg = 0;
	int  ret = 0;

	SNPRINTF(sql, sizeof(sql), "insert into temperature values (null, '%s', %lu, %lu, '', 0 )",
		pParam->m_szTagId, pParam->m_dwTemp, (DWORD)pParam->m_time);
	/* Execute SQL statement */
	sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);

	return 0;
}

int  CMySqliteDatabase::QueryTempByTag(const char * szTagId, std::vector<TempData*> & vRet) {
	// 删除一周前的温度
	char szSql[8192];
	time_t now = time(0);

	struct tm tTmTime;
	localtime_s(&tTmTime, &now);

	tTmTime.tm_hour = 0;
	tTmTime.tm_min = 0;
	tTmTime.tm_sec = 0;

	time_t today_zero_time = mktime(&tTmTime);

	// 一周前的开始位置
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 6;

	SNPRINTF(szSql, sizeof(szSql), "select * from temperature where tag_id = '%s' and time >= %lu order by time",
		szTagId, (DWORD)tWeekBegin );

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	char *zErrMsg = 0;            // 错误描述
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, &zErrMsg);
	for (int i = 0; i < nrow; i++) {
		TempData * pItem = new TempData;
		memset(pItem, 0, sizeof(TempData));

		DWORD  dw;
		sscanf_s(azResult[(i + 1)*ncolumn + 2], "%lu", &pItem->dwTemperature);
		sscanf_s(azResult[(i + 1)*ncolumn + 3], "%lu", &dw);
		pItem->tTime = (time_t)dw;
		strncpy_s(pItem->szRemark, azResult[(i + 1)*ncolumn + 4], sizeof(pItem->szRemark));

		vRet.push_back(pItem);
	}
	sqlite3_free_table(azResult);

	return 0;
}

int  CMySqliteDatabase::SaveRemark(const CSetRemarkSqliteParam * pParam) {

	char sql[8192];
	char *zErrMsg = 0;
	int  ret = 0;

	char szRemark[256];
	StrReplaceAll(szRemark, sizeof(szRemark), pParam->m_szRemark, "'", "''");

	SNPRINTF(sql, sizeof(sql), "update temperature set remark='%s' "
		"where tag_id='%s' and time=%lu and type=0",
		szRemark, pParam->m_szTagId, (DWORD)pParam->m_tTime);
	/* Execute SQL statement */
	sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);

	return 0;
}

#if 0
// 按照tagid的字母顺序排列的。但是用户更希望最新的tag显示在最前面
int  CMySqliteDatabase::QueryHandReaderTemp(vector< vector<TempData *> * > & vData,
	                vector< string * >  & vTagId, vector< string * > & vTagName) {

	char szSql[8192];
	time_t now = time(0);

	struct tm tTmTime;
	localtime_s(&tTmTime, &now);

	tTmTime.tm_hour = 0;
	tTmTime.tm_min = 0;
	tTmTime.tm_sec = 0;

	time_t today_zero_time = mktime(&tTmTime);

	// 一周前的开始位置
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 6;

	SNPRINTF(szSql, sizeof(szSql), "select * from %s where time >= %lu order by tag_id, time", TEMP_TABLE_NAME_1,
		(DWORD)tWeekBegin) ;

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	char *zErrMsg = 0;            // 错误描述
	char szTagId[20] = {0};
	vector<TempData *> * pvTemp = 0;

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, &zErrMsg);
	for (int i = 0; i < nrow; i++) {

		TempData * pItem = new TempData;
		memset(pItem, 0, sizeof(TempData));

		DWORD  dw;
		sscanf_s(azResult[(i + 1)*ncolumn + 2], "%lu", &pItem->dwTemperature);    // temp
		sscanf_s(azResult[(i + 1)*ncolumn + 3], "%lu", &dw);                      // time
		pItem->tTime = (time_t)dw;
		strncpy_s(pItem->szRemark, azResult[(i + 1)*ncolumn + 4], sizeof(pItem->szRemark));   // remark
		sscanf_s(azResult[(i + 1)*ncolumn + 0], "%lu", &pItem->dwIndex);   //id

		char szCurTagId[20] = { 0 };
		STRNCPY(szCurTagId, azResult[(i + 1)*ncolumn + 1], sizeof(szCurTagId));

		// 新的一组tag
		if ( 0 != strcmp(szCurTagId, szTagId) ) {
			pvTemp = new vector<TempData *>;
			pvTemp->push_back(pItem);
			vData.push_back(pvTemp);

			string * sTagId = new string(szCurTagId);
			vTagId.push_back(sTagId);

			string * sTagName = new string;
			QueryTagNameByTagId(szCurTagId, *sTagName);
			vTagName.push_back(sTagName);

			STRNCPY(szTagId, szCurTagId, sizeof(szTagId));
		}
		else {
			assert(pvTemp);
			pvTemp->push_back(pItem);
		}
	}
	sqlite3_free_table(azResult);

	return 0;
}
#else
int  CMySqliteDatabase::QueryHandReaderTemp( vector< vector<TempData *> * > & vData,
	                                         vector< string * >  & vTagId, vector< string * > & vTagName) {

	char szSql[8192];
	time_t now = time(0);

	struct tm tTmTime;
	localtime_s(&tTmTime, &now);

	tTmTime.tm_hour = 0;
	tTmTime.tm_min = 0;
	tTmTime.tm_sec = 0;

	time_t today_zero_time = mktime(&tTmTime);

	// 一周前的开始位置
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 6;

	SNPRINTF(szSql, sizeof(szSql), "select * from %s where time >= %lu AND type = 1 order by time desc", TEMP_TABLE_NAME,
		(DWORD)tWeekBegin);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	char *zErrMsg = 0;            // 错误描述
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, &zErrMsg);
	for (int i = 0; i < nrow; i++) {
		TempData * pItem = new TempData;
		memset(pItem, 0, sizeof(TempData));

		// get record
		DWORD  dw;
		sscanf_s(azResult[(i + 1)*ncolumn + 2], "%lu", &pItem->dwTemperature);    // temp
		sscanf_s(azResult[(i + 1)*ncolumn + 3], "%lu", &dw);                      // time
		pItem->tTime = (time_t)dw;
		strncpy_s(pItem->szRemark, azResult[(i + 1)*ncolumn + 4], sizeof(pItem->szRemark));   // remark
		sscanf_s(azResult[(i + 1)*ncolumn + 0], "%lu", &pItem->dwIndex);                      //id
		char szCurTagId[20] = { 0 };
		STRNCPY(szCurTagId, azResult[(i + 1)*ncolumn + 1], sizeof(szCurTagId));    // tagid
		// end of get record

		vector< string * >::iterator it;
		for (it = vTagId.begin(); it != vTagId.end(); ++it) {
			string * sTagId = *it;
			if (0 == strcmp(sTagId->c_str(), szCurTagId)) {
				break;
			}
		}

		// 新的一组Tag
		if (it == vTagId.end()) {
			vector<TempData *> * pvTemp = new vector<TempData *>;
			pvTemp->push_back(pItem);
			vData.push_back(pvTemp);

			string * sTagId = new string(szCurTagId);
			vTagId.push_back(sTagId);

			string * sTagName = new string;
			QueryTagNameByTagId(szCurTagId, *sTagName);
			vTagName.push_back(sTagName);
		}
		// 已存在的tag
		else {
			int nIndex = it - vTagId.begin();
			vector<TempData *> * pvTemp = vData[nIndex];
			pvTemp->insert(pvTemp->begin(), pItem);
		}
	}
	sqlite3_free_table(azResult);

	return 0;
}
#endif

int  CMySqliteDatabase::QueryTagNameByTagId(const char * szTagId, string & sName) {
	assert(szTagId);

	char szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "select * from %s where tag_id = '%s'", TAG_NICKNAME, szTagId);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	char *zErrMsg = 0;            // 错误描述
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (nrow > 0) {
		char szName[256] = {0};
		STRNCPY( szName, azResult[(0 + 1)*ncolumn + 1], sizeof(szName));
		sName = szName;
	}
	sqlite3_free_table(azResult);

	return 0;
}

int  CMySqliteDatabase::SaveHandTemp(const CSaveHandTempParam * pParam) {
	char sql[8192];
	char *zErrMsg = 0;
	int  ret = 0;

	//  id,  tag, temp, time, remark, cardid
	SNPRINTF(sql, sizeof(sql), "insert into %s values (null, '%s', %lu, %lu, '', 1 )", TEMP_TABLE_NAME,
		pParam->m_szTagId, pParam->m_dwTemp, (DWORD)pParam->m_time );
	/* Execute SQL statement */
	sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);

	return 0;
}

int  CMySqliteDatabase::SaveHandTagNickname(const CSaveHandTagNicknameParam * pParam) {
	char sql[8192];
	int  ret = 0;
	time_t now = time(0);

	//  id,  name, time
	SNPRINTF(sql, sizeof(sql), "select * from %s where tag_id='%s' ", TAG_NICKNAME, pParam->m_szTagId);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	char *zErrMsg = 0;            // 错误描述
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	sqlite3_free_table(azResult);

	char  szName[256];
	StrReplaceAll(szName, sizeof(szName), pParam->m_szName, "'", "''");

	// tag_id, name, time
	// 如果存在
	if ( nrow > 0 ) {
		SNPRINTF( sql, sizeof(sql), "update %s set name = '%s' where tag_id = '%s' ", TAG_NICKNAME,
			      szName, pParam->m_szTagId );
		/* Execute SQL statement */
		sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
	}
	// 不存在
	else {
		SNPRINTF(sql, sizeof(sql), "insert into %s values ( '%s', '%s', %lu ) ", TAG_NICKNAME,
			pParam->m_szTagId, szName, (DWORD)now );
		/* Execute SQL statement */
		sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
	}

	return 0;
}

int  CMySqliteDatabase::SaveHandRemark(const CSetRemarkSqliteParam * pParam) {
	char sql[8192];
	char *zErrMsg = 0;
	int  ret = 0;

	char szRemark[256];
	StrReplaceAll(szRemark, sizeof(szRemark), pParam->m_szRemark, "'", "''");

	SNPRINTF(sql, sizeof(sql), "update %s set remark='%s' "
		"where tag_id='%s' and time=%lu and type=1", TEMP_TABLE_NAME,
		szRemark, pParam->m_szTagId, (DWORD)pParam->m_tTime);
	/* Execute SQL statement */
	sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);

	return 0;
}

int  CMySqliteDatabase::QueryTagBindingGrids(vector<TagBindingGrid*> & vRet) {

	char szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "select * from %s", BINDING_GRID );

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	char *zErrMsg = 0;            // 错误描述
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, &zErrMsg);
	for (int i = 0; i < nrow; i++) {
		TagBindingGrid * pItem = new TagBindingGrid;
		memset(pItem, 0, sizeof(TagBindingGrid));
		sscanf_s(azResult[(i + 1)*ncolumn + 0], "%d",  &pItem->m_nGridIndex);
		STRNCPY(pItem->m_szTagId, azResult[(i + 1)*ncolumn + 1], sizeof(pItem->m_szTagId));
		vRet.push_back(pItem);
	}
	sqlite3_free_table(azResult);

	return 0;
}

int  CMySqliteDatabase::SetTagBindingGrid(const CSetTagBindingGridParam * pParam) {
	char sql[8192];
	int  ret = 0;

	// grid_index, tag_id  
	SNPRINTF(sql, sizeof(sql), "update %s set tag_id = '' where tag_id='%s' ", BINDING_GRID, pParam->m_szTagId );
	sqlite3_exec(m_db, sql, 0, 0, 0);

	SNPRINTF(sql, sizeof(sql), "select * from  %s where grid_index=%d ", BINDING_GRID, pParam->m_nGridIndex);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	sqlite3_free_table(azResult);

	// 如果存在
	if (nrow > 0) {
		SNPRINTF(sql, sizeof(sql), "update %s set tag_id = '%s' where grid_index = %d ", BINDING_GRID,
			pParam->m_szTagId, pParam->m_nGridIndex );
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}
	// 不存在
	else {
		SNPRINTF(sql, sizeof(sql), "insert into %s values ( %d, '%s' ) ", BINDING_GRID,
			pParam->m_nGridIndex, pParam->m_szTagId );
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}

	return 0;
}