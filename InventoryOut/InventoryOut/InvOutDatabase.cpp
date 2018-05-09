#include <time.h>
#include "InvOutDatabase.h"

#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")
#pragma comment(lib, "crypt32.lib")

#define  AGENCY_TABLE_NAME          "agencies"
#define  STAFF_TABLE_NAME           "staff"
#define  BIG_PKG_TABLE_NAME         "big_pkg"
#define  SMALL_PKG_TABLE_NAME       "small_pkg"
#define  BIG_OUT_TABLE_NAME         "big_out"
#define  SMALL_OUT_TABLE_NAME       "small_out"

CInvoutDatabase::CInvoutDatabase() {
	
}

CInvoutDatabase::~CInvoutDatabase() {
	
}

// 登录
int CInvoutDatabase::Login( const CLoginParam * pParam, DuiLib::CDuiString & strLoginId, DuiLib::CDuiString & strLoginName ) {
	if ( GetStatus() == STATUS_CLOSE ) {
		return CLmnOdbc::ERROR_DISCONNECTED;
	}

	char buf[8192];
	char szUserName[256];
	char szUserPassword[256];

	String2SqlValue( szUserName,     sizeof(szUserName),     pParam->m_szUserName );
	String2SqlValue( szUserPassword, sizeof(szUserPassword), pParam->m_szUserPassword );

	SNPRINTF(buf, sizeof(buf), "SELECT stfid, stfname FROM staff WHERE ext='%s' AND ext='%s'", szUserName, szUserPassword);

	int ret = -1;

	try
	{
		OpenRecordSet(buf);
	
		if ( MoveNext() ) {
			GetFieldValue(1, buf, sizeof(buf));
			strLoginId = buf;

			GetFieldValue(2, buf, sizeof(buf));
			strLoginName = buf;

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
		EndTran();

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
		EndTran(FALSE);
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
		EndTran();
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
		EndTran(FALSE);
	}

	return ret;
}

// 删除经销商
int  CInvoutDatabase::DeleteAgency(const CAgencyParam * pParam) {
	if (GetStatus() == STATUS_CLOSE) {
		return CLmnOdbc::ERROR_DISCONNECTED;
	}

	int ret = 0;
	char sql[8192];
	char szId[256];

	String2SqlValue( szId, sizeof(szId), pParam->m_tAgency.szId );

	// 先查询出库记录里有无经销商
	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE out_target_type = 1 AND out_target_id='%s' ", BIG_OUT_TABLE_NAME, szId );

	try
	{
		OpenRecordSet(sql);
		// 如果存在记录
		if (MoveNext()) {
			ret = InvOutDbErr_Cannt_delete_foreign_key;
		}
		CloseRecordSet();
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

	if (0 != ret) {
		return ret;
	}

	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE out_target_type = 1 AND out_target_id='%s' ", SMALL_OUT_TABLE_NAME, szId);

	try
	{
		OpenRecordSet(sql);
		// 如果存在记录
		if (MoveNext()) {
			ret = InvOutDbErr_Cannt_delete_foreign_key;
		}
		CloseRecordSet();
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

	if (0 != ret) {
		return ret;
	}


	// 出库数据里无该供应商记录，可以删除

	SNPRINTF(sql, sizeof(sql), "DELETE FROM  %s WHERE id = %lu ", AGENCY_TABLE_NAME, pParam->m_tAgency.dwId );

	try
	{
		Execute(sql);
		EndTran();
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
		EndTran(FALSE);
	}

	return ret;
}


// 获取所有的销售
int  CInvoutDatabase::GetAllSales(std::vector<SaleStaff *> & vRet) {
	if (GetStatus() == STATUS_CLOSE) {
		return CLmnOdbc::ERROR_DISCONNECTED;
	}

	char sql[8192];
	BOOL  bNull = FALSE;

	int ret = -1;
	SNPRINTF(sql, sizeof(sql), "SELECT stfid, stfname FROM %s ", STAFF_TABLE_NAME);

	try
	{
		OpenRecordSet(sql);

		while (MoveNext()) {

			SaleStaff * pItem = new SaleStaff;
			memset(pItem, 0, sizeof(SaleStaff));

			GetFieldValue(1, pItem->szId,   sizeof(pItem->szId),   &bNull);
			GetFieldValue(2, pItem->szName, sizeof(pItem->szName), &bNull);

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

int   CInvoutDatabase::SaveInvOut(const CSaveInvOutParam * pParam) {

	std::vector<Package * > vBig;
	std::vector<Package * > vSmall;
	char sql[8192];
	char buf[8192];
	BOOL  bNull = FALSE;
	int ret = 0;
	std::vector<Package * >::iterator it_x;

	// 查询大包装是否已经入库
	std::vector<DuiLib::CDuiString *>::const_iterator it;
	for (it = pParam->m_vBig.begin(); it != pParam->m_vBig.end(); it++) {
		DuiLib::CDuiString * pStr = *it;
		SNPRINTF(sql, sizeof(sql), "SELECT id, package_id FROM %s WHERE package_id='%s' ", BIG_PKG_TABLE_NAME, (const char*)(*pStr) );

		try
		{
			OpenRecordSet(sql);

			if (MoveNext()) {

				Package * pItem = new Package;
				memset(pItem, 0, sizeof(Package));

				GetFieldValue(1, buf, sizeof(buf), &bNull);
				sscanf(buf, "%lu", &pItem->dwId);

				GetFieldValue(2, pItem->szId, sizeof(pItem->szId), &bNull);

				vBig.push_back(pItem);
			}
			else {
				ret = InvOutDbErr_Not_found;
			}

			CloseRecordSet();
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

		// 如果出现 该大包装号不存在或其他错误
		if (0 != ret) {
			ClearVector(vBig);
			return ret;
		}
	}


	// 查询小包装是否已经入库
	for (it = pParam->m_vSmall.begin(); it != pParam->m_vSmall.end(); it++) {
		DuiLib::CDuiString * pStr = *it;
		SNPRINTF(sql, sizeof(sql), "SELECT id, package_id, big_pkg_id FROM %s WHERE package_id='%s' ", SMALL_PKG_TABLE_NAME, (const char*)(*pStr));

		try
		{
			OpenRecordSet(sql);

			if (MoveNext()) {

				Package * pItem = new Package;
				memset(pItem, 0, sizeof(Package));

				GetFieldValue(1, buf, sizeof(buf), &bNull);
				sscanf(buf, "%lu", &pItem->dwId);

				GetFieldValue(2, pItem->szId, sizeof(pItem->szId), &bNull);

				GetFieldValue(3, buf, sizeof(buf), &bNull);
				sscanf(buf, "%lu", &pItem->dwParentId);

				BOOL bFound = FALSE;
				// 查看小包装是否已经包含在大包装里了
				if (pItem->dwParentId > 0) {
					for (it_x = vBig.begin(); it_x != vBig.end(); it_x++) {
						Package * pBig = *it_x;
						// 确认包含（重复错误）
						if (pItem->dwParentId == pBig->dwId) {
							bFound = TRUE;
							break;
						}
					}
				}
				
				if (bFound) {
					delete pItem;
				}
				else {
					vSmall.push_back(pItem);
				}
			}
			else {
				ret = InvOutDbErr_Not_found;
			}

			CloseRecordSet();
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

		// 如果出现 该小包装号不存在或其他错误
		if (0 != ret) {
			ClearVector(vBig);
			ClearVector(vSmall);
			return ret;
		}
	}

	// 查看小包装的父包装是否出库
	for (it_x = vSmall.begin(); it_x != vSmall.end(); it_x++) {
		Package * pPackage = *it_x;

		BOOL bFound = FALSE;
		if (pPackage->dwParentId > 0) {
			SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE package_id=%lu ", BIG_OUT_TABLE_NAME, pPackage->dwParentId);
			try
			{
				OpenRecordSet(sql);
				if ( MoveNext() ) {
					bFound = TRUE;
				}
				CloseRecordSet();
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
		}

		if (0 != ret) {
			ClearVector(vBig);
			ClearVector(vSmall);
			return ret;
		}

		// 如果找到父包装已经入库
		if ( bFound ) {
			ret = InvOutDbErr_Small_pkg_Parent_pkg_Already_out;
			ClearVector(vBig);
			ClearVector(vSmall);
			return ret;
		}		
	}

	char  szTargetId[32];
	char  szOperatorId[32];

	String2SqlValue(szTargetId,   sizeof(szTargetId),   pParam->m_strTargetId);
	String2SqlValue(szOperatorId, sizeof(szOperatorId), pParam->m_strOperatorId);

	time_t now = time(0);
	char szNow[64] = { 0 };
	DateTime2String(szNow, sizeof(szNow), &now);

	for ( it_x = vBig.begin(); it_x != vBig.end(); it_x++ ) {
		Package * pPackage = *it_x;

		SNPRINTF( sql, sizeof(sql), "INSERT INTO  %s VALUES ( %lu, %d, '%s', '%s', to_date('%s','yyyy-mm-dd hh24:mi:ss') ) ", 
			      BIG_OUT_TABLE_NAME, pPackage->dwId, pParam->m_nTargetType, szTargetId, szOperatorId, szNow );

		try
		{
			Execute(sql);
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

		if (0 != ret) {
			ClearVector(vBig);
			ClearVector(vSmall);
			return ret;
		}
	}

	for (it_x = vSmall.begin(); it_x != vSmall.end(); it_x++) {
		Package * pPackage = *it_x;

		SNPRINTF(sql, sizeof(sql), "INSERT INTO  %s VALUES ( %lu, %d, '%s', '%s', to_date('%s','yyyy-mm-dd hh24:mi:ss') ) ",
			SMALL_OUT_TABLE_NAME, pPackage->dwId, pParam->m_nTargetType, szTargetId, szOperatorId, szNow);

		try
		{
			Execute(sql);
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

		if (0 != ret) {
			ClearVector(vBig);
			ClearVector(vSmall);
			return ret;
		}
	}

	try
	{
		EndTran();
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
		EndTran(FALSE);
	}

	return 0;
}