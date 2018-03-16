#include <afx.h>
#include "InvDatabase.h"


CInvDatabase::CInvDatabase(/*CBusiness * pBusiness*/) {
	m_eDbStatus = STATUS_CLOSE;
	m_eDbType   = TYPE_ORACLE;
	m_bUtf8     = FALSE;

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

// 重连数据库
int CInvDatabase::ReconnectDb() {
	char buf[8192];
	
	// 如果数据库已经打开
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
			g_cfg->GetConfig( ODBC_STRING_ORACLE, buf, sizeof(buf) );
		}
		else {
			g_cfg->GetConfig(ODBC_STRING_MYSQL, buf, sizeof(buf));
		}

		char  szOdbcStr[8192] = {0};
		DWORD dwLen = sizeof(szOdbcStr);

		// 解密
		MyDecrypt( buf, szOdbcStr, dwLen);

		// 连接数据库
		m_database.OpenEx(szOdbcStr, CDatabase::noOdbcDialog);

		// 如果是oracle数据库，防止锁住
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

		//g_thrd_db->PostDelayMessage(RECONNECT_DB_TIME, g_handler_db, MSG_RECONNECT_DB);
	}

	//::PostMessage(g_hDlg, UM_SHOW_STATUS, STATUS_TYPE_DATABASE, m_nStatus);
}