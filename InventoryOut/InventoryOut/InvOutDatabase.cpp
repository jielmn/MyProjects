#include "InvOutDatabase.h"

#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")
#pragma comment(lib, "crypt32.lib")

#define  AGENCY_TABLE_NAME          "agencies"

CInvoutDatabase::CInvoutDatabase() {
	
}

CInvoutDatabase::~CInvoutDatabase() {
	
}

// 登录
int CInvoutDatabase::Login(const CLoginParam * pParam) {
	if ( GetStatus() == STATUS_CLOSE ) {
		return CLmnOdbc::ERROR_DISCONNECTED;
	}

	char buf[8192];
	char szUserName[256];
	char szUserPassword[256];

	String2SqlValue( szUserName,     sizeof(szUserName),     pParam->m_szUserName );
	String2SqlValue( szUserPassword, sizeof(szUserPassword), pParam->m_szUserPassword );

	SNPRINTF(buf, sizeof(buf), "SELECT * FROM staff WHERE ext='%s' AND ext='%s'", szUserName, szUserPassword);

	int ret = -1;

	try
	{
		OpenRecordSet(buf);
	
		if ( MoveNext() ) {
			ret = 0;
		}

		CloseRecordSet();
	}
	catch (CLmnOdbcException e)
	{
		ret = e.GetError();
		CloseRecordSet();
	}

	return ret;
}

// 添加经销商
int CInvoutDatabase::AddAgency(const CAgencyParam * pParam, DWORD & dwNewId) {
	if (GetStatus() == STATUS_CLOSE) {
		return CLmnOdbc::ERROR_DISCONNECTED;
	}

	char sql[8192];
	char buf[8192];
	BOOL  bNull = FALSE;
	DWORD  dwMaxId = 0;

	AgencyItem   tAgency;

	String2SqlValue(tAgency.szName,         sizeof(tAgency.szName),         pParam->m_tAgency.szName );
	String2SqlValue(tAgency.szId,           sizeof(tAgency.szId),           pParam->m_tAgency.szId );
	String2SqlValue(tAgency.szProvince,     sizeof(tAgency.szProvince),     pParam->m_tAgency.szProvince );

	
	SNPRINTF(sql, sizeof(sql), "SELECT max(id) FROM %s ", AGENCY_TABLE_NAME);

	int ret = -1;

	try
	{
		OpenRecordSet(sql);
		MoveNext();
		GetFieldValue(1, buf, sizeof(buf), &bNull);
		
		if (!bNull) {
			sscanf(buf, "%lu", &dwMaxId);
		}
		CloseRecordSet();

		SNPRINTF(sql, sizeof(sql), "INSERT INTO %s VALUES (%lu, '%s', '%s', '%s') ", AGENCY_TABLE_NAME, (dwMaxId + 1), tAgency.szId, tAgency.szName, tAgency.szProvince);
		Execute(sql);

		dwNewId = dwMaxId + 1;
		ret = 0;
	}
	catch (CLmnOdbcException e)
	{
		const char * pStatus = GetSysStatus();
		if (0 == strcmp(pStatus, "23000")) {
			ret = InvOutDbErr_Integrity_constraint_violation;
		}
		else {
			ret = e.GetError();
		}
		CloseRecordSet();
	}

	return ret;   
}

// 获取所有经销商
int  CInvoutDatabase::GetAllAgency(std::vector<AgencyItem *> & vRet) {
	if (GetStatus() == STATUS_CLOSE) {
		return CLmnOdbc::ERROR_DISCONNECTED;
	}

	char sql[8192];
	char buf[8192];
	BOOL  bNull = FALSE;

	int ret = -1;
	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s ", AGENCY_TABLE_NAME);

	try
	{
		OpenRecordSet(sql);

		while (MoveNext()) {

			AgencyItem * pItem = new AgencyItem;
			memset(pItem, 0, sizeof(AgencyItem));

			GetFieldValue(1, buf, sizeof(buf), &bNull);
			sscanf(buf, "%lu", &pItem->dwId);

			GetFieldValue(2, buf, sizeof(buf), &bNull);
			STRNCPY(pItem->szId, buf, sizeof(pItem->szId));

			GetFieldValue(3, buf, sizeof(buf), &bNull);
			STRNCPY(pItem->szName, buf, sizeof(pItem->szName));

			GetFieldValue(4, buf, sizeof(buf), &bNull);
			STRNCPY(pItem->szProvince, buf, sizeof(pItem->szProvince));

			vRet.push_back(pItem);
		}

		CloseRecordSet();

		ret = 0;
	}
	catch (CLmnOdbcException e)
	{
		const char * pStatus = GetSysStatus(); 
		if (0 == strcmp(pStatus, "23000")) {
			ret = InvOutDbErr_Integrity_constraint_violation;
		}
		else {
			ret = e.GetError();
		}
		CloseRecordSet();
	}

	return ret;
}

// 修改经销商
int  CInvoutDatabase::ModifyAgency(const CAgencyParam * pParam) {
	if (GetStatus() == STATUS_CLOSE) {
		return CLmnOdbc::ERROR_DISCONNECTED;
	}

	char sql[8192];

	AgencyItem   tAgency;
	String2SqlValue(tAgency.szName, sizeof(tAgency.szName), pParam->m_tAgency.szName);
	String2SqlValue(tAgency.szProvince, sizeof(tAgency.szProvince), pParam->m_tAgency.szProvince);
	tAgency.dwId = pParam->m_tAgency.dwId;

	SNPRINTF(sql, sizeof(sql), "UPDATE  %s set name='%s', province='%s' WHERE id = %lu ", AGENCY_TABLE_NAME, tAgency.szName, tAgency.szProvince, tAgency.dwId );

	int ret = -1;
	try
	{
		Execute(sql);
		ret = 0;
	}
	catch (CLmnOdbcException e)
	{
		const char * pStatus = GetSysStatus();
		if (0 == strcmp(pStatus, "23000")) {
			ret = InvOutDbErr_Integrity_constraint_violation;
		}
		else {
			ret = e.GetError();
		}
		CloseRecordSet();
	}

	return ret;
}

// 删除经销商
int  CInvoutDatabase::DeleteAgency(const CAgencyParam * pParam) {
	if (GetStatus() == STATUS_CLOSE) {
		return CLmnOdbc::ERROR_DISCONNECTED;
	}

	char sql[8192];

	SNPRINTF(sql, sizeof(sql), "DELETE FROM  %s WHERE id = %lu ", AGENCY_TABLE_NAME, pParam->m_tAgency.dwId );

	int ret = -1;
	try
	{
		Execute(sql);
		ret = 0;
	}
	catch (CLmnOdbcException e)
	{
		const char * pStatus = GetSysStatus();
		if (0 == strcmp(pStatus, "23000")) {
			ret = InvOutDbErr_Integrity_constraint_violation;
		}
		else {
			ret = e.GetError();
		}
		CloseRecordSet();
	}

	return ret;
}