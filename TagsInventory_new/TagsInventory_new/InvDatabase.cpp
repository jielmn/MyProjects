#include <afx.h>
#include "InvDatabase.h"
#include "Business.h"

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