#include <assert.h>
#include "zsDatabase.h"
#include "Business.h"
#include "LmnString.h"
#include "LmnTemplates.h"

#define  PATIENTS_TABLE_NAME                "patients"
#define  TEMPERATURE_TABLE_NAME             "temperature_data"
#define  NURSES_TABLE_NAME                  "nurses"
#define  TAGS_TABLE_NAME                    "tags"

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
		//g_log->Output(ILog::LOG_SEVERITY_INFO, "odbc string = %s\n", szOdbcStr);
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

		m_recordset.Close();//关闭记录集


		std::vector<PatientInfo *>::iterator it;
		for (it = vRet.begin(); it != vRet.end(); it++) {
			PatientInfo * pItem = *it;

			strSql.Format("SELECT id FROM %s WHERE patient_id=%lu ", TAGS_TABLE_NAME, pItem->dwId );
			m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);

			int i = 0;
			while (!m_recordset.IsEOF()&& i < MAX_PATIENT_TAGS)
			{
				CString       strValue;

				m_recordset.GetFieldValue((short)0, strValue);
				GetUid(&pItem->tags[i], strValue);

				m_recordset.MoveNext();
				i++;
			}
			pItem->dwTagsCnt = i;

			m_recordset.Close();//关闭记录集
		}
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

		if (tPatient.bStrIdChanged) {
			strSql.Format("UPDATE %s SET patient_id='%s' where patient_id='%s' ", TEMPERATURE_TABLE_NAME,
				tPatient.szId, tPatient.szId);
			m_database.ExecuteSQL(strSql);
		}
		

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
		// 如果有数据
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
		m_recordset.Close();//关闭记录集

		// 如果该病人没有数据
		if (0 == ret) {
			strSql.Format("DELETE FROM %s WHERE patient_id=%lu ", TAGS_TABLE_NAME, pParam->m_dwId);
			m_database.ExecuteSQL(strSql);

			strSql.Format("DELETE FROM %s WHERE id=%lu ", PATIENTS_TABLE_NAME, pParam->m_dwId);
			m_database.ExecuteSQL(strSql);
		}
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
		m_recordset.Close();//关闭记录集

		// 如果存在相同str_id的病人记录
		if (bFind) {
			strSql.Format("UPDATE %s SET name='%s',bed_no='%s',gender=%lu,in_hos=%lu WHERE str_id='%s' ", PATIENTS_TABLE_NAME,
				tPatient.szName, tPatient.szBedNo, tPatient.bFemale, tPatient.bOutHos, tPatient.szId );
			m_database.ExecuteSQL(strSql);
			pPatient->bToUpdated = TRUE;
		}
		// 插入记录
		else {
			strSql.Format("INSERT INTO %s VALUES( null, '%s', '%s', '%s', %lu, %lu ) ", PATIENTS_TABLE_NAME, tPatient.szId, tPatient.szName, tPatient.szBedNo, tPatient.bFemale, tPatient.bOutHos);			
			m_database.ExecuteSQL(strSql);

			strSql.Format("SELECT MAX(id) from %s ", PATIENTS_TABLE_NAME);
			m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);

			m_recordset.GetFieldValue((short)0, strValue);
			sscanf_s(strValue, "%lu", &tPatient.dwId);

			m_recordset.Close();//关闭记录集
			pPatient->bToUpdated = FALSE;
		}
		pPatient->dwId = tPatient.dwId;
		
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


int CZsDatabase::AddNurse(const CNurseParam * pParam, DWORD & dwId) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	int ret = 0;
	try
	{
		NurseInfo  tNurse;
		memcpy(&tNurse, &pParam->m_nurse, sizeof(NurseInfo));
		ConvertSqlField(tNurse.szId,   sizeof(tNurse.szId),   pParam->m_nurse.szId);
		ConvertSqlField(tNurse.szName, sizeof(tNurse.szName), pParam->m_nurse.szName);

		strSql.Format("INSERT INTO %s VALUES( null, '%s', '%s', null ) ", NURSES_TABLE_NAME, tNurse.szId, tNurse.szName );

		m_database.ExecuteSQL(strSql);

		strSql.Format("SELECT MAX(id) from %s ", NURSES_TABLE_NAME);
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);

		CString   strValue;
		m_recordset.GetFieldValue((short)0, strValue);
		sscanf_s(strValue, "%lu", &dwId);

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

int CZsDatabase::GetAllNurses(std::vector<NurseInfo *> & vRet) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	int ret = 0;
	try
	{
		strSql.Format("SELECT * FROM %s ", NURSES_TABLE_NAME);
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);

		while (!m_recordset.IsEOF())
		{
			CString       strValue;
			NurseInfo     tNurse;
			memset(&tNurse, 0, sizeof(NurseInfo));

			m_recordset.GetFieldValue((short)0, strValue);
			sscanf_s(strValue, "%lu", &tNurse.dwId);

			m_recordset.GetFieldValue((short)1, strValue);
			strncpy_s(tNurse.szId, strValue, sizeof(tNurse.szId));

			m_recordset.GetFieldValue((short)2, strValue);
			strncpy_s(tNurse.szName, strValue, sizeof(tNurse.szName));

			CDBVariant  objValue;
			m_recordset.GetFieldValue((short)3, objValue);
			if (objValue.m_dwType != 0) {
				GetUid(&tNurse.tag, *objValue.m_pstring);
				tNurse.bBindtingCard = TRUE;
			}
			

			NurseInfo *  pNurse = new NurseInfo;
			if (0 == pNurse) {
				ClearVector(vRet);
				ret = ZS_ERR_NO_MEMORY;
				return ret;
			}

			memcpy(pNurse, &tNurse, sizeof(NurseInfo));
			vRet.push_back(pNurse);

			m_recordset.MoveNext();
		}

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

int CZsDatabase::ModifyNurse(const CNurseParam * pParam) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	int ret = 0;
	try
	{
		NurseInfo  tNurse;
		memcpy(&tNurse, &pParam->m_nurse, sizeof(NurseInfo));

		ConvertSqlField(tNurse.szId, sizeof(tNurse.szId),     pParam->m_nurse.szId);
		ConvertSqlField(tNurse.szName, sizeof(tNurse.szName), pParam->m_nurse.szName);

		strSql.Format("UPDATE %s SET str_id='%s',name='%s' where id=%lu ", NURSES_TABLE_NAME,
			tNurse.szId, tNurse.szName, tNurse.dwId);

		m_database.ExecuteSQL(strSql);

		if (tNurse.bStrIdChanged) {
			strSql.Format("UPDATE %s SET nurse_id='%s' where nurse_id='%s' ", TEMPERATURE_TABLE_NAME,
				tNurse.szId, tNurse.szId);
			m_database.ExecuteSQL(strSql);
		}

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


int CZsDatabase::DeleteNurse(const CDeleteNurseParam * pParam) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	int ret = 0;
	try
	{
		strSql.Format("SELECT count(*) from %s a inner join %s b on a.str_id = b.nurse_id WHERE a.id=%lu ", NURSES_TABLE_NAME, TEMPERATURE_TABLE_NAME, pParam->m_dwId);
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);
		// 如果有数据
		if (!m_recordset.IsEOF())
		{
			CString       strValue;
			DWORD         dwCnt = 0;

			m_recordset.GetFieldValue((short)0, strValue);
			sscanf_s(strValue, "%lu", &dwCnt);

			if (dwCnt > 0) {
				ret = ZS_ERR_NURSE_HAS_TEMP_DATA;
			}
		}
		m_recordset.Close();//关闭记录集

		// 如果该护士没有温度数据
		if (0 == ret) {
			strSql.Format("DELETE FROM %s WHERE id=%lu ", NURSES_TABLE_NAME, pParam->m_dwId);
			m_database.ExecuteSQL(strSql);
		}
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


int CZsDatabase::ImportNurse(NurseInfo * pNurse) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	int ret = 0;
	try
	{
		NurseInfo  tNurse;
		memcpy(&tNurse, pNurse, sizeof(NurseInfo));

		ConvertSqlField(tNurse.szId,   sizeof(tNurse.szId),   pNurse->szId);
		ConvertSqlField(tNurse.szName, sizeof(tNurse.szName), pNurse->szName);

		BOOL bFind = FALSE;
		CString   strValue;
		strSql.Format("SELECT id FROM %s WHERE str_id='%s' ", NURSES_TABLE_NAME, tNurse.szId);

		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);
		if (!m_recordset.IsEOF())
		{
			m_recordset.GetFieldValue((short)0, strValue);
			sscanf_s(strValue, "%lu", &tNurse.dwId);
			bFind = TRUE;
		}
		m_recordset.Close();//关闭记录集

							// 如果存在相同str_id的病人记录
		if (bFind) {
			strSql.Format("UPDATE %s SET name='%s' WHERE str_id='%s' ", NURSES_TABLE_NAME, tNurse.szName, tNurse.szId);
			m_database.ExecuteSQL(strSql);
			pNurse->bToUpdated = TRUE;
		}
		// 插入记录
		else {
			strSql.Format("INSERT INTO %s VALUES( null, '%s', '%s', null ) ", NURSES_TABLE_NAME, tNurse.szId, tNurse.szName );
			m_database.ExecuteSQL(strSql);

			strSql.Format("SELECT MAX(id) from %s ", NURSES_TABLE_NAME);
			m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);

			m_recordset.GetFieldValue((short)0, strValue);
			sscanf_s(strValue, "%lu", &tNurse.dwId);

			m_recordset.Close();//关闭记录集
			pNurse->bToUpdated = FALSE;
		}
		pNurse->dwId = tNurse.dwId;

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


int CZsDatabase::CheckTagBinding(const CTagItemParam * pParam, DWORD & dwPatientId) {
	CString strSql;
	dwPatientId = 0;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	char szTagId[256] = {0};
	GetUid(szTagId, sizeof(szTagId), pParam->m_tag.abyUid, pParam->m_tag.dwUidLen);

	int ret = 0;
	try
	{
		strSql.Format("SELECT b.id FROM %s a INNER JOIN %s b on a.patient_id = b.id WHERE a.id='%s' ", TAGS_TABLE_NAME, PATIENTS_TABLE_NAME, szTagId );
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);

		if (!m_recordset.IsEOF())
		{
			CString       strValue;
			m_recordset.GetFieldValue((short)0, strValue);
			sscanf_s(strValue, "%lu", &dwPatientId);			
		}

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

int CZsDatabase::BindingPatient(const CBindingPatientParam * pParam) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	char szTagId[256] = { 0 };
	GetUid(szTagId, sizeof(szTagId), pParam->m_tag.abyUid, pParam->m_tag.dwUidLen);

	int ret = 0;
	try
	{
		strSql.Format("SELECT count(*) FROM %s WHERE patient_id=%lu", TAGS_TABLE_NAME, pParam->m_dwPatientId);
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);
		DWORD dwCnt = 0;
		if (!m_recordset.IsEOF())
		{
			CString       strValue;
			m_recordset.GetFieldValue((short)0, strValue);
			sscanf_s(strValue, "%lu", &dwCnt);
		}
		m_recordset.Close();

		// 绑定的Tag太多
		if (dwCnt >= MAX_PATIENT_TAGS) {
			return ZS_ERR_PATIENT_HAS_TOO_MANY_TAGS;
		}

		strSql.Format("INSERT INTO %s VALUES ('%s', %lu)", TAGS_TABLE_NAME, szTagId, pParam->m_dwPatientId);
		m_database.ExecuteSQL(strSql);
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

int CZsDatabase::DeleteTag(const CDeleteTagParam * pParam) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	char szTagId[256] = { 0 };
	GetUid(szTagId, sizeof(szTagId), pParam->m_tag.abyUid, pParam->m_tag.dwUidLen);

	int ret = 0;
	try
	{
		strSql.Format("DELETE FROM %s WHERE id='%s'", TAGS_TABLE_NAME, szTagId);
		m_database.ExecuteSQL(strSql);
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

int CZsDatabase::CheckCardBinding(const CTagItemParam * pParam, DWORD & dwNurseId) {
	CString strSql;
	dwNurseId = 0;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	char szTagId[256] = { 0 };
	GetUid(szTagId, sizeof(szTagId), pParam->m_tag.abyUid, pParam->m_tag.dwUidLen);

	int ret = 0;
	try
	{
		strSql.Format("SELECT id FROM %s WHERE card_no='%s' ", NURSES_TABLE_NAME, szTagId);
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);

		if (!m_recordset.IsEOF())
		{
			CString       strValue;
			m_recordset.GetFieldValue((short)0, strValue);
			sscanf_s(strValue, "%lu", &dwNurseId);
		}

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

int CZsDatabase::BindingNurse(const CBindingNurseParam * pParam) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	char szTagId[256] = { 0 };
	GetUid(szTagId, sizeof(szTagId), pParam->m_tag.abyUid, pParam->m_tag.dwUidLen);

	int ret = 0;
	try
	{
		strSql.Format("UPDATE %s SET card_no='%s' WHERE  id=%lu", NURSES_TABLE_NAME, szTagId, pParam->m_dwNurseId );
		m_database.ExecuteSQL(strSql);
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

int CZsDatabase::CompleteSyncData(const CCompleteSyncDataParam * pParam) {
	CString    strSql;
	char       buf[8192];	

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	std::vector<SyncItem*> * pvSyncData = pParam->m_pvSyncData;
	assert(pvSyncData);

	std::vector<TagItem * >  vNursesId;
	std::vector<TagItem * >  vTagsId;

	std::vector<SyncItem*>::iterator  it0;
	std::vector<TagItem * >::iterator it1;
	std::vector<TagItem * >::iterator it2;

	// 提取出nurse id和tag id
	for ( it0 = pvSyncData->begin(); it0 != pvSyncData->end(); it0++ ) {
		SyncItem* pSyncItem = *it0;

		for ( it1 = vNursesId.begin(); it1 != vNursesId.end(); it1++ ) {
			TagItem * pNurseIdItem = *it1;

			// 如果是相同的护士ID
			if ( IsSameTag(pNurseIdItem, &pSyncItem->tNurseId) ) {
				break;
			}
		}
		// 如果没有找到
		if (it1 == vNursesId.end()) {
			vNursesId.push_back( &pSyncItem->tNurseId );
		}

		for (it2 = vTagsId.begin(); it2 != vTagsId.end(); it2++) {
			TagItem * pTagIdItem = *it2;

			// 如果是相同的TagID
			if (IsSameTag(pTagIdItem, &pSyncItem->tTagId)) {
				break;
			}
		}
		// 如果没有找到
		if (it2 == vTagsId.end()) {
			vTagsId.push_back(&pSyncItem->tTagId);
		}
	}

	// 根据nurse id和tag id查询护士和病人信息
	int ret = 0;
	try
	{
		
		for (it1 = vNursesId.begin(); it1 != vNursesId.end(); it1++) {
			TagItem * pNurseIdItem = *it1;		
			GetUid(buf, sizeof(buf), pNurseIdItem->abyUid, pNurseIdItem->dwUidLen);

			strSql.Format("SELECT str_id, name FROM %s WHERE  card_no='%s' ", NURSES_TABLE_NAME, buf);
			m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);
			if (!m_recordset.IsEOF())
			{
				CString    strId;
				CString    strName;

				m_recordset.GetFieldValue((short)0, strId);
				m_recordset.GetFieldValue((short)1, strName);

				for (it0 = pvSyncData->begin(); it0 != pvSyncData->end(); it0++) {
					SyncItem* pSyncItem = *it0;

					// 填写护士信息
					if ( IsSameTag(pNurseIdItem, &pSyncItem->tNurseId)) {
						strncpy_s( pSyncItem->szNurseName, strName,sizeof(pSyncItem->szNurseName) );
						strncpy_s( pSyncItem->szNurseId, strId, sizeof(pSyncItem->szNurseId) );
						SetBit(pSyncItem->dwInfoBitmask, NURSE_BITMASK_INDEX, true);
					}
				}
			}
			m_recordset.Close();//关闭记录集
		}


		for (it2 = vTagsId.begin(); it2 != vTagsId.end(); it2++) {
			TagItem * pTagIdItem = *it2;
			GetUid(buf, sizeof(buf), pTagIdItem->abyUid, pTagIdItem->dwUidLen);

			strSql.Format("SELECT a.str_id, a.name, a.bed_no FROM %s a inner join %s b on a.id = b.patient_id WHERE b.id = '%s' ", PATIENTS_TABLE_NAME, TAGS_TABLE_NAME, buf );
			m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);
			if (!m_recordset.IsEOF())
			{
				CString    strId;
				CString    strName;
				CString    strBedNo;

				m_recordset.GetFieldValue((short)0, strId);
				m_recordset.GetFieldValue((short)1, strName);
				m_recordset.GetFieldValue((short)2, strBedNo);

				for (it0 = pvSyncData->begin(); it0 != pvSyncData->end(); it0++) {
					SyncItem* pSyncItem = *it0;

					// 填写病人信息
					if (IsSameTag(pTagIdItem, &pSyncItem->tTagId)) {
						strncpy_s(pSyncItem->szPatientName,  strName,  sizeof(pSyncItem->szPatientName));
						strncpy_s(pSyncItem->szPatientId,    strId,    sizeof(pSyncItem->szPatientId));
						strncpy_s(pSyncItem->szPatientBedNo, strBedNo, sizeof(pSyncItem->szPatientBedNo));

						SetBit(pSyncItem->dwInfoBitmask, PATIENT_BITMASK_INDEX, true);
					}
				}
			}
			m_recordset.Close();//关闭记录集
		}

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

int CZsDatabase::Update(const CUpdateParam * pParam) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	std::vector<SyncItem*> * pvSyncData = pParam->m_pvSyncData;
	assert(pvSyncData);

	int ret = 0;
	char szPatientId[256];
	char szNurseId[256];
	char szReaderId[256];
	char szDate[256];

	try
	{
		std::vector<SyncItem*>::iterator it;
		for (it = pvSyncData->begin(); it != pvSyncData->end(); it++) {
			SyncItem* pItem = *it;

			// 如果id为空，则不保存
			if (pItem->szNurseId[0] == '\0' || pItem->szPatientId[0] == '\0') {
				continue; 
			}

			ConvertSqlField(szPatientId, sizeof(szPatientId), pItem->szPatientId);
			ConvertSqlField(szNurseId,   sizeof(szNurseId),   pItem->szNurseId);
			GetUid(szReaderId, sizeof(szReaderId), pItem->tReaderId.abyUid, pItem->tReaderId.dwUidLen );
			DateTime2Str( szDate, sizeof(szDate), &pItem->tTime );

			strSql.Format("INSERT INTO %s VALUES (NULL, '%s', '%s', '%s', '%s', %lu ) ", TEMPERATURE_TABLE_NAME, szPatientId, szNurseId, szReaderId, szDate, pItem->dwTemperature );
			m_database.ExecuteSQL(strSql);
		}
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

int CZsDatabase::Query(const CQueryParam * pParam, std::vector<QueryItem* > & vRet) {
	CString strSql;

	if (m_eDbStatus == STATUS_CLOSE) {
		return ZS_ERR_DB_CLOSE;
	}

	int ret = 0;
	time_t tStart = pParam->m_tTime;
	time_t tEnd = tStart + (pParam->m_nTimeSpanIndex + 1) * 7 * 3600 * 24;

	char szStart[256];
	char szEnd[256];

	DateTime2Str(szStart, sizeof(szStart), &tStart);
	DateTime2Str(szEnd, sizeof(szEnd), &tEnd);

	try
	{
		strSql.Format("SELECT c.name, a.reader_id, a.gen_time, a.temp_data FROM %s a inner join %s b on a.patient_id = b.str_id"
			          " inner join %s c on a.nurse_id = c.str_id WHERE b.id = %lu AND a.gen_time >= '%s' AND a.gen_time < '%s' ", 
			           TEMPERATURE_TABLE_NAME, PATIENTS_TABLE_NAME, NURSES_TABLE_NAME, pParam->m_dwPatientId, szStart, szEnd );

		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);

		while (!m_recordset.IsEOF())
		{
			QueryItem* pItem = new QueryItem;
			memset(pItem, 0, sizeof(QueryItem));

			CString       strValue;

			m_recordset.GetFieldValue((short)0, strValue);
			strncpy_s(pItem->szNurseName, strValue, sizeof(pItem->szNurseName));

			m_recordset.GetFieldValue((short)1, strValue);
			GetUid(&pItem->tReaderId, strValue);
			
			m_recordset.GetFieldValue((short)2, strValue);
			pItem->tTime = String2Time(strValue);

			m_recordset.GetFieldValue((short)3, strValue);
			sscanf_s(strValue, "%lu", &pItem->dwTemperature);

			vRet.push_back(pItem);

			m_recordset.MoveNext();
		}

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