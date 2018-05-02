#include <assert.h>
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

	case -1:
		szRet = "未知错误";
		break;

	default:
		break;
	}

	return szRet;
}