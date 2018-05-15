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


// 根据时间查询
int   CInvoutDatabase::QueryByTime(const CQueryByTimeParam * pParam, std::vector<QueryByTimeItem*> & vRet ) {
	if (GetStatus() == STATUS_CLOSE) {
		return CLmnOdbc::ERROR_DISCONNECTED;
	}

	char sql[8192];
	char buf[8192];
	char szWhere[8192];
	BOOL  bNull = FALSE;
	char szStartTime[256];
	char szEndTime[256];
	time_t tEnd = pParam->m_tEnd + 3600 * 24;
	char szTargetId[256];

	int ret = 0;

	DateTime2String( szStartTime, sizeof(szStartTime), &pParam->m_tStart );
	DateTime2String( szEndTime,   sizeof(szEndTime),   &tEnd );
	String2SqlValue( szTargetId, sizeof(szTargetId), pParam->m_szTargetId );

	if (pParam->m_nTargetType == -1) {
		SNPRINTF(szWhere, sizeof(szWhere), "b.operate_time >= to_date( '%s', 'yyyy-mm-dd hh24:mi:ss' ) AND b.operate_time < to_date( '%s', 'yyyy-mm-dd hh24:mi:ss' ) ", szStartTime, szEndTime);

		// 大包装开始
		SNPRINTF(sql, sizeof(sql), " select a.package_id, b.out_target_type, c.name, d.stfname, b.operate_time from %s a "
			" inner join %s b on a.id = b.package_id"
			" inner join %s c on b.out_target_id = c.str_id"
			" inner join %s d on b.operator = d.stfid"
			" where b.out_target_type = 1 AND %s; ", BIG_PKG_TABLE_NAME, BIG_OUT_TABLE_NAME, AGENCY_TABLE_NAME, STAFF_TABLE_NAME, szWhere);
		try
		{
			OpenRecordSet(sql);

			while (MoveNext()) {

				QueryByTimeItem * pItem = new QueryByTimeItem;
				memset(pItem, 0, sizeof(QueryByTimeItem));

				pItem->m_nPackageType = PACKAGE_TYPE_BIG;

				GetFieldValue(1, pItem->m_szPackageId, sizeof(pItem->m_szPackageId), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(2, buf, sizeof(buf), &bNull);
				sscanf(buf, "%d", &pItem->m_nTargetType);

				GetFieldValue(3, pItem->m_szTargetName, sizeof(pItem->m_szTargetName), &bNull);

				GetFieldValue(4, pItem->m_szOperatorName, sizeof(pItem->m_szOperatorName), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(5, buf, sizeof(buf), &bNull);
				pItem->m_tOperatorTime = String2DateTime(buf);

				vRet.push_back(pItem);
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
			ClearVector(vRet);
			return ret;
		}

		SNPRINTF(sql, sizeof(sql), " select a.package_id, b.out_target_type, c.stfname, d.stfname, b.operate_time from %s a "
			" inner join %s b on a.id = b.package_id"
			" inner join %s c on b.out_target_id = c.stfid"
			" inner join %s d on b.operator = d.stfid"
			" where b.out_target_type = 0 AND %s; ", BIG_PKG_TABLE_NAME, BIG_OUT_TABLE_NAME, STAFF_TABLE_NAME, STAFF_TABLE_NAME, szWhere);
		try
		{
			OpenRecordSet(sql);

			while (MoveNext()) {

				QueryByTimeItem * pItem = new QueryByTimeItem;
				memset(pItem, 0, sizeof(QueryByTimeItem));

				pItem->m_nPackageType = PACKAGE_TYPE_BIG;

				GetFieldValue(1, pItem->m_szPackageId, sizeof(pItem->m_szPackageId), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(2, buf, sizeof(buf), &bNull);
				sscanf(buf, "%d", &pItem->m_nTargetType);

				GetFieldValue(3, pItem->m_szTargetName, sizeof(pItem->m_szTargetName), &bNull);

				GetFieldValue(4, pItem->m_szOperatorName, sizeof(pItem->m_szOperatorName), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(5, buf, sizeof(buf), &bNull);
				pItem->m_tOperatorTime = String2DateTime(buf);

				vRet.push_back(pItem);
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
			ClearVector(vRet);
			return ret;
		}


		// 小包装开始
		SNPRINTF(sql, sizeof(sql), " select a.package_id, b.out_target_type, c.name, d.stfname, b.operate_time from %s a "
			" inner join %s b on a.id = b.package_id"
			" inner join %s c on b.out_target_id = c.str_id"
			" inner join %s d on b.operator = d.stfid"
			" where b.out_target_type = 1 AND %s; ", SMALL_PKG_TABLE_NAME, SMALL_OUT_TABLE_NAME, AGENCY_TABLE_NAME, STAFF_TABLE_NAME, szWhere);

		try
		{
			OpenRecordSet(sql);

			while (MoveNext()) {

				QueryByTimeItem * pItem = new QueryByTimeItem;
				memset(pItem, 0, sizeof(QueryByTimeItem));

				pItem->m_nPackageType = PACKAGE_TYPE_SMALL;

				GetFieldValue(1, pItem->m_szPackageId, sizeof(pItem->m_szPackageId), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(2, buf, sizeof(buf), &bNull);
				sscanf(buf, "%d", &pItem->m_nTargetType);

				GetFieldValue(3, pItem->m_szTargetName, sizeof(pItem->m_szTargetName), &bNull);

				GetFieldValue(4, pItem->m_szOperatorName, sizeof(pItem->m_szOperatorName), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(5, buf, sizeof(buf), &bNull);
				pItem->m_tOperatorTime = String2DateTime(buf);

				vRet.push_back(pItem);
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
			ClearVector(vRet);
			return ret;
		}



		SNPRINTF(sql, sizeof(sql), " select a.package_id, b.out_target_type, c.stfname, d.stfname, b.operate_time from %s a "
			" inner join %s b on a.id = b.package_id"
			" inner join %s c on b.out_target_id = c.stfid"
			" inner join %s d on b.operator = d.stfid"
			" where b.out_target_type = 0 AND %s; ", SMALL_PKG_TABLE_NAME, SMALL_OUT_TABLE_NAME, STAFF_TABLE_NAME, STAFF_TABLE_NAME, szWhere);
		try
		{
			OpenRecordSet(sql);

			while (MoveNext()) {

				QueryByTimeItem * pItem = new QueryByTimeItem;
				memset(pItem, 0, sizeof(QueryByTimeItem));

				pItem->m_nPackageType = PACKAGE_TYPE_SMALL;

				GetFieldValue(1, pItem->m_szPackageId, sizeof(pItem->m_szPackageId), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(2, buf, sizeof(buf), &bNull);
				sscanf(buf, "%d", &pItem->m_nTargetType);

				GetFieldValue(3, pItem->m_szTargetName, sizeof(pItem->m_szTargetName), &bNull);

				GetFieldValue(4, pItem->m_szOperatorName, sizeof(pItem->m_szOperatorName), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(5, buf, sizeof(buf), &bNull);
				pItem->m_tOperatorTime = String2DateTime(buf);

				vRet.push_back(pItem);
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
			ClearVector(vRet);
			return ret;
		}
	} 
	// 根据out target查询
	else if (pParam->m_nTargetType == TARGET_TYPE_SALES) {
		SNPRINTF(szWhere, sizeof(szWhere), " b.operate_time >= to_date( '%s', 'yyyy-mm-dd hh24:mi:ss' )"
			                               " AND b.operate_time < to_date( '%s', 'yyyy-mm-dd hh24:mi:ss' ) "
			                               " AND b.out_target_id = '%s' ", szStartTime, szEndTime, szTargetId );

		// 大包装开始
		SNPRINTF(sql, sizeof(sql), " select a.package_id, b.out_target_type, c.stfname, d.stfname, b.operate_time from %s a "
			" inner join %s b on a.id = b.package_id"
			" inner join %s c on b.out_target_id = c.stfid"
			" inner join %s d on b.operator = d.stfid"
			" where b.out_target_type = 0 AND %s; ", BIG_PKG_TABLE_NAME, BIG_OUT_TABLE_NAME, STAFF_TABLE_NAME, STAFF_TABLE_NAME, szWhere);
		try
		{
			OpenRecordSet(sql);

			while (MoveNext()) {

				QueryByTimeItem * pItem = new QueryByTimeItem;
				memset(pItem, 0, sizeof(QueryByTimeItem));

				pItem->m_nPackageType = PACKAGE_TYPE_BIG;

				GetFieldValue(1, pItem->m_szPackageId, sizeof(pItem->m_szPackageId), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(2, buf, sizeof(buf), &bNull);
				sscanf(buf, "%d", &pItem->m_nTargetType);

				GetFieldValue(3, pItem->m_szTargetName, sizeof(pItem->m_szTargetName), &bNull);

				GetFieldValue(4, pItem->m_szOperatorName, sizeof(pItem->m_szOperatorName), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(5, buf, sizeof(buf), &bNull);
				pItem->m_tOperatorTime = String2DateTime(buf);

				vRet.push_back(pItem);
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
			ClearVector(vRet);
			return ret;
		}


		// 小包装开始
		SNPRINTF(sql, sizeof(sql), " select a.package_id, b.out_target_type, c.stfname, d.stfname, b.operate_time from %s a "
			" inner join %s b on a.id = b.package_id"
			" inner join %s c on b.out_target_id = c.stfid"
			" inner join %s d on b.operator = d.stfid"
			" where b.out_target_type = 0 AND %s; ", SMALL_PKG_TABLE_NAME, SMALL_OUT_TABLE_NAME, STAFF_TABLE_NAME, STAFF_TABLE_NAME, szWhere);
		try
		{
			OpenRecordSet(sql);

			while (MoveNext()) {

				QueryByTimeItem * pItem = new QueryByTimeItem;
				memset(pItem, 0, sizeof(QueryByTimeItem));

				pItem->m_nPackageType = PACKAGE_TYPE_SMALL;

				GetFieldValue(1, pItem->m_szPackageId, sizeof(pItem->m_szPackageId), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(2, buf, sizeof(buf), &bNull);
				sscanf(buf, "%d", &pItem->m_nTargetType);

				GetFieldValue(3, pItem->m_szTargetName, sizeof(pItem->m_szTargetName), &bNull);

				GetFieldValue(4, pItem->m_szOperatorName, sizeof(pItem->m_szOperatorName), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(5, buf, sizeof(buf), &bNull);
				pItem->m_tOperatorTime = String2DateTime(buf);

				vRet.push_back(pItem);
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
			ClearVector(vRet);
			return ret;
		}
	}
	else if (pParam->m_nTargetType == TARGET_TYPE_AGENCIES) {
		SNPRINTF(szWhere, sizeof(szWhere), " b.operate_time >= to_date( '%s', 'yyyy-mm-dd hh24:mi:ss' )"
			                               " AND b.operate_time < to_date( '%s', 'yyyy-mm-dd hh24:mi:ss' ) "
			                               " AND b.out_target_id = '%s' ", szStartTime, szEndTime, szTargetId );

		// 大包装开始
		SNPRINTF(sql, sizeof(sql), " select a.package_id, b.out_target_type, c.name, d.stfname, b.operate_time from %s a "
			" inner join %s b on a.id = b.package_id"
			" inner join %s c on b.out_target_id = c.str_id"
			" inner join %s d on b.operator = d.stfid"
			" where b.out_target_type = 1 AND %s; ", BIG_PKG_TABLE_NAME, BIG_OUT_TABLE_NAME, AGENCY_TABLE_NAME, STAFF_TABLE_NAME, szWhere);
		try
		{
			OpenRecordSet(sql);

			while (MoveNext()) {

				QueryByTimeItem * pItem = new QueryByTimeItem;
				memset(pItem, 0, sizeof(QueryByTimeItem));

				pItem->m_nPackageType = PACKAGE_TYPE_BIG;

				GetFieldValue(1, pItem->m_szPackageId, sizeof(pItem->m_szPackageId), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(2, buf, sizeof(buf), &bNull);
				sscanf(buf, "%d", &pItem->m_nTargetType);

				GetFieldValue(3, pItem->m_szTargetName, sizeof(pItem->m_szTargetName), &bNull);

				GetFieldValue(4, pItem->m_szOperatorName, sizeof(pItem->m_szOperatorName), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(5, buf, sizeof(buf), &bNull);
				pItem->m_tOperatorTime = String2DateTime(buf);

				vRet.push_back(pItem);
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
			ClearVector(vRet);
			return ret;
		}

		SNPRINTF(sql, sizeof(sql), " select a.package_id, b.out_target_type, c.name, d.stfname, b.operate_time from %s a "
			" inner join %s b on a.id = b.package_id"
			" inner join %s c on b.out_target_id = c.str_id"
			" inner join %s d on b.operator = d.stfid"
			" where b.out_target_type = 1 AND %s; ", SMALL_PKG_TABLE_NAME, SMALL_OUT_TABLE_NAME, AGENCY_TABLE_NAME, STAFF_TABLE_NAME, szWhere);

		try
		{
			OpenRecordSet(sql);

			while (MoveNext()) {

				QueryByTimeItem * pItem = new QueryByTimeItem;
				memset(pItem, 0, sizeof(QueryByTimeItem));

				pItem->m_nPackageType = PACKAGE_TYPE_SMALL;

				GetFieldValue(1, pItem->m_szPackageId, sizeof(pItem->m_szPackageId), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(2, buf, sizeof(buf), &bNull);
				sscanf(buf, "%d", &pItem->m_nTargetType);

				GetFieldValue(3, pItem->m_szTargetName, sizeof(pItem->m_szTargetName), &bNull);

				GetFieldValue(4, pItem->m_szOperatorName, sizeof(pItem->m_szOperatorName), &bNull);

				memset(buf, 0, sizeof(buf));
				GetFieldValue(5, buf, sizeof(buf), &bNull);
				pItem->m_tOperatorTime = String2DateTime(buf);

				vRet.push_back(pItem);
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
			ClearVector(vRet);
			return ret;
		}

	}
	else {
		assert(0);
	}
	
	return ret;
}
   

// 根据大包装查询
int   CInvoutDatabase::QueryByBigPkg( const CQueryByBigPkgParam * pParam , std::vector<PkgItem*> & vBig, std::vector<PkgItem*> & vSmall) {

	if ( GetStatus() == STATUS_CLOSE ) {
		return CLmnOdbc::ERROR_DISCONNECTED;
	}

	char sql[8192];
	char buf[8192];
	char szBigPkgId[256];
	int ret = 0;
	BOOL bNull = FALSE;

	String2SqlValue(szBigPkgId, sizeof(szBigPkgId), pParam->m_szBigPkgId);

	SNPRINTF(sql, sizeof(sql), " select a.id, a.PACKAGE_ID, c.stfname, a.procetime, b.OUT_TARGET_TYPE, e.stfname, d.stfname,  b.OPERATE_TIME from %s a"
		                       " left join %s b on a.id = b.package_id"
		                       " left join %s c on a.staff_id = c.STFID"
		                       " left join %s d on b.operator = d.STFID"
		                       " left join %s e on b.OUT_TARGET_ID = e.STFID"
	                           " where (b.OUT_TARGET_TYPE is null OR b.OUT_TARGET_TYPE = 0) AND a.package_id like '%%%s%%'; ", 
		                       BIG_PKG_TABLE_NAME, BIG_OUT_TABLE_NAME, STAFF_TABLE_NAME, STAFF_TABLE_NAME, STAFF_TABLE_NAME, szBigPkgId );
	try
	{
		OpenRecordSet(sql);

		while (MoveNext()) {

			PkgItem * pItem = new PkgItem;
			memset(pItem, 0, sizeof(PkgItem));

			memset(buf, 0, sizeof(buf));
			GetFieldValue( 1, buf, sizeof(buf), &bNull );
			sscanf( buf, "%lu", &pItem->dwPkgId );

			GetFieldValue(2, pItem->szId, sizeof(pItem->szId), &bNull);

			GetFieldValue(3, pItem->szInOperator, sizeof(pItem->szInOperator), &bNull);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(4, buf, sizeof(buf), &bNull);
			pItem->tInTime = String2DateTime(buf);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(5, buf, sizeof(buf), &bNull);
			if (bNull) {
				pItem->nOutTargetType = -1;
			}
			else {
				sscanf(buf, "%d", &pItem->nOutTargetType);
			}
			
			GetFieldValue(6, pItem->szOutTargetName, sizeof(pItem->szOutTargetName), &bNull);

			GetFieldValue(7, pItem->szOutOperator, sizeof(pItem->szOutOperator), &bNull);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(8, buf, sizeof(buf), &bNull);
			pItem->tOutTime = String2DateTime(buf);

			vBig.push_back(pItem);
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
		ClearVector(vBig);
		return ret;
	}


	SNPRINTF(sql, sizeof(sql), " select a.id, a.PACKAGE_ID, c.stfname, a.procetime, b.OUT_TARGET_TYPE, e.name, d.stfname,  b.OPERATE_TIME from %s a"
		                       " inner join %s b on a.id = b.package_id"
		                       " inner join %s c on a.staff_id = c.STFID"
		                       " inner join %s d on b.operator = d.STFID"
		                       " inner join %s e on b.OUT_TARGET_ID = e.str_id"
	                           " where b.OUT_TARGET_TYPE = 1 AND a.package_id like '%%%s%%'; ",
		                       BIG_PKG_TABLE_NAME, BIG_OUT_TABLE_NAME, STAFF_TABLE_NAME, STAFF_TABLE_NAME, AGENCY_TABLE_NAME, szBigPkgId);
	try
	{
		OpenRecordSet(sql);

		while (MoveNext()) {

			PkgItem * pItem = new PkgItem;
			memset(pItem, 0, sizeof(PkgItem));

			memset(buf, 0, sizeof(buf));
			GetFieldValue(1, buf, sizeof(buf), &bNull);
			sscanf(buf, "%lu", &pItem->dwPkgId);

			GetFieldValue(2, pItem->szId, sizeof(pItem->szId), &bNull);

			GetFieldValue(3, pItem->szInOperator, sizeof(pItem->szInOperator), &bNull);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(4, buf, sizeof(buf), &bNull);
			pItem->tInTime = String2DateTime(buf);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(5, buf, sizeof(buf), &bNull);
			sscanf(buf, "%d", &pItem->nOutTargetType);

			GetFieldValue(6, pItem->szOutTargetName, sizeof(pItem->szOutTargetName), &bNull);

			GetFieldValue(7, pItem->szOutOperator, sizeof(pItem->szOutOperator), &bNull);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(8, buf, sizeof(buf), &bNull);
			pItem->tOutTime = String2DateTime(buf);

			vBig.push_back(pItem);
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
		ClearVector(vBig);
		return ret;
	}

	// 如果查询的大包装记录个数为0
	if ( 0 == vBig.size() ) {
		return ret;
	}

	// 如果大包装个数大于1个
	if ( vBig.size() > 1 ) {

		std::vector<PkgItem*>::iterator it;
		for (it = vBig.begin(); it != vBig.end(); ++it) {
			PkgItem* pItem = *it;
			// 已经出库
			if (pItem->nOutTargetType != -1) {
				pItem->dwStatus = PKG_STATUS_OUT;
				continue;
			}

			// 遍历每个大包装的子包装，看其有无出库
			SNPRINTF(sql, sizeof(sql), " select count(*) from %s a inner join %s b on a.id = b.package_id where a.BIG_PKG_ID = %lu;  ",
				SMALL_PKG_TABLE_NAME, SMALL_OUT_TABLE_NAME, pItem->dwPkgId );
			try
			{
				OpenRecordSet(sql);

				if (MoveNext()) {

					DWORD  dwCount = 0;
					memset(buf, 0, sizeof(buf));
					GetFieldValue(1, buf, sizeof(buf), &bNull);
					sscanf(buf, "%lu", &dwCount);

					if (0 == dwCount) {
						pItem->dwStatus = PKG_STATUS_NOT_OUT;
					}
					else {
						pItem->dwStatus = PKG_STATUS_HALF_OUT;
					}
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
				ClearVector(vBig);
				return ret;
			}			
		}		
		return ret;
	}

	// 只有一个结果的情况下，查询其小包装
	PkgItem* pBigPkg = vBig.at(0);
	// 如果大包装没有出库，查询其子包装有无出库记录
	// 如果已经出库，则查询结束
	if ( pBigPkg->nOutTargetType != -1 ) {
		pBigPkg->dwStatus = PKG_STATUS_OUT;
		return ret;
	}

	BOOL  bFindChildrenOut = FALSE;

	SNPRINTF(sql, sizeof(sql), " select a.id, a.PACKAGE_ID, c.stfname, a.procetime, b.OUT_TARGET_TYPE, e.stfname, d.stfname,  b.OPERATE_TIME from %s a"
		                       " left join %s b on a.id = b.package_id"
		                       " left join %s c on a.staff_id = c.STFID"
		                       " left join %s d on b.operator = d.STFID"
		                       " left join %s e on b.OUT_TARGET_ID = e.STFID"
	                           " where (b.OUT_TARGET_TYPE is null OR b.OUT_TARGET_TYPE = 0) AND a.big_pkg_id = %lu;  ",
		                       SMALL_PKG_TABLE_NAME, SMALL_OUT_TABLE_NAME, STAFF_TABLE_NAME, STAFF_TABLE_NAME, STAFF_TABLE_NAME, pBigPkg->dwPkgId );
	try
	{
		OpenRecordSet(sql);

		while (MoveNext()) {

			PkgItem * pItem = new PkgItem;
			memset(pItem, 0, sizeof(PkgItem));

			memset(buf, 0, sizeof(buf));
			GetFieldValue(1, buf, sizeof(buf), &bNull);
			sscanf(buf, "%lu", &pItem->dwPkgId);

			GetFieldValue(2, pItem->szId, sizeof(pItem->szId), &bNull);

			GetFieldValue(3, pItem->szInOperator, sizeof(pItem->szInOperator), &bNull);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(4, buf, sizeof(buf), &bNull);
			pItem->tInTime = String2DateTime(buf);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(5, buf, sizeof(buf), &bNull);
			if (bNull) {
				pItem->nOutTargetType = -1;
				pItem->dwStatus = PKG_STATUS_NOT_OUT;
			}
			else {
				sscanf(buf, "%d", &pItem->nOutTargetType);
				pItem->dwStatus = PKG_STATUS_OUT;
				bFindChildrenOut = TRUE;
			}

			GetFieldValue(6, pItem->szOutTargetName, sizeof(pItem->szOutTargetName), &bNull);

			GetFieldValue(7, pItem->szOutOperator, sizeof(pItem->szOutOperator), &bNull);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(8, buf, sizeof(buf), &bNull);
			pItem->tOutTime = String2DateTime(buf);

			vSmall.push_back(pItem);
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
		ClearVector(vBig);
		ClearVector(vSmall);
		return ret;
	}


	SNPRINTF(sql, sizeof(sql), " select a.id, a.PACKAGE_ID, c.stfname, a.procetime, b.OUT_TARGET_TYPE, e.name, d.stfname,  b.OPERATE_TIME from %s a"
		                       " inner join %s b on a.id = b.package_id"
		                       " inner join %s c on a.staff_id = c.STFID"
		                       " inner join %s d on b.operator = d.STFID"
		                       " inner join %s e on b.OUT_TARGET_ID = e.str_id"
	                           " where b.OUT_TARGET_TYPE = 1 AND a.big_pkg_id = %lu; ",
		                       SMALL_PKG_TABLE_NAME, SMALL_OUT_TABLE_NAME, STAFF_TABLE_NAME, STAFF_TABLE_NAME, AGENCY_TABLE_NAME, pBigPkg->dwPkgId);
	try
	{
		OpenRecordSet(sql);

		while (MoveNext()) {

			PkgItem * pItem = new PkgItem;
			memset(pItem, 0, sizeof(PkgItem));

			memset(buf, 0, sizeof(buf));
			GetFieldValue(1, buf, sizeof(buf), &bNull);
			sscanf(buf, "%lu", &pItem->dwPkgId);

			GetFieldValue(2, pItem->szId, sizeof(pItem->szId), &bNull);

			GetFieldValue(3, pItem->szInOperator, sizeof(pItem->szInOperator), &bNull);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(4, buf, sizeof(buf), &bNull);
			pItem->tInTime = String2DateTime(buf);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(5, buf, sizeof(buf), &bNull);
			sscanf(buf, "%d", &pItem->nOutTargetType);
			pItem->dwStatus = PKG_STATUS_OUT;
			bFindChildrenOut = TRUE;

			GetFieldValue(6, pItem->szOutTargetName, sizeof(pItem->szOutTargetName), &bNull);

			GetFieldValue(7, pItem->szOutOperator, sizeof(pItem->szOutOperator), &bNull);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(8, buf, sizeof(buf), &bNull);
			pItem->tOutTime = String2DateTime(buf);

			vSmall.push_back(pItem);
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
		ClearVector(vBig);
		ClearVector(vSmall);
		return ret;
	}

	if (bFindChildrenOut) {
		pBigPkg->dwStatus = PKG_STATUS_HALF_OUT;
	}
	else {
		pBigPkg->dwStatus = PKG_STATUS_NOT_OUT;
	}

	return 0;
}

int   CInvoutDatabase::QueryBySmallPkg(const CQueryBySmallPkgParam * pParam, std::vector<PkgItem*> & vSmall) {

	if (GetStatus() == STATUS_CLOSE) {
		return CLmnOdbc::ERROR_DISCONNECTED;
	}

	char sql[8192];
	char buf[8192];
	char szSmallPkgId[256];
	int ret = 0;
	BOOL bNull = FALSE;

	String2SqlValue(szSmallPkgId, sizeof(szSmallPkgId), pParam->m_szSmallPkgId);

	SNPRINTF( sql, sizeof(sql), " select a.id, a.package_id, a.big_pkg_id, c.stfname, a.procetime, b.out_target_type, e.stfname, d.stfname, b.operate_time, f.package_id"
		                        " from %s a left join %s b on a.id = b.package_id"
		                        " left join %s c on a.staff_id = c.stfid"
		                        " left join %s d on b.operator = d.stfid"
		                        " left join %s e on b.out_target_id = e.stfid"
								" left join %s f on a.big_pkg_id = f.id"
	                            " where (b.out_target_type is null or b.out_target_type = 0) and a.package_id like '%%%s%%'; ",
		                        SMALL_PKG_TABLE_NAME, SMALL_OUT_TABLE_NAME, STAFF_TABLE_NAME, STAFF_TABLE_NAME, STAFF_TABLE_NAME, BIG_PKG_TABLE_NAME, szSmallPkgId );
	try
	{
		OpenRecordSet(sql);

		while (MoveNext()) {

			PkgItem * pItem = new PkgItem;
			memset(pItem, 0, sizeof(PkgItem));

			memset(buf, 0, sizeof(buf));
			GetFieldValue(1, buf, sizeof(buf), &bNull);
			sscanf(buf, "%lu", &pItem->dwPkgId);

			GetFieldValue(2, pItem->szId, sizeof(pItem->szId), &bNull);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(3, buf, sizeof(buf), &bNull);
			sscanf(buf, "%lu", &pItem->dwParentPkgId);

			GetFieldValue(4, pItem->szInOperator, sizeof(pItem->szInOperator), &bNull);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(5, buf, sizeof(buf), &bNull);
			pItem->tInTime = String2DateTime(buf);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(6, buf, sizeof(buf), &bNull);
			if (bNull) {
				pItem->nOutTargetType = -1;
			}
			else {
				sscanf(buf, "%d", &pItem->nOutTargetType);
				pItem->dwStatus = PKG_STATUS_OUT;
			}

			GetFieldValue(7, pItem->szOutTargetName, sizeof(pItem->szOutTargetName), &bNull);

			GetFieldValue(8, pItem->szOutOperator, sizeof(pItem->szOutOperator), &bNull);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(9, buf, sizeof(buf), &bNull);
			pItem->tOutTime = String2DateTime(buf);

			GetFieldValue(10, pItem->szParentPkgId, sizeof(pItem->szParentPkgId), &bNull);

			vSmall.push_back(pItem);
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
		ClearVector(vSmall);
		return ret;
	}


	SNPRINTF(sql, sizeof(sql), " select a.id, a.package_id, a.big_pkg_id, c.stfname, a.procetime, b.out_target_type, e.name, d.stfname, b.operate_time, f.package_id"
		                       " from %s a inner join %s b on a.id = b.package_id"
		                       " inner join %s c on a.staff_id = c.stfid"
		                       " inner join %s d on b.operator = d.stfid"
		                       " inner join %s e on b.out_target_id = e.str_id"
		                       " left join %s f on a.big_pkg_id = f.id"
	                           " where ( b.out_target_type is null or b.out_target_type = 1) and a.package_id like '%%%s%%'; ",
		                       SMALL_PKG_TABLE_NAME, SMALL_OUT_TABLE_NAME, STAFF_TABLE_NAME, STAFF_TABLE_NAME, AGENCY_TABLE_NAME, BIG_PKG_TABLE_NAME, szSmallPkgId );
	try
	{
		OpenRecordSet(sql);

		while (MoveNext()) {

			PkgItem * pItem = new PkgItem;
			memset(pItem, 0, sizeof(PkgItem));

			memset(buf, 0, sizeof(buf));
			GetFieldValue(1, buf, sizeof(buf), &bNull);
			sscanf(buf, "%lu", &pItem->dwPkgId);

			GetFieldValue(2, pItem->szId, sizeof(pItem->szId), &bNull);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(3, buf, sizeof(buf), &bNull);
			sscanf(buf, "%lu", &pItem->dwParentPkgId);

			GetFieldValue(4, pItem->szInOperator, sizeof(pItem->szInOperator), &bNull);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(5, buf, sizeof(buf), &bNull);
			pItem->tInTime = String2DateTime(buf);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(6, buf, sizeof(buf), &bNull);
			sscanf(buf, "%d", &pItem->nOutTargetType);
			pItem->dwStatus = PKG_STATUS_OUT;

			GetFieldValue(7, pItem->szOutTargetName, sizeof(pItem->szOutTargetName), &bNull);

			GetFieldValue(8, pItem->szOutOperator, sizeof(pItem->szOutOperator), &bNull);

			memset(buf, 0, sizeof(buf));
			GetFieldValue(9, buf, sizeof(buf), &bNull);
			pItem->tOutTime = String2DateTime(buf);

			GetFieldValue(10, pItem->szParentPkgId, sizeof(pItem->szParentPkgId), &bNull);

			vSmall.push_back(pItem);
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
		ClearVector(vSmall);
		return ret;
	}

	std::vector<PkgItem*>::iterator it;
	for ( it = vSmall.begin(); it != vSmall.end(); ++it ) {
		PkgItem* pPkg = *it;

		// 如果小包装没有出库，查看其大包装有无出库
		if (pPkg->nOutTargetType == -1) {

			SNPRINTF(sql, sizeof(sql), " select a.out_target_type, b.stfname, c.stfname, a.operate_time"
				                       " from %s a inner join %s b on a.out_target_id = b.stfid"
				                       " inner join %s c on a.operator = c.stfid"
			                           " where a.out_target_type = 0 and a.package_id = %lu"
				                       " union"
				                       " select a.out_target_type, b.name, c.stfname, a.operate_time"
				                       " from %s a inner join %s b on a.out_target_id = b.str_id"
				                       " inner join %s c on a.operator = c.stfid"
			                           " where a.out_target_type = 1 and a.package_id = %lu; ", 
				                       BIG_OUT_TABLE_NAME, STAFF_TABLE_NAME, STAFF_TABLE_NAME, pPkg->dwParentPkgId,
				                       BIG_OUT_TABLE_NAME, AGENCY_TABLE_NAME, STAFF_TABLE_NAME, pPkg->dwParentPkgId);
			try
			{
				OpenRecordSet(sql);

				if (MoveNext()) {

					pPkg->dwStatus = PKG_STATUS_OUT;

					memset( buf, 0, sizeof(buf) );
					GetFieldValue( 1, buf, sizeof(buf), &bNull );
					sscanf( buf, "%lu", &pPkg->nOutTargetType );

					GetFieldValue(2, pPkg->szOutTargetName, sizeof(pPkg->szOutTargetName), &bNull);

					GetFieldValue(3, pPkg->szOutOperator, sizeof(pPkg->szOutOperator), &bNull);

					memset(buf, 0, sizeof(buf));
					GetFieldValue(4, buf, sizeof(buf), &bNull);
					pPkg->tOutTime = String2DateTime(buf);
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
				ClearVector(vSmall);
				return ret;
			}
		}
	}

	return 0;
}