#include "MyDatabase.h"

#define  DB_NAME                          "temp_monitor.db"

/******* �¶ȱ� *****/
#define  TEMP_TABLE                       "temperature"
/*
	id         INTEGER        PRIMARY KEY     AUTOINCREMENT
	tag_id     CHAR(16)       NOT NULL
	temp       int            NOT NULL
	time       int            NOT NULL
	remark     varchar(28)    NOT NULL
	type       int            NOT NULL        DEFAULT 0            **** 0: �������£�1: �ֳֶ���������
	readerid   CHAR(16)       NOT NULL
*/

// �¶���
#define  TAGS_TABLE                      "tags"
/*
	tag_id         CHAR(16)           NOT NULL           PRIMARY KEY
	patient_name   VARCHAR(16)        NOT NULL
	time           int                NOT NULL
*/

// �����
#define  GRID_BINDING_TABLE              "gridbinding"
/*
	grid_index     int         NOT NULL        PRIMARY KEY
	tag_id         CHAR(16)    NOT NULL
*/

// ��һ�ε�����tag
#define  LAST_SUR_TAGS                   "lastsurtags"
/*
    bed_id        int          NOT NULL        PRIMARY KEY
	tag_id        CHAR(16)     NOT NULL
*/

CMySqliteDatabase::CMySqliteDatabase() {
	m_db = 0;
}

int CMySqliteDatabase::InitDb() {
	int ret = sqlite3_open(DB_NAME, &m_db);
	if ( ret != 0 ) {
		return -1;
	}

	// �����¶ȱ�
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
	

	// ɾ����ʱ�ľ�����
	PruneOldData();

	return 0;
}

int CMySqliteDatabase::DeinitDb() {
	return sqlite3_close(m_db);
}

// szTableName: Ҫ�����ı���
// szSql: create table (...)���������������
void  CMySqliteDatabase::CreateTable( const char * szTableName, const char * szSql ) {
	int nrow = 0, ncolumn = 0;    // ��ѯ�����������������
	char **azResult = 0;          // ��ά�����Ž��
	//char *zErrMsg = 0;            // ��������
	char buf[8192];

	// �鿴�¶ȱ��Ƿ���ڣ���������ڣ��򴴽�
	SNPRINTF(buf, sizeof(buf), "select name from sqlite_master where name = '%s';", szTableName);
	sqlite3_get_table( m_db, buf, &azResult, &nrow, &ncolumn, 0 );

	// �����ڣ��򴴽���
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

// ɾ����ʱ���¶����ݣ�Tag����
void  CMySqliteDatabase::PruneOldData() {
	char *zErrMsg = 0;
	// ɾ��һ��ǰ���¶�
	char szSql[8192];
	time_t today_zero_time = GetTodayZeroTime();
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 6;  // һ��ǰ�Ŀ�ʼλ��

	// ɾ��һ��ǰ���¶�����
	SNPRINTF(szSql, sizeof(szSql), "delete from %s where time < %lu", TEMP_TABLE, (DWORD)tWeekBegin);
	int ret = sqlite3_exec( m_db, szSql, 0, 0, &zErrMsg);
	assert(0 == ret);

	// ɾ��������ǰ��tag����
	//time_t tThreeMonthAgo = today_zero_time - 3600 * 24 * 89;
	//SNPRINTF(szSql, sizeof(szSql), "delete from %s where time < %lu", TAGS_TABLE, (DWORD)tThreeMonthAgo);
	//ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
	//assert(0 == ret);
}

void  CMySqliteDatabase::SaveSurTemp(CSaveSurTempParam * pParam) {
	char sql[8192];
	int  ret = 0;
	int  nrow = 0, ncolumn = 0;    // ��ѯ�����������������
	char **azResult = 0;           // ��ά�����Ž��

	SNPRINTF(sql, sizeof(sql), "insert into %s values (null, '%s', %lu, %lu, '', 0, '%s' )", 
		TEMP_TABLE, pParam->m_item.m_szTagId, pParam->m_item.m_dwTemp, (DWORD)pParam->m_item.m_time, 
		pParam->m_item.m_szReaderId );
	ret = sqlite3_exec( m_db, sql, 0, 0, 0);
	if (0 != ret) {
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "�������ݿ������¶�ʧ��!\n");
		return;
	}

	// ��ȡ����������id
	SNPRINTF(sql, sizeof(sql), "select last_insert_rowid();");
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);

	if (nrow <= 0) {
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "�������ݿ������¶�ʧ��!\n");
		sqlite3_free_table(azResult);
		return;
	}

	sscanf_s( azResult[ncolumn], "%lu", &pParam->m_item.m_dwDbId );
	sqlite3_free_table(azResult);
}

// �����ֳ��¶�����
void  CMySqliteDatabase::SaveHandTemp(CSaveHandTempParam * pParam) {
	char sql[8192];
	int  ret = 0;
	int  nrow = 0, ncolumn = 0;    // ��ѯ�����������������
	char **azResult = 0;           // ��ά�����Ž��

	SNPRINTF(sql, sizeof(sql), "insert into %s values (null, '%s', %lu, %lu, '', 1, '%s' )",
		TEMP_TABLE, pParam->m_item.m_szTagId, pParam->m_item.m_dwTemp, (DWORD)pParam->m_item.m_time,
		pParam->m_item.m_szReaderId);
	ret = sqlite3_exec(m_db, sql, 0, 0, 0);
	if (0 != ret) {
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "�������ݿ������¶�ʧ��!\n");
		return;
	}

	// ��ȡ����������id
	SNPRINTF(sql, sizeof(sql), "select last_insert_rowid();");
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);

	if (nrow <= 0) {
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "�������ݿ������¶�ʧ��!\n");
		sqlite3_free_table(azResult);
		return;
	}

	sscanf_s(azResult[ncolumn], "%lu", &pParam->m_item.m_dwDbId);
	sqlite3_free_table(azResult);
}

void  CMySqliteDatabase::QueryTempByTag(const char * szTagId, std::vector<TempItem*> & vRet) {

	time_t today_zero_time = GetTodayZeroTime();
	// һ��ǰ0��ʱ��
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 6;

	char szSql[8192];
	SNPRINTF( szSql, sizeof(szSql), "select * from %s where tag_id = '%s' and time >= %lu order by time",
		      TEMP_TABLE, szTagId, (DWORD)tWeekBegin );

	int nrow = 0, ncolumn = 0;    // ��ѯ�����������������
	char **azResult = 0;          // ��ά�����Ž��
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

// ����ע��
void  CMySqliteDatabase::SaveRemark(const CSaveRemarkParam * pParam) {
	char sql[8192];
	char szRemark[256];
	StrReplaceAll(szRemark, sizeof(szRemark), pParam->m_szRemark, "'", "''");

	SNPRINTF( sql, sizeof(sql), "UPDATE %s set remark='%s' where id=%lu", TEMP_TABLE, szRemark, pParam->m_dwDbId );
	int ret = sqlite3_exec(m_db, sql, 0, 0, 0);
	if (0 != ret) {
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "����ע�͵����ݿ�ʧ��!\n");
		return;
	}
}

// ����TagID��ѯTag�Ĳ�������
void  CMySqliteDatabase::QueryTagPNameByTagId(const char * szTagId, char * szPName, DWORD  dwPNameLen) {
	assert(szPName && dwPNameLen > 0);

	char szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "select * from %s where tag_id = '%s' ", TAGS_TABLE, szTagId );

	int nrow = 0, ncolumn = 0;    // ��ѯ�����������������
	char **azResult = 0;          // ��ά�����Ž��
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

// ������������tag id
void CMySqliteDatabase::SaveLastSurTagId(const CSaveLastSurTagId * pParam) {

	char szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "select * from %s where bed_id = %lu", LAST_SUR_TAGS, (DWORD)pParam->m_wBedId );

	int nrow = 0, ncolumn = 0;    // ��ѯ�����������������
	char **azResult = 0;          // ��ά�����Ž��
	DWORD  dwValue = 0;

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	sqlite3_free_table(azResult);

	// ����м�¼
	if ( nrow > 0 ) {
		SNPRINTF(szSql, sizeof(szSql), "UPDATE %s set tag_id = '%s' WHERE bed_id = %lu ",
			LAST_SUR_TAGS, pParam->m_szTagId, (DWORD)pParam->m_wBedId);
		int ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
		if (0 != ret) {
			g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "������������tagidʧ��!\n");
			return;
		}
	}
	// û�м�¼ 
	else {
		SNPRINTF(szSql, sizeof(szSql), "INSERT INTO %s VALUES(%lu,'%s') ",
			LAST_SUR_TAGS, (DWORD)pParam->m_wBedId, pParam->m_szTagId);
		int ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
		if (0 != ret) {
			g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "������������tagidʧ��!\n");
			return;
		}
	}
	
}

// ��ȡ���һ�ε�����TagId
void CMySqliteDatabase::GetAllLastSurTags(std::vector<LastSurTagItem *> & vRet) {
	char szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "select * from %s", LAST_SUR_TAGS);

	int nrow = 0, ncolumn = 0;    // ��ѯ�����������������
	char **azResult = 0;          // ��ά�����Ž��
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

// ��ȡ���е��ֳ�Tag�¶�����
void  CMySqliteDatabase::GetAllHandTagTempData(std::vector<HandTagResult *> & vHandTagRet) {
	time_t today_zero_time = GetTodayZeroTime();
	// һ��ǰ0��ʱ��
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 6;

	char szSql[8192];
	SNPRINTF( szSql, sizeof(szSql), "SELECT tag_id FROM %s WHERE time >= %lu AND type = 1 GROUP BY tag_id ORDER BY time DESC ",
		      TEMP_TABLE, (DWORD)tWeekBegin );

	int nrow = 0, ncolumn = 0;    // ��ѯ�����������������
	char **azResult = 0;          // ��ά�����Ž��
	//char *szErrMsg = 0;         // ��������
	DWORD  dwValue  = 0;


	/*****  ��ȡ���е��ֳ�tag id  *****/
	int ret = sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		HandTagResult * pItem = new HandTagResult;
		memset(pItem, 0, sizeof(HandTagResult));

		STRNCPY(pItem->m_szTagId, azResult[(i + 1)*ncolumn + 0], MAX_TAG_ID_LENGTH);
		vHandTagRet.push_back(pItem);

		QueryTagPNameByTagId(pItem->m_szTagId, pItem->m_szTagPName, MAX_TAG_PNAME_LENGTH);
	}
	sqlite3_free_table(azResult);

	/*****  ��ȡ���е��ֳ�tag�¶�����  *****/
	std::vector<HandTagResult *>::iterator  it;
	for ( it = vHandTagRet.begin(); it != vHandTagRet.end(); ++it ) {
		HandTagResult * pItem = *it;

		char szSql[8192];
		SNPRINTF( szSql, sizeof(szSql), "SELECT * FROM %s WHERE time >= %lu AND tag_id='%s' ORDER BY time",
			      TEMP_TABLE, (DWORD)tWeekBegin, pItem->m_szTagId );

		nrow = 0;
		ncolumn = 0;           // ��ѯ�����������������
		azResult = 0;          // ��ά�����Ž��
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

// ����Tag�ʹ����
void  CMySqliteDatabase::TagBindingGrid(const CBindingTagGrid * pParam, std::string & old_tagid) {
	char sql[8192];
	int  ret = 0;

	// ��ɾ��tag_id��Ӧ��grid index��¼
	SNPRINTF(sql, sizeof(sql), "DELETE FROM %s WHERE tag_id='%s' ", GRID_BINDING_TABLE, pParam->m_szTagId);
	sqlite3_exec(m_db, sql, 0, 0, 0);

	SNPRINTF(sql, sizeof(sql), "SELECT * FROM  %s WHERE grid_index=%d ", GRID_BINDING_TABLE, pParam->m_nGridIndex);

	int nrow = 0, ncolumn = 0;    // ��ѯ�����������������
	char **azResult = 0;          // ��ά�����Ž��
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	// �������
	if (nrow > 0) {
		char  szTagId[MAX_TAG_ID_LENGTH];
		STRNCPY( szTagId, azResult[ncolumn + 1], MAX_TAG_ID_LENGTH );
		old_tagid = szTagId;

		SNPRINTF( sql, sizeof(sql), "UPDATE %s SET tag_id = '%s' WHERE grid_index = %d ", GRID_BINDING_TABLE,
			pParam->m_szTagId, pParam->m_nGridIndex);
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}
	// ������
	else {
		SNPRINTF(sql, sizeof(sql), "INSERT INTO %s values ( %d, '%s' ) ", GRID_BINDING_TABLE,
			pParam->m_nGridIndex, pParam->m_szTagId);
		sqlite3_exec(m_db, sql, 0, 0, 0);
		old_tagid = "";
	}
	sqlite3_free_table(azResult);
}