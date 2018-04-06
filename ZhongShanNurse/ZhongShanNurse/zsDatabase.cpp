#include "zsDatabase.h"
#include "Business.h"
#include "LmnString.h"


#define  PATIENTS_TABLE_NAME                "patients"
#define  TEMPERATURE_TABLE_NAME             "temperature_data"

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
		if (0 != strstr(buf, "connection") || 0 != strstr(buf, "û������") || 0 != strstr(buf, "gone awaw")) {
			m_eDbStatus = STATUS_CLOSE;
			ret = ZS_ERR_DB_CLOSE;
		}
		else if (strstr(buf, "Duplicate entry") != 0) {
			ret = ZS_ERR_DB_NOT_UNIQUE;
		}
	}
	else {
		// ����ORACLE��Ͽ�����
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

// �������ݿ�
int CZsDatabase::ReconnectDb() {
	char buf[8192];

	// ������ݿ��Ѿ���
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

		// ����
		MyDecrypt(buf, szOdbcStr, dwLen);

		// �������ݿ�
		m_database.OpenEx(szOdbcStr, CDatabase::noOdbcDialog);

		// �����oracle���ݿ⣬��ֹ��ס
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
			m_recordset.Close();//�رռ�¼��
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

		// �������ݿ�
		m_pBusiness->ReconnectDatabaseAsyn(RECONNECT_DB_TIME);
	}

	m_pBusiness->NotifyUiDbStatus(m_eDbStatus);
	return 0;
}


int CZsDatabase::AddPatient(const CPatientParam * pParam, DWORD & dwId) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	int ret = 0;
	try
	{
		PatientInfo  tPatient;
		memcpy(&tPatient, &pParam->m_patient, sizeof(PatientInfo));
		ConvertSqlField( tPatient.szId,    sizeof(tPatient.szId),      pParam->m_patient.szId );
		ConvertSqlField( tPatient.szName,  sizeof(tPatient.szName),    pParam->m_patient.szName );
		ConvertSqlField( tPatient.szBedNo, sizeof(tPatient.szBedNo),   pParam->m_patient.szBedNo );

		strSql.Format( "INSERT INTO %s VALUES( null, '%s', '%s', '%s', %lu, %lu ) ", PATIENTS_TABLE_NAME, tPatient.szId, tPatient.szName, tPatient.szBedNo, tPatient.bFemale, tPatient.bOutHos );

		m_database.ExecuteSQL(strSql);		

		strSql.Format("SELECT MAX(id) from %s ", PATIENTS_TABLE_NAME);
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly );

		CString   strValue;
		m_recordset.GetFieldValue((short)0, strValue);
		sscanf_s( strValue, "%lu",  &dwId );

		m_recordset.Close();//�رռ�¼��
	}
	catch (CException* e)
	{
		ret = OnDatabaseException(e);
	}

	// ������ݿ�˿ڣ���������
	if (m_eDbStatus == STATUS_CLOSE) {
		m_pBusiness->NotifyUiDbStatus(m_eDbStatus);
		m_pBusiness->ReconnectDatabaseAsyn(RECONNECT_DB_TIME);
	}

	return ret;
}

int CZsDatabase::GetAllPatients( std::vector<PatientInfo *> & vRet ) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	int ret = 0;
	try
	{
		strSql.Format("SELECT * FROM %s ORDER BY in_hos, bed_no ", PATIENTS_TABLE_NAME );
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);

		while (!m_recordset.IsEOF())
		{
			CString       strValue;
			PatientInfo   tPatient;
			memset( &tPatient, 0, sizeof(PatientInfo));

			m_recordset.GetFieldValue((short)0, strValue);
			sscanf_s( strValue, "%lu", &tPatient.dwId );

			m_recordset.GetFieldValue((short)1, strValue);
			strncpy_s(tPatient.szId, strValue, sizeof(tPatient.szId));

			m_recordset.GetFieldValue((short)2, strValue);
			strncpy_s(tPatient.szName, strValue, sizeof(tPatient.szName));

			m_recordset.GetFieldValue((short)3, strValue);
			strncpy_s(tPatient.szBedNo, strValue, sizeof(tPatient.szBedNo));

			m_recordset.GetFieldValue((short)4, strValue);
			sscanf_s(strValue, "%lu",  &tPatient.bFemale );

			m_recordset.GetFieldValue((short)5, strValue);
			sscanf_s(strValue, "%lu", &tPatient.bOutHos );

			PatientInfo *  pPatient = new PatientInfo;
			if ( 0 == pPatient) {
				ClearVector(vRet);
				ret = ZS_ERR_NO_MEMORY;
				return ret;
			}

			memcpy(pPatient, &tPatient, sizeof(PatientInfo));
			vRet.push_back(pPatient);

			m_recordset.MoveNext();
		}

		m_recordset.Close();//�رռ�¼��
	}
	catch (CException* e)
	{
		ret = OnDatabaseException(e);
	}

	// ������ݿ�˿ڣ���������
	if (m_eDbStatus == STATUS_CLOSE) {
		m_pBusiness->NotifyUiDbStatus(m_eDbStatus);
		m_pBusiness->ReconnectDatabaseAsyn(RECONNECT_DB_TIME);
	}

	return ret;
}

int CZsDatabase::ModifyPatient(const CPatientParam * pParam) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	int ret = 0;
	try
	{
		PatientInfo  tPatient;
		memcpy(&tPatient, &pParam->m_patient, sizeof(PatientInfo));
		ConvertSqlField(tPatient.szId,     sizeof(tPatient.szId), pParam->m_patient.szId);
		ConvertSqlField(tPatient.szName,   sizeof(tPatient.szName), pParam->m_patient.szName);
		ConvertSqlField(tPatient.szBedNo, sizeof(tPatient.szBedNo), pParam->m_patient.szBedNo);

		strSql.Format( "UPDATE %s SET str_id='%s',name='%s',bed_no='%s',gender=%lu,in_hos=%lu where id=%lu ", PATIENTS_TABLE_NAME, 
			            tPatient.szId, tPatient.szName, tPatient.szBedNo, tPatient.bFemale, tPatient.bOutHos, tPatient.dwId );

		m_database.ExecuteSQL(strSql);		
	}
	catch (CException* e)
	{
		ret = OnDatabaseException(e);
	}

	// ������ݿ�˿ڣ���������
	if (m_eDbStatus == STATUS_CLOSE) {
		m_pBusiness->NotifyUiDbStatus(m_eDbStatus);
		m_pBusiness->ReconnectDatabaseAsyn(RECONNECT_DB_TIME);
	}

	return ret;
}

int CZsDatabase::DeletePatient(const CDeletePatientParam * pParam) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	int ret = 0;
	try
	{
		strSql.Format("SELECT count(*) from %s a inner join %s b on a.str_id = b.patient_id WHERE a.id=%lu ", PATIENTS_TABLE_NAME, TEMPERATURE_TABLE_NAME, pParam->m_dwId );
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);
		// ���������
		if ( !m_recordset.IsEOF() )
		{
			CString       strValue;
			DWORD         dwCnt = 0;

			m_recordset.GetFieldValue((short)0, strValue);
			sscanf_s(strValue, "%lu", &dwCnt);

			if (dwCnt > 0) {
				ret = ZS_ERR_PATIENT_HAS_TEMP_DATA;
			}			
		}
		m_recordset.Close();//�رռ�¼��

		// ����ò���û������
		if (0 == ret) {
			strSql.Format("DELETE FROM %s WHERE id=%lu ", PATIENTS_TABLE_NAME, pParam->m_dwId);
			m_database.ExecuteSQL(strSql);
		}
	}
	catch (CException* e)
	{
		ret = OnDatabaseException(e);
	}

	// ������ݿ�˿ڣ���������
	if (m_eDbStatus == STATUS_CLOSE) {
		m_pBusiness->NotifyUiDbStatus(m_eDbStatus);
		m_pBusiness->ReconnectDatabaseAsyn(RECONNECT_DB_TIME);
	}

	return ret;
}


int CZsDatabase::ImportPatient( PatientInfo * pPatient) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	int ret = 0;
	try
	{
		PatientInfo  tPatient;
		memcpy(&tPatient, pPatient, sizeof(PatientInfo));
		ConvertSqlField(tPatient.szId, sizeof(tPatient.szId), pPatient->szId);
		ConvertSqlField(tPatient.szName, sizeof(tPatient.szName), pPatient->szName);
		ConvertSqlField(tPatient.szBedNo, sizeof(tPatient.szBedNo), pPatient->szBedNo);

		BOOL bFind = FALSE;
		CString   strValue;
		strSql.Format("SELECT id FROM %s WHERE str_id='%s' ", PATIENTS_TABLE_NAME, tPatient.szId);

		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);
		if (!m_recordset.IsEOF())
		{
			m_recordset.GetFieldValue((short)0, strValue);
			sscanf_s(strValue, "%lu", &tPatient.dwId);
			bFind = TRUE;
		}
		m_recordset.Close();//�رռ�¼��

		// ���������ͬstr_id�Ĳ��˼�¼
		if (bFind) {
			strSql.Format("UPDATE %s SET name='%s',bed_no='%s',gender=%lu,in_hos=%lu WHERE str_id='%s' ", PATIENTS_TABLE_NAME,
				tPatient.szName, tPatient.szBedNo, tPatient.bFemale, tPatient.bOutHos, tPatient.szId );
			m_database.ExecuteSQL(strSql);
			pPatient->bToUpdated = TRUE;
		}
		// �����¼
		else {
			strSql.Format("INSERT INTO %s VALUES( null, '%s', '%s', '%s', %lu, %lu ) ", PATIENTS_TABLE_NAME, tPatient.szId, tPatient.szName, tPatient.szBedNo, tPatient.bFemale, tPatient.bOutHos);			
			m_database.ExecuteSQL(strSql);

			strSql.Format("SELECT MAX(id) from %s ", PATIENTS_TABLE_NAME);
			m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);

			m_recordset.GetFieldValue((short)0, strValue);
			sscanf_s(strValue, "%lu", &tPatient.dwId);

			m_recordset.Close();//�رռ�¼��
			pPatient->bToUpdated = FALSE;
		}
		pPatient->dwId = tPatient.dwId;
		
	}
	catch (CException* e)
	{
		ret = OnDatabaseException(e);
	}

	// ������ݿ�˿ڣ���������
	if (m_eDbStatus == STATUS_CLOSE) {
		m_pBusiness->NotifyUiDbStatus(m_eDbStatus);
		m_pBusiness->ReconnectDatabaseAsyn(RECONNECT_DB_TIME);
	}

	return ret;
}
