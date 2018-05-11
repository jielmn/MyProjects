#include <assert.h>
#include <time.h>
#include "InvOutCommon.h"
#include "LmnOdbc.h"
#include "InvOutDatabase.h"

ILog    * g_log = 0;
IConfig * g_cfg = 0;


char * MyEncrypt(const void * pSrc, DWORD dwSrcSize, char * dest, DWORD dwDestSize) {
	if (dest == 0 || dwDestSize == 0) {
		return 0;
	}

	if (pSrc == 0 || dwSrcSize == 0) {
		dest[0] = '\0';
		return dest;
	}

	char * pTmp = new char[dwSrcSize];
	if (0 == pTmp) {
		dest[0] = '\0';
		return dest;
	}

	memcpy(pTmp, pSrc, dwSrcSize);

	for (DWORD i = 0; i < dwSrcSize; i++) {
		pTmp[i] += 5;
	}

	int ret = EncodeBase64(dest, dwDestSize, pTmp, dwSrcSize);
	delete[] pTmp;

	if (LMNX_OK != ret) {
		dest[0] = '\0';
		return dest;
	}

	return dest;
}

int MyDecrypt(const char * szSrc, void * pDest, DWORD & dwDestSize) {
	if (pDest == 0 || dwDestSize == 0) {
		return 1;
	}

	if (szSrc == 0) {
		dwDestSize = 0;
		return 0;
	}

	DWORD dwSrcLen = strlen(szSrc);
	char * pTmp = new char[dwSrcLen];
	if (0 == pTmp) {
		return -1;
	}

	int ret = DecodeBase64(pTmp, &dwSrcLen, szSrc);
	if (LMNX_OK != ret) {
		delete[] pTmp;
		return 2;
	}

	if (dwSrcLen > dwDestSize) {
		delete[] pTmp;
		return 3;
	}

	char * dest = (char *)pDest;
	for (DWORD i = 0; i < dwSrcLen; i++) {
		dest[i] = pTmp[i] - 5;
	}
	dwDestSize = dwSrcLen;

	delete[] pTmp;
	return 0;
}

char * String2SqlValue (char * szDest, DWORD dwDestSize, const char * strValue ) {
	int ret = StrReplaceAll(szDest, dwDestSize - 1, strValue, "'", "''");
	if (0 == ret) {
		return szDest;
	}
	else {
		return 0;
	}
}

const char * GetErrorDescription(int ret) {
	const char * szRet = "";

	assert(0 != ret);

	switch (ret)
	{
	case CLmnOdbc::ERROR_CONNECTION_FAILED:
		szRet = "数据库连接失败";
		break;

	case CLmnOdbc::ERROR_DISCONNECTED:
		szRet = "数据库断开";
		break;

	case CLmnOdbc::ERROR_FAILED_TO_ALLOCATE_HANDLE :
		szRet = "数据库断开";
		break;

	case CLmnOdbc::ERROR_FAILED_TO_EXECUTE:
		szRet = "执行Sql失败";
		break;

	case CLmnOdbc::ERROR_FAILED_TO_FETCH:
		szRet = "MoveNext数据失败";
		break;

	case CLmnOdbc::ERROR_FAILED_TO_GET_DATA:
		szRet = "获取记录值失败";
		break;

	case CLmnOdbc::ERROR_RECORDSET_EOF:
		szRet = "已经达到记录集末尾";
		break;

	case CInvoutDatabase::InvOutDbErr_Integrity_constraint_violation:
		szRet = "违反唯一约束条件";
		break;

	case CInvoutDatabase::InvOutDbErr_Not_found:
		szRet = "记录不存在数据库";
		break;

	case CInvoutDatabase::InvOutDbErr_Cannt_delete_foreign_key:
		szRet = "由于外键约束，删除失败";
		break;

	case CInvoutDatabase::InvOutDbErr_Small_pkg_Parent_pkg_Already_out:
		szRet = "小包装的所在大包装已经出库";
		break;

	case -1:
		szRet = "未知错误";
		break;

	default:
		break;
	}

	return szRet;
}



CSaveInvOutParam::CSaveInvOutParam( int nTargetType, const DuiLib::CDuiString & strTargetId, const DuiLib::CDuiString & strOperatorId, 
	                                const std::vector<DuiLib::CDuiString *> & vBig, const std::vector<DuiLib::CDuiString *> & vSmall ) {
	m_nTargetType = nTargetType;
	m_strTargetId = strTargetId;
	m_strOperatorId = strOperatorId;

	std::vector<DuiLib::CDuiString *>::const_iterator it;
	for (it = vBig.begin(); it != vBig.end(); it++) {
		DuiLib::CDuiString * pStr = *it;
		DuiLib::CDuiString * pNewStr = new DuiLib::CDuiString(*pStr);
		m_vBig.push_back(pNewStr);
	}

	for (it = vSmall.begin(); it != vSmall.end(); it++) {
		DuiLib::CDuiString * pStr = *it;
		DuiLib::CDuiString * pNewStr = new DuiLib::CDuiString(*pStr);
		m_vSmall.push_back(pNewStr);
	}
}

CSaveInvOutParam::~CSaveInvOutParam() {
	ClearVector(m_vBig);
	ClearVector(m_vSmall);
}

char * DateTime2String(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%04d-%02d-%02d %02d:%02d:%02d", tmp.tm_year + 1900, tmp.tm_mon + 1, tmp.tm_mday, tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
	return szDest;
}

time_t  String2DateTime(const char * szDatetime) {
	struct tm  tmp;
	if (6 != sscanf_s(szDatetime, "%d-%d-%d %d:%d:%d", &tmp.tm_year, &tmp.tm_mon, &tmp.tm_mday, &tmp.tm_hour, &tmp.tm_min, &tmp.tm_sec)) {
		return 0;
	}

	tmp.tm_year -= 1900;
	tmp.tm_mon--;
	return mktime(&tmp);
}

time_t SystemTime2Time(const SYSTEMTIME & t) {
	struct tm tmTime;
	memset(&tmTime, 0, sizeof(tmTime));

	tmTime.tm_year = t.wYear - 1900;
	tmTime.tm_mon = t.wMonth - 1;
	tmTime.tm_mday = t.wDay;

	tmTime.tm_hour = t.wHour;
	tmTime.tm_min = t.wMinute;
	tmTime.tm_sec = t.wSecond;

	return mktime(&tmTime);
}

SYSTEMTIME Time2SystemTime(time_t  t) {
	SYSTEMTIME tSystemTime;
	memset(&tSystemTime, 0, sizeof(tSystemTime));

	struct tm * pTime = localtime(&t);
	if (pTime) {
		tSystemTime.wYear = pTime->tm_year + 1900;
		tSystemTime.wMonth = pTime->tm_mon + 1;
		tSystemTime.wDay = pTime->tm_mday;

		tSystemTime.wHour = pTime->tm_hour;
		tSystemTime.wMinute = pTime->tm_min;
		tSystemTime.wSecond = pTime->tm_sec;
	}

	return tSystemTime;
}