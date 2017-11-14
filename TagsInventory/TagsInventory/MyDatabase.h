#pragma once

class  CMyDatabase {
public:
	CMyDatabase();
	~CMyDatabase();

	void ReconnectDb();
	int  GetStatus() const;
	void VerifyUser(const TagItem * pItem);
	void SaveInventory(CSaveInvParam * pParam);
	void Query(const LmnToolkits::MessageData * pParam);

private:
	void  OnDatabaseException(HWND hWnd, CException* e, BOOL bNotifyUi = TRUE, void * pParam = 0 );
	void  NotifyUiError( HWND hWnd, DWORD dwErrorID, const char * szErrDescription = 0, BOOL bNeedReconnect = FALSE, const char * szBatchId = 0, void * pParam = 0);

	BOOL                m_bUtf8;
	int                 m_nDbType;

	int                 m_nStatus;
	CDatabase           m_database;
	CRecordset          m_recordset;
};