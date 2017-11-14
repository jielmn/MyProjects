#include "stdafx.h"
#include "MyDatabase.h"
#include "MyEncrypt.h"


#define  FLOW_NUM_LEN                         3

#define  DB_TYPE_ORACLE                  0
#define  DB_TYPE_MYSQL                   1

static int CountChar(const CString & str, char ch) {
	int iStart = -1;
	int cnt = 0;
	iStart = str.Find(ch, iStart + 1);
	while (iStart != -1) {
		cnt++;
		iStart = str.Find(ch, iStart + 1);
	}
	return cnt;
}

CMyDatabase::CMyDatabase() {
	m_nStatus = -1;
	m_bUtf8 = FALSE;
	m_nDbType = -1;
}

CMyDatabase::~CMyDatabase() {

}

int CMyDatabase::GetStatus() const {
	return m_nStatus;
}

void CMyDatabase::ReconnectDb() {
	char buf[8192];
	char tmp[8192];
	static BOOL bFist = TRUE;
	CString strSql;

	if (m_nStatus == STATUS_OPEN) {
		return;
	}

	try
	{
		m_database.Close();
		m_nStatus = STATUS_CLOSE;
		g_cfg->GetConfig("odbc string", tmp, sizeof(tmp));
		DWORD dwLen = sizeof(buf);
		MyDecrypt(tmp, buf, dwLen);
		m_database.OpenEx(buf, CDatabase::noOdbcDialog);
		if (bFist) {
			m_recordset.m_pDatabase = &m_database;
			bFist = FALSE;
		}

		g_cfg->GetConfig("odbc type", buf, sizeof(buf));
		if (0 == _stricmp(buf, "oracle")) {
			m_nDbType = DB_TYPE_ORACLE;
			strSql = "SELECT sid, serial#, username, osuser FROM v$session where sid in(select session_id from v$locked_object);";
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
			m_nDbType = DB_TYPE_MYSQL;
			m_bUtf8 = TRUE;
		}
		m_nStatus = STATUS_OPEN;
	}
	catch (CException* e)
	{
		e->GetErrorMessage(buf, sizeof(buf));
		g_log->Output(ILog::LOG_SEVERITY_ERROR, buf);
		m_nStatus = STATUS_CLOSE;

		g_thrd_db->PostDelayMessage(RECONNECT_DB_TIME, g_handler_db, MSG_RECONNECT_DB);
	}

	::PostMessage(g_hDlg, UM_SHOW_STATUS, STATUS_TYPE_DATABASE, m_nStatus);
}

void CMyDatabase::VerifyUser(const TagItem * pItem) {
	CString strSql;
	char buf[8192];
	int static cnt = 0;
	cnt++;

	if ( m_nStatus == STATUS_CLOSE ) {
		return;
	}

	try
	{
		GetUid(buf, sizeof(buf), pItem->abyUid, pItem->dwUidLen);
		if (g_bLogVerifyID) {
			g_log->Output(ILog::LOG_SEVERITY_INFO, "verify id is %s\n", buf);
		}

		strSql.Format("select * from staff where ext1='%s'", buf);
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);
		// 存在记录，验证OK
		if (!m_recordset.IsEOF())
		{
			CString   v;
			m_recordset.GetFieldValue( (short)1, v );
			strncpy(g_login_user.user_id, v, sizeof(g_login_user.user_id) - 1);

			m_recordset.GetFieldValue( (short)2, v );
			if ( m_bUtf8) {
				Utf8ToAnsi(g_login_user.user_name, sizeof(g_login_user.user_name), v);
			}
			else {
				strncpy(g_login_user.user_name, v, sizeof(g_login_user.user_name) - 1);
			}

			memcpy(&g_login_user.user_tag_id, pItem, sizeof(TagItem));

			// 验证成功
			::PostMessage(g_hDlg, UM_VERIFY_USER_RESULT, TRUE, 0);	
		}
		//验证失败
		else {
			::PostMessage(g_hDlg, UM_VERIFY_USER_RESULT, FALSE, 0);
		}
		m_recordset.Close();//关闭记录集
	}
	catch (CException* e)
	{
		e->GetErrorMessage(buf, sizeof(buf));
		g_log->Output(ILog::LOG_SEVERITY_ERROR, buf);
	}
	
}


void  CMyDatabase::SaveInventory(CSaveInvParam * pParam) {
	if (m_nStatus == STATUS_CLOSE) {
		NotifyUiError(g_hDlg, 1, "数据库没有连接上", TRUE, "");
		return;
	}

	CTime now = CTime::GetCurrentTime();
	CString strTime = now.Format("%Y-%m-%d %H:%M:%S");

	char  szFactoryId[64];
	char  szProductId[64];

	g_cfg->GetConfig("factory code", szFactoryId, sizeof(szFactoryId));
	g_cfg->GetConfig("product code", szProductId, sizeof(szProductId));

	char buf[8192];
	CString strTags;
	vector<TagItem *>::iterator it;
	for ( it = pParam->m_items.begin(); it != pParam->m_items.end(); ++it) {
		TagItem * pItem = *it;
		GetUid( buf, sizeof(buf), pItem->abyUid, pItem->dwUidLen );
		if (strTags.GetLength() > 0) {
			strTags += ",";
		}
		strTags += buf;
	}

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
		m_recordset.Close();//关闭记录集
	}
	catch (CException* e)
	{
		OnDatabaseException(g_hDlg, e);
		return;
	}



	CString strTmp;
	// 开始事务
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

		// 插入tag数据
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
		m_recordset.Close();//关闭记录集

		int nFlowId = 0;
		if (strMaxBatchId.GetLength() > 0) {
			DWORD dwFactoryLen = strlen(szFactoryId);
			DWORD dwProcCodeLen = strlen(szProductId);
			sscanf(strMaxBatchId.Mid(dwFactoryLen + dwProcCodeLen + pParam->m_strBatchId.GetLength(), FLOW_NUM_LEN), "%d", &nFlowId);
		}
		nFlowId++;

		strTmp.Format("%s%s%s%03d", szFactoryId, szProductId, pParam->m_strBatchId, nFlowId);
		CString strBatchId = strTmp;

		if (m_nDbType == DB_TYPE_ORACLE) {
			// 插入批量数据
			strSql.Format("insert into proceinfo values ( %u, 100, '%s', to_date('%s','yyyy-mm-dd hh24:mi:ss'),'%s','%s')",
				dwProceId + 1, g_login_user.user_id, strTime, strTags, strBatchId);
		}
		else {
			// 插入批量数据
			strSql.Format("insert into proceinfo values ( %u, 100, '%s', '%s','%s','%s')",
				dwProceId + 1, g_login_user.user_id, strTime, strTags, strBatchId);
		}
		
		m_database.ExecuteSQL(strSql);
		m_database.CommitTrans();
		NotifyUiError( g_hDlg, 0, 0, FALSE, strBatchId);
	}
	catch (CException* e)
	{
		OnDatabaseException(g_hDlg, e);
		if (m_nStatus == STATUS_OPEN) {
			m_database.Rollback();
		}
		return;
	}
}

void  CMyDatabase::OnDatabaseException( HWND hWnd, CException* e, BOOL bNotifyUi /*= TRUE*/, void * pParam /*= 0*/) {
	char buf[8192];
	e->GetErrorMessage(buf, sizeof(buf));
	g_log->Output(ILog::LOG_SEVERITY_ERROR, buf);

	if (!bNotifyUi) {
		return;
	}

	BOOL  bNeedReconnect = FALSE;
	if (m_nDbType == DB_TYPE_MYSQL) {
		if ( 0 != strstr(buf, "connection") || 0 != strstr(buf, "没有连接") || 0 != strstr(buf, "gone awaw") ) {
			bNeedReconnect = TRUE;
			m_nStatus = STATUS_CLOSE;
		}
	}
	else {
		// 看是ORACLE否断开连接
		if (strstr(buf, "ORA-12152") != 0) {
			bNeedReconnect = TRUE;
			m_nStatus = STATUS_CLOSE;
		}
	}
	
	NotifyUiError( hWnd, 1, buf, bNeedReconnect, 0, pParam );
}

void  CMyDatabase::NotifyUiError(HWND hWnd, DWORD dwErrorID, const char * szErrDescription /*= 0*/, BOOL bNeedReconnect /*= FALSE*/, const char * szBatchId /*= 0*/, void * pParam /*= 0*/) {
	DbErrMsg * pDbErrMsg = new DbErrMsg;
	memset(pDbErrMsg, 0, sizeof(DbErrMsg));

	pDbErrMsg->bDisconnected = bNeedReconnect;
	pDbErrMsg->dwErrId = dwErrorID;
	if (szErrDescription) {
		strncpy(pDbErrMsg->szErrDescription, szErrDescription, sizeof(pDbErrMsg->szErrDescription) - 1);
	}	
	if (szBatchId) {
		strncpy(pDbErrMsg->szBatchId, szBatchId, sizeof(pDbErrMsg->szBatchId) - 1);
	}
	pDbErrMsg->pParam = pParam;
	::PostMessage(hWnd, UM_DB_ERR_MSG, (WPARAM)pDbErrMsg, 0);
}

void CMyDatabase::Query(const LmnToolkits::MessageData * pParam) {
	char buf[8192];

	CQueryParam * pQueryParam = (CQueryParam *)pParam;

	if (m_nStatus == STATUS_CLOSE) {
		NotifyUiError(g_hQueryDlg, 1, "数据库没有连接上", TRUE, "");
		return;
	}

	DWORD dwOperatorLen = strlen(pQueryParam->m_szOperator);
	CString strSql;

	if (m_nDbType == DB_TYPE_ORACLE) {
		if (dwOperatorLen == 0) {
			strSql.Format("select a.id, a.procetype,b.stfname,a.procetime,a.tagbaseid,a.proce_batch_id from proceinfo a inner join staff b on a.proceman = b.stfid where a.procetime >= to_date('%s','yyyy-mm-dd hh24:mi:ss') "
				"AND a.procetime <= to_date('%s','yyyy-mm-dd hh24:mi:ss')", pQueryParam->m_szStartTime, pQueryParam->m_szEndTime);
		}
		else {
			strSql.Format("select a.id, a.procetype,b.stfname,a.procetime,a.tagbaseid,a.proce_batch_id from proceinfo a inner join staff b on a.proceman = b.stfid where a.procetime >= to_date('%s','yyyy-mm-dd hh24:mi:ss') "
				"AND a.procetime <= to_date('%s','yyyy-mm-dd hh24:mi:ss') "
				"AND a.procetime like '%%%s%%'", pQueryParam->m_szStartTime, pQueryParam->m_szEndTime, pQueryParam->m_szOperator);
		}
	}
	else {
		if (dwOperatorLen == 0) {
			strSql.Format("select a.id, a.procetype,b.stfname,a.procetime,a.tagbaseid,a.proce_batch_id from proceinfo a inner join staff b on a.proceman = b.stfid WHERE a.procetime >= '%s' "
				"AND a.procetime <= '%s' ", pQueryParam->m_szStartTime, pQueryParam->m_szEndTime );
		}
		else {
			strSql.Format("select a.id, a.procetype,b.stfname,a.procetime,a.tagbaseid,a.proce_batch_id from proceinfo a inner join staff b on a.proceman = b.stfid WHERE a.procetime >= '%s' "
				"AND a.procetime <= '%s' "
				"AND b.stfname like '%%%s%%'", pQueryParam->m_szStartTime, pQueryParam->m_szEndTime, AnsiToUtf8( buf, sizeof(buf), pQueryParam->m_szOperator) );
		}
	}
	
	
	try
	{
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);
		std::vector<QueryResult *> * pvRet = new std::vector<QueryResult *>;

		while (!m_recordset.IsEOF())
		{
			QueryResult * pResult = new QueryResult;
			memset(pResult, 0, sizeof(QueryResult));

			CString  strRet;
			m_recordset.GetFieldValue((short)0, strRet);
			sscanf(strRet, " %lu", &pResult->dwId);

			m_recordset.GetFieldValue((short)1, strRet);
			sscanf(strRet, " %d", &pResult->nProcType);

			m_recordset.GetFieldValue((short)2, strRet);
			if (m_bUtf8) {
				Utf8ToAnsi(buf, sizeof(buf), strRet);
				strncpy(pResult->szOperator, buf, sizeof(pResult->szOperator) - 1);
			}
			else {
				strncpy(pResult->szOperator, strRet, sizeof(pResult->szOperator) - 1);
			}
			

			m_recordset.GetFieldValue((short)3, strRet);
			strncpy(pResult->szProcTime, strRet, sizeof(pResult->szProcTime) - 1);

			m_recordset.GetFieldValue((short)4, strRet);
			pResult->dwTagsCnt = CountChar(strRet, ',') + 1;

			m_recordset.GetFieldValue((short)5, strRet);
			strncpy(pResult->szBatchId, strRet, sizeof(pResult->szBatchId) - 1);

			pvRet->push_back(pResult);

			m_recordset.MoveNext();
		}
		m_recordset.Close();//关闭记录集

		NotifyUiError(g_hQueryDlg, 0, 0, FALSE, 0, pvRet );
	}
	catch (CException* e)
	{
		OnDatabaseException(g_hQueryDlg, e);
	}
}