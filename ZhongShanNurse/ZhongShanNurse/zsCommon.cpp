
#include "zsCommon.h"
#include <assert.h>
#include <odbcinst.h>
#include "LmnString.h"

ILog    * g_log = 0;
IConfig * g_cfg = 0;
HWND    g_hWnd = 0;

LmnToolkits::Thread *  g_thrd_db = 0;                     // 数据库线程
LmnToolkits::Thread *  g_thrd_reader = 0;                 // Reader线程
LmnToolkits::Thread *  g_thrd_timer = 0;                  // TIMER
LmnToolkits::Thread *  g_thrd_sync_reader = 0;

ReaderCmd  SYNC_COMMAND;
ReaderCmd  PREPARE_COMMAND;
ReaderCmd  CLEAR_COMMAND;


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


const char * GetErrDescription(int e) {
	const char * szDescription = 0;
	switch (e) {
	case 0:
		szDescription = "OK";
		break;

	case ZS_ERR_DB_CLOSE:
		szDescription = "连接数据库失败";
		break;

	case ZS_ERR_DB_NOT_UNIQUE:
		szDescription = "违反唯一约束";
		break;

	case ZS_ERR_DB_ERROR:
		szDescription = "数据库错误";
		break;
	
	case ZS_ERR_PATIENT_HAS_TEMP_DATA:
		szDescription = "病人已经有温度数据在数据库里，不能删除";
		break;

	case ZS_ERR_EXCEL_DRIVER_NOT_FOUND:
		szDescription = "没有找到Excel驱动";
		break;

	case ZS_ERR_PARTIALLY_FAILED_TO_IMPORT_EXCEL:
		szDescription = "导入病人信息部分失败";
		break;

	case ZS_ERR_PATIENT_HAS_TOO_MANY_TAGS:
		szDescription = "病人已经绑定太多Tags";
		break;

	case ZS_ERR_SYNC_READER_CLOSE:
		szDescription = "同步读卡器关闭状态";
		break;

	case ZS_ERR_SYNC_READER_FAILED_TO_WRITE:
		szDescription = "同步读卡器写数据失败";
		break;

	case ZS_ERR_SYNC_READER_FAILED_TO_RECEIVE_OR_WRONG_FORMAT:
		szDescription = "同步读卡器接收数据格式错误";
		break;

	default:
		szDescription = "未知错误";
	}
	return szDescription;
}

const char * GetGender(BOOL bFemale) {
	if (bFemale) {
		return "女";
	}
	else {
		return "男";
	}
}

// 是否女性
const BOOL  GetGender(const char * szGender) {
	if ( 0 == strcmp(szGender, "男") ) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}

const char * GetInHosStatus(BOOL bOutHos) {
	if (bOutHos) {
		return "出院";
	}
	else {
		return "住院";
	}
}

BOOL GetInHosStatus(const char * szStatus) {
	if (0 == strcmp(szStatus, "住院")) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}


CString GetExcelDriver()
{
	char szBuf[2001];
	WORD cbBufMax = 2000;
	WORD cbBufOut;
	char *pszBuf = szBuf;
	CString sDriver;

	//get driver name function(included in odbcinst.h)  
	if (!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
		return "";
	//check excel included or not  
	do
	{
		if (strstr(pszBuf, "Excel") != 0)
		{
			//found  
			sDriver = CString(pszBuf);
			break;
		}
		pszBuf = strchr(pszBuf, '\0') + 1;
	} while (pszBuf[1] != '\0');

	return sDriver;
}

BOOL  IsSameTag(const TagItem * p1, const TagItem * p2) {
	assert(p1 && p2);
	if ( (p1->dwUidLen == p2->dwUidLen) && (0 == memcmp(p1->abyUid, p2->abyUid, p1->dwUidLen)) ) {
		return TRUE;
	}

	return FALSE;
}


// 从十六进制数获取Byte
static int _GetByte(char ch1, char ch2, BYTE & by) {
	ch1 = Char2Lower(ch1);
	if (!((ch1 >= '0' && ch1 <= '9') || (ch1 >= 'a' && ch1 <= 'f'))) {
		return -1;
	}

	ch2 = Char2Lower(ch2);
	if (!((ch2 >= '0' && ch2 <= '9') || (ch2 >= 'a' && ch2 <= 'f'))) {
		return -1;
	}

	DWORD n1 = 0;
	DWORD n2 = 0;

	if (ch1 >= '0' && ch1 <= '9') {
		n1 = ch1 - '0';
	}
	else {
		n1 = ch1 - 'a' + 10;
	}

	if (ch2 >= '0' && ch2 <= '9') {
		n2 = ch2 - '0';
	}
	else {
		n2 = ch2 - 'a' + 10;
	}

	by = (BYTE)(n1 * 16 + n2);
	return 0;
}


char * GetUid(char * buf, DWORD dwBufLen, const BYTE uid[], DWORD uidlen, char chSplit /*= '\0'*/) {
	if (0 == buf || 0 == dwBufLen) {
		return 0;
	}

	if ((WORD)dwBufLen < uidlen * 3) {
		return 0;
	}

	if (0 == uidlen) {
		buf[0] = '\0';
		return buf;
	}

	if (chSplit != '\0') {
		for (WORD i = 0; i < uidlen; i++) {
			sprintf(buf + i * 3, "%02x", uid[i]);
			if (i < uidlen - 1) {
				buf[i * 3 + 2] = chSplit;
			}
		}
		buf[uidlen * 3] = '\0';
	}
	else {
		for (WORD i = 0; i < uidlen; i++) {
			sprintf(buf + i * 2, "%02x", uid[i]);
		}
		buf[uidlen * 2] = '\0';
	}


	return buf;
}

// 把 e001020304050607 转化为 BYTE格式
// 把 e0-01-02-03-04-05-06-07 转化为 BYTE格式
TagItem * GetUid(TagItem * pTagItem, const char * szUid, BOOL bWithSplitChar /*= FALSE*/) {
	if (0 == pTagItem) {
		return 0;
	}

	if (0 == szUid) {
		pTagItem->dwUidLen = 0;
		return pTagItem;
	}

	DWORD  len = strlen(szUid);
	if (!bWithSplitChar) {
		// 如果非偶数，错误格式
		if (len % 2 != 0) {
			return 0;
		}
	}
	else {
		// 错误格式
		if (len == 1) {
			return 0;
		}
		else if (len > 2) {
			// 错误格式
			if ((len - 2) % 3 != 0) {
				return 0;
			}
		}
	}

	int i = 0;
	BYTE by = 0;
	int ret = 0;
	pTagItem->dwUidLen = 0;

	while (i < (int)len) {
		ret = _GetByte(szUid[i], szUid[i + 1], by);
		if (0 == ret) {
			pTagItem->abyUid[pTagItem->dwUidLen] = by;
			pTagItem->dwUidLen++;
		}
		else {
			return 0;
		}

		if (bWithSplitChar) {
			i += 3;
		}
		else {
			i += 2;
		}
	}

	return pTagItem;
}

int TransferReaderCmd(ReaderCmd & cmd, const char * szCmd) {
	if (0 == szCmd) {
		return -1;
	}

	SplitString split;
	split.SplitByBlankChars(szCmd);
	DWORD dwSize = split.Size();
	if (dwSize >= sizeof(cmd.abyCommand)) {
		return -1;
	}

	for (DWORD i = 0; i < dwSize; i++) {
		int  tmp = 0;
		sscanf_s(split[i], "%x", &tmp);
		cmd.abyCommand[i] = (BYTE)tmp;
	}
	cmd.dwCommandLength = dwSize;

	return 0;
}

time_t   GetTelemedTagDate( const BYTE * pData, DWORD dwDataLen ) {
	assert(dwDataLen == 5);

	// 时间5个字节
	struct tm timeinfo;
	timeinfo.tm_year = 2000 + pData[0] - 1900;
	timeinfo.tm_mon  = pData[1] - 1;
	timeinfo.tm_mday = pData[2];
	timeinfo.tm_hour = pData[3];
	timeinfo.tm_min  = pData[4];
	timeinfo.tm_sec  = 0;
	return mktime(&timeinfo);
}

char * DateTime2Str (char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%04d-%02d-%02d %02d:%02d:%02d", tmp.tm_year + 1900, tmp.tm_mon + 1, tmp.tm_mday, tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
	return szDest;
}