//#ifdef _DEBUG
//#include "vld.h"
//#endif
#include <afx.h>
#include <assert.h>
#include "LmnString.h"
#include "InvCommon.h"




ILog    * g_log = 0;
IConfig * g_cfg = 0;
HWND    g_hWnd = 0;

LmnToolkits::Thread *  g_thrd_db = 0;                     // 数据库线程
LmnToolkits::Thread *  g_thrd_reader = 0;                 // Reader线程
LmnToolkits::Thread *  g_thrd_timer = 0;                  // TIMER

// 用于线程的参数
CTagItemParam::CTagItemParam(const TagItem * pItem) {
	if (0 != pItem) {
		memcpy(&m_item, pItem, sizeof(TagItem));
	}
	else {
		memset(&m_item, 0, sizeof(TagItem));
	}
}

CTagItemParam::~CTagItemParam() {

}

// 保存小盘点的参数
CInvSmallSaveParam::CInvSmallSaveParam() {

}

CInvSmallSaveParam::~CInvSmallSaveParam() {
	ClearVector(m_items);
}


// 保存大盘点的参数
CInvBigSaveParam::CInvBigSaveParam() {

}

CInvBigSaveParam::~CInvBigSaveParam() {
	ClearVector(m_items);
}


// 查询参数
CQueryParam::CQueryParam() {

}

CQueryParam::CQueryParam(time_t tStart, time_t tEnd, const char * szBatchId, const char * szOperator, int nQueryType) {
	m_tStart = tStart;
	m_tEnd = tEnd;

	if (szBatchId) {
		m_strBatchId = szBatchId;
	}

	if (szOperator) {
		m_strOperator = szOperator;
	}

	m_nQueryType = nQueryType;
}

CQueryParam::~CQueryParam() {

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

int CompTag(const TagItem * p1, const TagItem * p2) {
	if ( p1 == 0 ) {
		if (p2 == 0) {
			return 0;
		}
		else {
			return -1;
		}
	}
	else if (p2 == 0) {
		return 1;
	}
	else {
		if (p1->dwUidLen == p2->dwUidLen) {
			return memcmp(p1->abyUid, p2->abyUid, p1->dwUidLen);
		}
		else {
			DWORD dwMinLen = p1->dwUidLen < p2->dwUidLen ? p1->dwUidLen : p2->dwUidLen;
			int ret = memcmp(p1->abyUid, p2->abyUid, dwMinLen);

			if ( 0 == ret ) {
				if (p1->dwUidLen < p2->dwUidLen) {
					return -1;
				}
				else {
					return 1;
				}
			}
			else {
				return ret;
			}
		}
	}
}

// 从十六进制数获取Byte
static int _GetByte(char ch1, char ch2, BYTE & by) {
	ch1 = Char2Lower(ch1);
	if ( !((ch1 >= '0' && ch1 <= '9') || (ch1 >= 'a' && ch1 <= 'f')) ) {
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

// 把 e001020304050607 转化为 BYTE格式
// 把 e0-01-02-03-04-05-06-07 转化为 BYTE格式
TagItem * GetUid(TagItem * pTagItem, const char * szUid, BOOL bWithSplitChar /*= FALSE*/) {
	if ( 0 == pTagItem ) {
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
		ret = _GetByte( szUid[i], szUid[i+1], by );
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


time_t  ConvertDateTime(const SYSTEMTIME & t) {
	struct tm tmp;
	memset(&tmp, 0, sizeof(tmp));

	tmp.tm_year = t.wYear - 1900;
	tmp.tm_mon = t.wMonth - 1;
	tmp.tm_mday = t.wDay;

	return mktime(&tmp);
}


err_t rfid_inventory(RFID_READER_HANDLE hreader,
	BOOLEAN newAI,
	BYTE Antennas[],
	BYTE AntCount,
	BOOLEAN enISO15693,
	BOOLEAN enAFI,
	BYTE afi,
	BOOLEAN enISO14443A,
	BOOLEAN enISO18000p3m3,
	CB_TAG_REPORT_HANDLE tag_report_handler,
	void *cbParam,
	DWORD *tagCount)
{
	err_t iret = 0;
	DWORD totalCount;
	RFID_DN_HANDLE dnInvenParamList = RFID_HANDLE_NULL;
	/*
	*  Add air interface protocol
	*/
	dnInvenParamList = RDR_CreateInvenParamSpecList();
	if (dnInvenParamList) {
		if (enISO15693) {// enable iso15693 inventory
			ISO15693_CreateInvenParam(dnInvenParamList, 0, enAFI, afi, 0x00);
		}
		if (enISO14443A) { // enable iso14443a inentory
			ISO14443A_CreateInvenParam(dnInvenParamList, 0);
		}
		if (enISO18000p3m3) {// enable iso18000p3m3 inventory
			ISO18000p3m3_CreateInvenParam(dnInvenParamList, 0, 0, 0, ISO18000p6C_Dynamic_Q);
		}
	}
	totalCount = 0;

	//if AntCount =0  ,use default
label_inventory:
	iret = RDR_TagInventory(hreader, newAI, AntCount, Antennas, dnInvenParamList);
	if ((iret == NO_ERR) || (iret == -21)) {
		//ClearVector(g_tag_items);
		totalCount += RDR_GetTagDataReportCount(hreader);
		RFID_DN_HANDLE dnhReport = RDR_GetTagDataReport(hreader, RFID_SEEK_FIRST);
		while (dnhReport != RFID_HANDLE_NULL)
		{
			DWORD AIPtype, TagType, AntId;
			BYTE dsfid;
			BYTE uid[32];
			BYTE uidlen;
			/* Is iso15693 tag ? if parse ok ,process it */
			iret = ISO15693_ParseTagDataReport(dnhReport, &AIPtype, &TagType, &AntId, &dsfid, uid);
			if (iret == NO_ERR)
			{
				tag_report_handler(cbParam, hreader, AIPtype, TagType, AntId, uid, 8);
			}
			/* Is iso14443A tag ? if parse ok ,process it */
			iret = ISO14443A_ParseTagDataReport(dnhReport, &AIPtype, &TagType, &AntId, uid, &uidlen);
			if (iret == NO_ERR) {
				tag_report_handler(cbParam, hreader, AIPtype, TagType, AntId, uid, uidlen);
			}
			/* Is iso18000-3 mode 3 tag ? if parse ok ,process it */
			BYTE tagData[64];
			DWORD tagDataLen = sizeof(tagData);
			DWORD metaFlags = 0;
			iret = ISO18000p3m3_ParseTagDataReport(dnhReport, &AIPtype, &TagType, &AntId, &metaFlags, tagData, &tagDataLen);
			if (iret == NO_ERR) {
				tag_report_handler(cbParam, hreader, AIPtype, TagType, AntId, tagData, (WORD)tagDataLen);
			}

			dnhReport = RDR_GetTagDataReport(hreader, RFID_SEEK_NEXT);
		}
		if (iret == -21)
		{
			/*Stop trigger ocur at the reader .Maybe there are tags left for reading. */
			newAI = AI_TYPE_CONTINUE;
			goto label_inventory;
		}

		iret = 0;
	}
	else {
		/* error occur */
	}
	if (dnInvenParamList) DNODE_Destroy(dnInvenParamList);
	if (tagCount) *tagCount = totalCount;

#ifdef MY_DEBUG

#endif	

	return iret;
}