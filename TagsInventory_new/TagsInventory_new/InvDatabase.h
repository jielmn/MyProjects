#pragma once
#include <afxdb.h>
#include "InvCommon.h"

class CBusiness;

class  CInvDatabase {
public:
	CInvDatabase( CBusiness * pBusiness );
	~CInvDatabase();

	// 重连数据库
	int ReconnectDb();
	// 用户登录
	int LoginUser( const CTagItemParam * pItem, User * pUser);
	// 保存小盘点
	int InvSmallSave( const CInvSmallSaveParam * pParam, const char * szUserId, CString & strBatchId);
	// check
	int CheckTag(const CTagItemParam * pItem);
	// 保存大盘点
	int InvBigSave(const CInvBigSaveParam * pParam, const char * szUserId, CString & strBatchId, CString & strWrongSmallPkgId );

	enum DATABASE_STATUS {
		STATUS_CLOSE = 0,
		STATUS_OPEN,
	};

	// odbc database type
	enum DATABASE_TYPE {
		TYPE_ORACLE = 0,
		TYPE_MYSQL,
	};

	DATABASE_STATUS GetStatus();

private:
	void Clear();
	int  OnDatabaseException(CException* e);

	DATABASE_STATUS     m_eDbStatus;
	DATABASE_TYPE       m_eDbType;
	CDatabase           m_database;
	CRecordset          m_recordset;
	BOOL                m_bUtf8;                       // 使用utf8字符集

	CBusiness  *        m_pBusiness;
};