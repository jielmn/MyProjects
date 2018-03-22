#include <afx.h>
#include "InvDatabase.h"
#include "Business.h"

#define  FLOW_NUM_LEN                         3
#define  PROCE_TYPE_INV_SMALL                 1

CInvDatabase::CInvDatabase( CBusiness * pBusiness ) {
	m_eDbStatus = STATUS_CLOSE;
	m_eDbType   = TYPE_ORACLE;
	m_bUtf8     = FALSE;
	m_pBusiness = pBusiness;

	m_recordset.m_pDatabase = &m_database;
}

CInvDatabase::~CInvDatabase() {
	Clear();
}

void CInvDatabase::Clear() {	
	m_eDbStatus = STATUS_CLOSE;
	m_eDbType   = TYPE_ORACLE;
	m_bUtf8     = FALSE;
}

// �������ݿ�
int CInvDatabase::ReconnectDb() {
	char buf[8192];
	
	// ������ݿ��Ѿ���
	if ( STATUS_OPEN == m_eDbStatus) {
		return 0;
	}

	try
	{
		m_database.Close();
		m_eDbStatus = STATUS_CLOSE;

		g_cfg->GetConfig( ODBC_TYPE, buf, sizeof(buf) );
		if ( 0 == _stricmp(buf, "oracle") ) {
			m_eDbType = TYPE_ORACLE;
		}
		else {
			m_eDbType = TYPE_MYSQL;
		}

		if (m_eDbType == TYPE_ORACLE) {
			g_cfg->GetConfig( ODBC_STRING_ORACLE, buf, sizeof(buf),"" );
		}
		else {
			g_cfg->GetConfig(ODBC_STRING_MYSQL, buf, sizeof(buf),"");
		}

		char  szOdbcStr[8192] = {0};
		DWORD dwLen = sizeof(szOdbcStr);

		// ����
		MyDecrypt( buf, szOdbcStr, dwLen);

		// �������ݿ�
		m_database.OpenEx(szOdbcStr, CDatabase::noOdbcDialog);

		// �����oracle���ݿ⣬��ֹ��ס
		if ( TYPE_ORACLE == m_eDbType ) {
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



// �û���¼
int CInvDatabase::LoginUser( const CTagItemParam * pItem, User * pUser) {
	CString strSql;
	char buf[8192];

	if ( m_eDbStatus == STATUS_CLOSE ) {
		return -1;
	}

	int ret = 0;
	try
	{
		GetUid( buf, sizeof(buf), pItem->m_item.abyUid, pItem->m_item.dwUidLen );
		g_log->Output(ILog::LOG_SEVERITY_INFO, "try to login id is %s\n", buf);

		strSql.Format("select * from staff where ext1='%s'", buf);
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);

		// ���ڼ�¼����֤OK
		// ��ȡ�û���Ϣ
		if (!m_recordset.IsEOF())
		{
			CString   v;
			m_recordset.GetFieldValue((short)1, v);
			strncpy(pUser->szUserId, v, sizeof(pUser->szUserId) - 1);

			m_recordset.GetFieldValue((short)2, v);
			if (m_bUtf8) {
				Utf8ToAnsi(pUser->szUserName, sizeof(pUser->szUserName), v);
			}
			else {
				strncpy(pUser->szUserName, v, sizeof(pUser->szUserName) - 1);
			}

			memcpy(&pUser->tTagId, &pItem->m_item, sizeof(TagItem));
		}
		//��֤ʧ��
		else {
			ret = 1;
		}
		m_recordset.Close();//�رռ�¼��
	}
	catch (CException* e)
	{
		ret = -2;
		e->GetErrorMessage(buf, sizeof(buf));
		g_log->Output(ILog::LOG_SEVERITY_ERROR, buf);
	}

	return ret;
}

CInvDatabase::DATABASE_STATUS CInvDatabase::GetStatus() {
	return m_eDbStatus;
}

// ����С�̵�
int CInvDatabase::InvSmallSave( const CInvSmallSaveParam * pParam, const char * szUserId, CString & strBatchId ) {
	int ret = 0;

	if ( m_eDbStatus == STATUS_CLOSE ) {
		return INV_ERR_DB_CLOSE;
	}

	CTime now = CTime::GetCurrentTime();
	CString strTime = now.Format("%Y-%m-%d %H:%M:%S");

	char  szFactoryId[64];
	char  szProductId[64];

	g_cfg->GetConfig( FACTORY_CODE, szFactoryId, sizeof(szFactoryId) );
	g_cfg->GetConfig( PRODUCT_CODE, szProductId, sizeof(szProductId) );

	char buf[8192];
	std::vector<TagItem *>::const_iterator it;

	CString  strSql;
	strSql.Format("select max(id) from proceinfo;");
	DWORD   dwProceId = 0;

	try
	{
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);
		if (!m_recordset.IsEOF())
		{
			CString   strValue;
			m_recordset.GetFieldValue((short)0, strValue);
			sscanf(strValue, "%u", &dwProceId);
		}
		m_recordset.Close();//�رռ�¼��
	}
	catch (CException* e)
	{
		ret = OnDatabaseException(e);
	}

	CString strTmp;
	CString strTmp1;

	// �����һ��û�д���
	if (0 == ret) {

		// ��ʼ����
		try
		{
#ifdef _DEBUG
			if (m_database.m_bTransactionPending) {
				m_database.Rollback();
			}
#else
			m_database.Rollback();
#endif
			m_database.BeginTrans();

			// ����tag����
			for (it = pParam->m_items.begin(); it != pParam->m_items.end(); it++) {
				TagItem * pIem = *it;
				GetUid(buf, sizeof(buf), pIem->abyUid, pIem->dwUidLen);
				strSql.Format("insert into tagsinfo values('%s',%u);", buf, dwProceId + 1);
				m_database.ExecuteSQL(strSql);
			}

			CString strMaxBatchId;
			now = CTime::GetCurrentTime();
			strTmp.Format("%s%s%s%%", szFactoryId, szProductId, pParam->m_strBatchId);
			strSql.Format("select proce_batch_id from proceinfo where proce_batch_id like '%s' order by proce_batch_id desc;", strTmp);
			m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);
			if (!m_recordset.IsEOF())
			{
				m_recordset.GetFieldValue((short)0, strMaxBatchId);
			}
			m_recordset.Close();//�رռ�¼��

			int nFlowId = 0;
			if (strMaxBatchId.GetLength() > 0) {
				DWORD dwFactoryLen = strlen(szFactoryId);
				DWORD dwProcCodeLen = strlen(szProductId);
				sscanf(strMaxBatchId.Mid(dwFactoryLen + dwProcCodeLen + pParam->m_strBatchId.GetLength(), FLOW_NUM_LEN), "%d", &nFlowId);
			}
			nFlowId++;

			strTmp1.Format("%%s%%s%%s%%0%dd", FLOW_NUM_LEN);

			strTmp.Format(strTmp1, szFactoryId, szProductId, pParam->m_strBatchId, nFlowId);
			strBatchId = strTmp;

			if (m_eDbType == TYPE_ORACLE) {
				// ������������
				strSql.Format("insert into proceinfo values ( %u, %d, '%s', to_date('%s','yyyy-mm-dd hh24:mi:ss'),'%s','%s')",
					dwProceId + 1, PROCE_TYPE_INV_SMALL, szUserId, strTime, " ", strBatchId);
			}
			else {
				// ������������
				strSql.Format("insert into proceinfo values ( %u, %d, '%s', '%s','%s','%s')",
					dwProceId + 1, PROCE_TYPE_INV_SMALL, szUserId, strTime, " ", strBatchId);
			}

			m_database.ExecuteSQL(strSql);
			m_database.CommitTrans();
		}
		catch (CException* e)
		{
			ret = OnDatabaseException(e);
			if (m_eDbType == STATUS_OPEN) {
				m_database.Rollback();
			}
		}
	}

	// ������ݿ�˿ڣ���������
	if (m_eDbStatus == STATUS_CLOSE) {
		m_pBusiness->NotifyUiDbStatus(m_eDbStatus);
		m_pBusiness->ReconnectDatabaseAsyn(RECONNECT_DB_TIME);
	}
	
	return ret;
}

int  CInvDatabase::OnDatabaseException( CException* e ) {
	char buf[8192];
	e->GetErrorMessage(buf, sizeof(buf));
	g_log->Output(ILog::LOG_SEVERITY_ERROR, buf);

	int ret = INV_ERR_DB_ERROR;

	if (m_eDbType == TYPE_MYSQL) {
		if (0 != strstr(buf, "connection") || 0 != strstr(buf, "û������") || 0 != strstr(buf, "gone awaw")) {
			m_eDbStatus = STATUS_CLOSE;
		}
	}
	else {
		// ����ORACLE��Ͽ�����
		if (strstr(buf, "ORA-12152") != 0) {
			m_eDbStatus = STATUS_CLOSE;
		}
		else if (strstr(buf, "ORA-00001") != 0) {
			ret = INV_ERR_DB_NOT_UNIQUE;
		}
	}

	return ret;
}


// check
int  CInvDatabase::CheckTag(const CTagItemParam * pItem) {
	CString strSql;
	char buf[8192];

	if (m_eDbStatus == STATUS_CLOSE) {
		return -1;
	}

	int ret = 0;
	try
	{
		GetUid(buf, sizeof(buf), pItem->m_item.abyUid, pItem->m_item.dwUidLen);

		strSql.Format("select * from tagsinfo where tagid='%s'", buf);
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);

		// ���ڼ�¼
		if (!m_recordset.IsEOF())
		{
			ret = 1;
		}
		else {
			ret = 0;
		}
		m_recordset.Close();//�رռ�¼��
	}
	catch (CException* e)
	{
		ret = -2;
		e->GetErrorMessage(buf, sizeof(buf));
		g_log->Output(ILog::LOG_SEVERITY_ERROR, buf);
	}

	return ret;
}