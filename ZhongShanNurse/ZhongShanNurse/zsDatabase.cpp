#include "zsDatabase.h"
#include "Business.h"
#include "LmnString.h"


#define  PATIENTS_TABLE_NAME                "patients"

static char * ConvertSqlField(char * szDest, DWORD dwDestSize, const char * filed_value) {
	int ret = StrReplaceAll(szDest, dwDestSize - 1, filed_value, "'", "''");
	if (0 == ret) {
		return szDest;
	}
	else {
		return 0;
	}
}

CZsDatabase::CZsDatabase(CBusiness * pBusiness) {
	m_eDbStatus = STATUS_CLOSE;
	m_eDbType = TYPE_ORACLE;
	m_bUtf8 = FALSE;
	m_pBusiness = pBusiness;

	m_recordset.m_pDatabase = &m_database;
}

CZsDatabase::~CZsDatabase() {
	Clear();
}

void CZsDatabase::Clear() {
	m_eDbStatus = STATUS_CLOSE;
	m_eDbType = TYPE_ORACLE;
	m_bUtf8 = FALSE;
}

CZsDatabase::DATABASE_STATUS CZsDatabase::GetStatus() {
	return m_eDbStatus;
}

int  CZsDatabase::OnDatabaseException(CException* e) {
	char buf[8192];
	e->GetErrorMessage(buf, sizeof(buf));
	g_log->Output(ILog::LOG_SEVERITY_ERROR, buf);

	int ret = ZS_ERR_DB_ERROR;

	if (m_eDbType == TYPE_MYSQL) {
		if (0 != strstr(buf, "connection") || 0 != strstr(buf, "没有连接") || 0 != strstr(buf, "gone awaw")) {
			m_eDbStatus = STATUS_CLOSE;
			ret = ZS_ERR_DB_CLOSE;
		}
		else if (strstr(buf, "Duplicate entry") != 0) {
			ret = ZS_ERR_DB_NOT_UNIQUE;
		}
	}
	else {
		// 看是ORACLE否断开连接
		if (strstr(buf, "ORA-12152") != 0) {
			m_eDbStatus = STATUS_CLOSE;
			ret = ZS_ERR_DB_CLOSE;
		}
		else if (strstr(buf, "ORA-00001") != 0) {
			ret = ZS_ERR_DB_NOT_UNIQUE;
		}
		else if (strstr(buf, "ORA-03135") != 0) {
			m_eDbStatus = STATUS_CLOSE;
			ret = ZS_ERR_DB_CLOSE;
		}
	}

	return ret;
}

// 重连数据库
int CZsDatabase::ReconnectDb() {
	char buf[8192];

	// 如果数据库已经打开
	if (STATUS_OPEN == m_eDbStatus) {
		return 0;
	}

	try
	{
		m_database.Close();
		m_eDbStatus = STATUS_CLOSE;

		g_cfg->GetConfig(ODBC_TYPE, buf, sizeof(buf));
		if (0 == _stricmp(buf, "oracle")) {
			m_eDbType = TYPE_ORACLE;
		}
		else {
			m_eDbType = TYPE_MYSQL;
		}

		if (m_eDbType == TYPE_ORACLE) {
			g_cfg->GetConfig(ODBC_STRING_ORACLE, buf, sizeof(buf), "");
		}
		else {
			g_cfg->GetConfig(ODBC_STRING_MYSQL, buf, sizeof(buf), "");
		}

		char  szOdbcStr[8192] = { 0 };
		DWORD dwLen = sizeof(szOdbcStr);

		// 解密
		MyDecrypt(buf, szOdbcStr, dwLen);

		// 连接数据库
		m_database.OpenEx(szOdbcStr, CDatabase::noOdbcDialog);

		// 如果是oracle数据库，防止锁住
		if (TYPE_ORACLE == m_eDbType) {
			CString strSql = "SELECT sid, serial#, username, osuser FROM v$session where sid in(select session_id from v$locked_object);";
			m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);
			while (!m_recordset.IsEOF())
			{
				CString   strValue[2];
				m_recordset.GetFieldValue((short)0, strValue[0]);
				m_recordset.GetFieldValue((short)1, strValue[1]);

				CString  strTemp;
				strTemp.Format("ALTER SYSTEM KILL SESSION '%s,%s';", strValue[0], strValue[1]);
				m_database.ExecuteSQL(strTemp);
				m_recordset.MoveNext();
			}
			m_recordset.Close();//关闭记录集
		}
		else {
			m_bUtf8 = TRUE;
		}
		m_eDbStatus = STATUS_OPEN;
	}
	catch (CException* e)
	{
		e->GetErrorMessage(buf, sizeof(buf));
		g_log->Output(ILog::LOG_SEVERITY_ERROR, buf);
		m_eDbStatus = STATUS_CLOSE;

		// 重连数据库
		m_pBusiness->ReconnectDatabaseAsyn(RECONNECT_DB_TIME);
	}

	m_pBusiness->NotifyUiDbStatus(m_eDbStatus);
	return 0;
}


int CZsDatabase::AddPatient(const CAddPatientParam * pParam, DWORD & dwId) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	int ret = 0;
	try
	{
		PatientInfo  tPatient;
		memcpy(&tPatient, &pParam->m_patient, sizeof(PatientInfo));
		ConvertSqlField( tPatient.szId,    sizeof(tPatient.szId),   pParam->m_patient.szId );
		ConvertSqlField( tPatient.szName,  sizeof(tPatient.szName), pParam->m_patient.szName );
		ConvertSqlField( tPatient.szBedNo, sizeof(tPatient.szId),   pParam->m_patient.szBedNo );

		strSql.Format( "INSERT INTO %s VALUES( null, '%s', '%s', '%s', %lu, %lu ) ", PATIENTS_TABLE_NAME, tPatient.szId, tPatient.szName, tPatient.szBedNo, tPatient.bFemale, tPatient.bOutHos );

		m_database.ExecuteSQL(strSql);		

		strSql.Format("SELECT MAX(id) from %s ", PATIENTS_TABLE_NAME);
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly );

		CString   strValue;
		m_recordset.GetFieldValue((short)0, strValue);
		sscanf_s( strValue, "%lu",  &dwId );

		m_recordset.Close();//关闭记录集
	}
	catch (CException* e)
	{
		ret = OnDatabaseException(e);
	}

	// 如果数据库端口，重新连接
	if (m_eDbStatus == STATUS_CLOSE) {
		m_pBusiness->NotifyUiDbStatus(m_eDbStatus);
		m_pBusiness->ReconnectDatabaseAsyn(RECONNECT_DB_TIME);
	}

	return ret;
}

