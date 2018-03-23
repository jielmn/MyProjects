#pragma once
#include <afxdb.h>
#include "InvCommon.h"

class CBusiness;

class  CInvDatabase {
public:
	CInvDatabase( CBusiness * pBusiness );
	~CInvDatabase();

	// �������ݿ�
	int ReconnectDb();
	// �û���¼
	int LoginUser( const CTagItemParam * pItem, User * pUser);
	// ����С�̵�
	int InvSmallSave( const CInvSmallSaveParam * pParam, const char * szUserId, CString & strBatchId);
	// check
	int CheckTag(const CTagItemParam * pItem);
	// ������̵�
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
	BOOL                m_bUtf8;                       // ʹ��utf8�ַ���

	CBusiness  *        m_pBusiness;
};