#include "stdafx.h"
#include "MyDatabase.h"



CMyDatabase::CMyDatabase() {
	mysql_init(&m_mysql);
	m_eStatus = DB_STATUS_CLOSED;
	m_content_board_index = 0;
}

CMyDatabase::~CMyDatabase() {
	mysql_close(&m_mysql);
}

DbStatus  CMyDatabase::GetStatus() const {
	return m_eStatus;
}

void CMyDatabase::ReconnectDb() {
	if ( m_eStatus == DB_STATUS_OPEN ) {
		return;
	}

	char szHost[256];
	char szUser[256];
	char szPwd[256];
	char szDatabase[256];
	DWORD dwPort;

	g_cfg->GetConfig("mysql host", szHost, sizeof(szHost));
	g_cfg->GetConfig("mysql user", szUser, sizeof(szUser));
	g_cfg->GetConfig("mysql pwd", szPwd, sizeof(szPwd));
	g_cfg->GetConfig("mysql database", szDatabase, sizeof(szDatabase));
	g_cfg->GetConfig("mysql port", dwPort );


	if (!mysql_real_connect(&m_mysql, szHost, szUser, szPwd, szDatabase, dwPort, 0, 0)) {
		// 没有连接上
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to connect mysql(host=%s,usr=%s,pwd=%s,database=%s,port=%u) \n",
			szHost, szUser, szPwd, szDatabase, dwPort);
		LogError();
		return;
	}

	if (0 != mysql_set_character_set(&m_mysql, "GBK")) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "faield to set database characterset! \n");
	}

	CString strSql;
	strSql.Format("select max(id) from tableinfo;");
	if (0 != mysql_query(&m_mysql, strSql)) {
		LogError();
		return;
	}

	MYSQL_RES * result = 0;
	if ( !(result = mysql_store_result(&m_mysql)) )
	{
		LogError();
		return;
	}

	MYSQL_ROW sql_row;
	sql_row = mysql_fetch_row(result);
	if (sql_row[0] == 0) {
		m_content_board_index = 0;
	}
	else {
		sscanf_s(sql_row[0], " %d", &m_content_board_index);
		m_content_board_index++;
	}
	mysql_free_result(result);

	m_eStatus = DB_STATUS_OPEN;
}

void  CMyDatabase::LogError() {
	DWORD dwErrno = mysql_errno(&m_mysql);
	const char * pErrorDescription = mysql_error(&m_mysql);
	g_log->Output(ILog::LOG_SEVERITY_ERROR, "error no = %lu, error description = %s \n", dwErrno, pErrorDescription);
}

void   CMyDatabase::Close() {
	m_eStatus = DB_STATUS_CLOSED;
}

int   CMyDatabase::UpdateFeverData(const  LmnToolkits::MessageData * pParam) {
	// 数据库没有连接上
	if (m_eStatus != DB_STATUS_OPEN) {
		return -1;
	}

	const CUpdateFeverData * pData = static_cast<const CUpdateFeverData*>(pParam);
	CString strSql;

	char buf[8192];

	time_t now = time(0);
	struct tm local_time;
	localtime_s(&local_time, &now);
	strftime(buf, sizeof(buf), "%Y-%m-%d", &local_time);

	strSql.Format("UPDATE gzqx t1 SET t1.y = (t1.y+%lu) where t1.s='1' and t1.x = '%s';", pData->m_dwFeverCount, buf);
	if (0 != mysql_query(&m_mysql, strSql)) {
		LogError();
		return -1;
	}

	// 贵阳医院
	if (0 == pData->m_dwHospitalIndex) {
		strSql.Format("UPDATE areainfo t SET t.value = 1, t.ivalue = t.ivalue + %lu, t.info = CONCAT(\"贵阳市<br>发热人数:\", t.ivalue) where t.area_id = '520100';", pData->m_dwFeverCount);
		if ( 0 != mysql_query(&m_mysql, strSql) ) {
			LogError();
			return -1;
		}

		strSql.Format("UPDATE areainfo t SET t.value = 1, t.ivalue = t.ivalue + %lu, t.info = CONCAT(\"息烽县<br>发热人数:\", t.ivalue) where t.area_id = '520122';", pData->m_dwFeverCount);
		if (0 != mysql_query(&m_mysql, strSql)) {
			LogError();
			return -1;
		}

		strSql.Format("UPDATE scattertest t SET t.type = '1', t.value = 15, t.ivalue = t.ivalue + %lu, t.info = CONCAT(t.name, \"<br>发热人数:\", t.ivalue) where t.name = '贵阳医院1';", pData->m_dwFeverCount);
		if (0 != mysql_query(&m_mysql, strSql)) {
			LogError();
			return -1;
		}

		strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &local_time);
		strSql.Format("insert into tableinfo values (%d, '', '%s 贵阳医院新上报%lu案例')", m_content_board_index, buf, pData->m_dwFeverCount );
		if (0 != mysql_query(&m_mysql, strSql)) {
			LogError();
			return -1;
		}
		m_content_board_index++;
	}
	// 六盘水医院
	else if (1 == pData->m_dwHospitalIndex) {
		strSql.Format("UPDATE areainfo t SET t.value = 1, t.ivalue = t.ivalue + %lu, t.info = CONCAT(\"六盘水市<br>发热人数:\", t.ivalue) where t.area_id = '520200';", pData->m_dwFeverCount);
		if (0 != mysql_query(&m_mysql, strSql)) {
			LogError();
			return -1;
		}

		strSql.Format("UPDATE areainfo t SET t.value = 1, t.ivalue = t.ivalue + %lu, t.info = CONCAT(\"钟山区<br>发热人数:\", t.ivalue) where t.area_id = '520201';", pData->m_dwFeverCount);
		if (0 != mysql_query(&m_mysql, strSql)) {
			LogError();
			return -1;
		}

		strSql.Format("UPDATE scattertest t SET t.type = '1', t.value = 15, t.ivalue = t.ivalue + %lu, t.info = CONCAT(t.name, \"<br>发热人数:\", t.ivalue) where t.name = '六盘水医院2';", pData->m_dwFeverCount);
		if (0 != mysql_query(&m_mysql, strSql)) {
			LogError();
			return -1;
		}

		strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &local_time);
		strSql.Format("insert into tableinfo values (%d, '', '%s 六盘水医院新上报%lu案例')", m_content_board_index, buf, pData->m_dwFeverCount );
		if (0 != mysql_query(&m_mysql, strSql)) {
			LogError();
			return -1;
		}
		m_content_board_index++;
	}
	return 0;
}