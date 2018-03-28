#include <afx.h>
#include <assert.h>
#include "InvDatabase.h"
#include "Business.h"
#include "LmnString.h"


//#define  PROCE_TYPE_INV_SMALL                 1

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
			g_cfg->GetConfig( ODBC_STRING_ORACLE, buf, sizeof(buf),"" );
		}
		else {
			g_cfg->GetConfig(ODBC_STRING_MYSQL, buf, sizeof(buf),"");
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

		// 重连数据库
		m_pBusiness->ReconnectDatabaseAsyn(RECONNECT_DB_TIME);
	}

	m_pBusiness->NotifyUiDbStatus(m_eDbStatus);
	return 0;
}



// 用户登录
int CInvDatabase::LoginUser( const CTagItemParam * pItem, User * pUser) {
	CString strSql;
	char buf[8192];

	if ( m_eDbStatus == STATUS_CLOSE ) {
		return INV_ERR_DB_CLOSE;
	}

	int ret = 0;
	try
	{
		GetUid( buf, sizeof(buf), pItem->m_item.abyUid, pItem->m_item.dwUidLen );
		g_log->Output(ILog::LOG_SEVERITY_INFO, "try to login id is %s\n", buf);

		strSql.Format("select * from staff where ext1='%s'", buf);
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);

		// 存在记录，验证OK
		// 获取用户信息
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
		//验证失败
		else {
			ret = 1;
		}
		m_recordset.Close();//关闭记录集
	}
	catch (CException* e)
	{		
		//ret = -2;
		//e->GetErrorMessage(buf, sizeof(buf));
		//g_log->Output(ILog::LOG_SEVERITY_ERROR, buf);
		ret = OnDatabaseException(e);
	}

	// 如果数据库端口，重新连接
	if (m_eDbStatus == STATUS_CLOSE) {
		m_pBusiness->NotifyUiDbStatus(m_eDbStatus);
		m_pBusiness->ReconnectDatabaseAsyn(RECONNECT_DB_TIME);
	}

	return ret;
}

CInvDatabase::DATABASE_STATUS CInvDatabase::GetStatus() {
	return m_eDbStatus;
}

// 保存小盘点
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
	strSql.Format("select max(id) from small_pkg;");
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
		ret = OnDatabaseException(e);
	}

	CString strTmp;
	CString strTmp1;

	// 如果上一步没有错误
	if (0 == ret) {

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
				strSql.Format("insert into tags values('%s',%u);", buf, dwProceId + 1);
				m_database.ExecuteSQL(strSql);
			}

			CString strMaxBatchId;
			now = CTime::GetCurrentTime();
			strTmp.Format("%s%s%s%%", szFactoryId, szProductId, pParam->m_strBatchId);
			strSql.Format("select package_id from small_pkg where package_id like '%s' order by package_id desc;", strTmp);
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
			// 流水号太大
			if ( nFlowId >= MAX_FLOW_NUMBER ) {
				m_database.Rollback();
				ret = INV_ERR_DB_TOO_LARGE_FLOW_NUM;
				return ret;
			}

			strTmp1.Format("%%s%%s%%s%%0%dd", FLOW_NUM_LEN);

			strTmp.Format(strTmp1, szFactoryId, szProductId, pParam->m_strBatchId, nFlowId);
			strBatchId = strTmp;

			if (m_eDbType == TYPE_ORACLE) {
				// 插入批量数据
				strSql.Format("insert into small_pkg values ( %u, '%s', to_date('%s','yyyy-mm-dd hh24:mi:ss'),'%s', 0)",
					dwProceId + 1, szUserId, strTime, strBatchId);
			}
			else {
				// 插入批量数据
				strSql.Format("insert into small_pkg values ( %u, '%s', '%s','%s',0)",
					dwProceId + 1, szUserId, strTime, strBatchId);
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

	// 如果数据库端口，重新连接
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
		if (0 != strstr(buf, "connection") || 0 != strstr(buf, "没有连接") || 0 != strstr(buf, "gone awaw")) {
			m_eDbStatus = STATUS_CLOSE;
			ret = INV_ERR_DB_CLOSE;
		}
	}
	else {
		// 看是ORACLE否断开连接
		if (strstr(buf, "ORA-12152") != 0) {
			m_eDbStatus = STATUS_CLOSE;
			ret = INV_ERR_DB_CLOSE;
		}
		else if (strstr(buf, "ORA-00001") != 0) {
			ret = INV_ERR_DB_NOT_UNIQUE;
		} else if (strstr(buf, "ORA-03135") != 0) {
			m_eDbStatus = STATUS_CLOSE;
			ret = INV_ERR_DB_CLOSE;
		}
	}

	return ret;
}


// check
int  CInvDatabase::CheckTag(const CTagItemParam * pItem) {
	CString strSql;
	char buf[8192];

	if (m_eDbStatus == STATUS_CLOSE) {
		return INV_ERR_DB_CLOSE;
	}

	int ret = 0;
	try
	{
		GetUid(buf, sizeof(buf), pItem->m_item.abyUid, pItem->m_item.dwUidLen);

		strSql.Format("select * from tags where id='%s'", buf);
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);

		// 存在记录
		if (!m_recordset.IsEOF())
		{
			ret = 1;
		}
		else {
			ret = 0;
		}
		m_recordset.Close();//关闭记录集
	}
	catch (CException* e)
	{
		ret = OnDatabaseException(e);
		//e->GetErrorMessage(buf, sizeof(buf));
		//g_log->Output(ILog::LOG_SEVERITY_ERROR, buf);
	}

	// 如果数据库端口，重新连接
	if (m_eDbStatus == STATUS_CLOSE) {
		m_pBusiness->NotifyUiDbStatus(m_eDbStatus);
		m_pBusiness->ReconnectDatabaseAsyn(RECONNECT_DB_TIME);
	}

	return ret;
}


int CInvDatabase::InvBigSave(const CInvBigSaveParam * pParam, const char * szUserId, CString & strBatchId, CString & strWrongSmallPkgId ) {
	int ret = 0;

	if (m_eDbStatus == STATUS_CLOSE) {
		return INV_ERR_DB_CLOSE;
	}

	CTime now = CTime::GetCurrentTime();
	CString strTime = now.Format("%Y-%m-%d %H:%M:%S");

	char  szFactoryId[64];
	char  szProductId[64];

	g_cfg->GetConfig(FACTORY_CODE, szFactoryId, sizeof(szFactoryId));
	g_cfg->GetConfig(PRODUCT_CODE, szProductId, sizeof(szProductId));

	//char buf[8192];
	std::vector<CString *>::const_iterator it;

	CString  strSql;
	strSql.Format("select max(id) from big_pkg;");
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
		ret = OnDatabaseException(e);
	}

	CString strTmp;
	CString strTmp1;
	int nTemp = 0;

	// 如果上一步没有错误
	if (0 == ret) {

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

			// 检查条码是否正确
			for (it = pParam->m_items.begin(); it != pParam->m_items.end(); it++) {
				CString * pIem = *it;
				BOOL  bCheckOk = FALSE;
				strSql.Format("select id, package_id, big_pkg_id from small_pkg where package_id='%s';", *pIem );

				m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);
				if (!m_recordset.IsEOF())
				{
					CString   strValue;
					DWORD     dwBigPkgId = 0;
					m_recordset.GetFieldValue((short)2, strValue);
					sscanf(strValue, "%u", &dwBigPkgId);
					// 大包装不为0，说明已经装入大包装, ERROR
					if ( dwBigPkgId != 0 ) {
						ret = INV_ERR_DB_SMALL_PKG_IN_USE;
					}
				}
				else {
					ret = INV_ERR_DB_SMALL_PKG_NOT_FOUND;
				}
				m_recordset.Close();//关闭记录集

				if ( ret != 0 ) {
					strWrongSmallPkgId = *pIem;
					m_database.Rollback();
					return ret;
				}
			}

			CString strMaxBatchId;
			now = CTime::GetCurrentTime();
			strTmp.Format("%s%s%s%%", szFactoryId, szProductId, pParam->m_strBatchId);
			strSql.Format("select package_id from big_pkg where package_id like '%s' order by package_id desc;", strTmp);
			m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);
			if (!m_recordset.IsEOF())
			{
				m_recordset.GetFieldValue((short)0, strMaxBatchId);
			}
			m_recordset.Close();//关闭记录集

			assert( FLOW_NUM_LEN > 1);
			int  nFlowId = 0;
			char ch = 0;
			if (strMaxBatchId.GetLength() > 0) {
				DWORD dwFactoryLen = strlen(szFactoryId);
				DWORD dwProcCodeLen = strlen(szProductId);
				nTemp = sscanf( strMaxBatchId.Mid( dwFactoryLen + dwProcCodeLen + pParam->m_strBatchId.GetLength() ), "%c%d", &ch, &nFlowId);
				if (2 != nTemp) {
					ch = 'A';
					nFlowId = 0;
				}
				else {
					ch = Char2Upper(ch);
					// 如果字符不是 a ~ z
					if ( !(ch >= 'A' && ch <= 'Z') ) {
						ch = 'A';
						nFlowId = 0;
					}
				}
			}
			else {
				ch = 'A';
				nFlowId = 0;
			}

			nFlowId++;
			if ( nFlowId >= MAX_FLOW_NUMBER_BIG ) {
				ch++;
				nFlowId = 1;
				// 超出范围
				if ( ch > 'Z' ) {
					m_database.Rollback();
					ret = INV_ERR_DB_TOO_LARGE_FLOW_NUM;
					return ret;
				}				
			}

			strTmp1.Format("%%s%%s%%s%%c%%0%dd", FLOW_NUM_LEN - 1 );

			strTmp.Format(strTmp1, szFactoryId, szProductId, pParam->m_strBatchId, ch, nFlowId );
			strBatchId = strTmp;

			if (m_eDbType == TYPE_ORACLE) {
				// 插入批量数据
				strSql.Format("insert into big_pkg values ( %u, '%s', to_date('%s','yyyy-mm-dd hh24:mi:ss'), '%s' )",
					dwProceId + 1, szUserId, strTime, strBatchId);
			}
			else {
				// 插入批量数据
				strSql.Format("insert into big_pkg values ( %u, '%s', '%s','%s' )",
					dwProceId + 1, szUserId, strTime, strBatchId);
			}

			m_database.ExecuteSQL(strSql);


			// 修改小包装的大包装号
			for (it = pParam->m_items.begin(); it != pParam->m_items.end(); it++) {
				CString * pIem = *it;
				strSql.Format("update small_pkg set big_pkg_id = %d where package_id='%s';", dwProceId + 1, *pIem );
				m_database.ExecuteSQL(strSql);				
			}
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

	// 如果数据库端口，重新连接
	if (m_eDbStatus == STATUS_CLOSE) {
		m_pBusiness->NotifyUiDbStatus(m_eDbStatus);
		m_pBusiness->ReconnectDatabaseAsyn(RECONNECT_DB_TIME);
	}

	return ret;
}



int CInvDatabase::Query(const CQueryParam * pParam, std::vector<QueryResultItem *> & vRet ) {
	assert(pParam);

	int ret = 0;

	if (m_eDbStatus == STATUS_CLOSE) {
		return INV_ERR_DB_CLOSE;
	}

	CString  strSql;
	CString  strWhere;

	// 查询小包装
	if (0 == pParam->m_nQueryType) {
		strSql.Format("select a.procetime, a.package_id, b.stfname from small_pkg a inner join staff b on a.staff_id = b.stfid where ");
	}
	// 查大包装
	else {
		strSql.Format("select a.procetime, a.package_id, b.stfname from big_pkg a inner join staff b on a.staff_id = b.stfid where ");
	}

	// 起始时间子句
	CTime  s1(pParam->m_tStart);
	CTime  s2(pParam->m_tEnd);

	CString  strStart, strEnd;
	strStart = s1.Format("%Y-%m-%d 00:00:00");
	strEnd = s2.Format("%Y-%m-%d 23:59:59");

	strWhere += "a.procetime >= to_date('" + strStart + "', 'yyyy-mm-dd hh24:mi:ss') AND a.procetime <= to_date('" + strEnd + "', 'yyyy-mm-dd hh24:mi:ss') ";


	if (pParam->m_strBatchId.GetLength() > 0) {
		strWhere += "AND a.package_id like '%" + pParam->m_strBatchId + "%' ";
	}

	if (pParam->m_strOperator.GetLength() > 0) {
		strWhere += "AND b.stfname like '%" + pParam->m_strOperator + "%' ";
	}

	strSql += strWhere;

	try
	{
		m_recordset.Open(CRecordset::forwardOnly, strSql, CRecordset::readOnly);
		while ( !m_recordset.IsEOF() )
		{
			CString  strRet;

			QueryResultItem * pResult = new QueryResultItem;
			memset(pResult, 0, sizeof(QueryResultItem));
			
			m_recordset.GetFieldValue((short)0, strRet);
			strncpy(pResult->szProcTime, strRet, sizeof(pResult->szProcTime) - 1);

			m_recordset.GetFieldValue((short)1, strRet);
			strncpy(pResult->szBatchId, strRet, sizeof(pResult->szBatchId) - 1);

			m_recordset.GetFieldValue((short)2, strRet);
			strncpy(pResult->szOperator, strRet, sizeof(pResult->szOperator) - 1);

			vRet.push_back(pResult);

			m_recordset.MoveNext();
		}
		m_recordset.Close();
	}
	catch (CException* e)
	{
		ret = OnDatabaseException(e);
	}

	if (m_eDbStatus == STATUS_CLOSE) {
		m_pBusiness->NotifyUiDbStatus(m_eDbStatus);
		m_pBusiness->ReconnectDatabaseAsyn(RECONNECT_DB_TIME);
	}
	

	

	return ret;
}