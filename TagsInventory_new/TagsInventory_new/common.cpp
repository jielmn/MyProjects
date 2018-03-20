//#ifdef _DEBUG
//#include "vld.h"
//#endif
#include <afx.h>
#include "LmnString.h"
#include "InvCommon.h"




ILog    * g_log = 0;
IConfig * g_cfg = 0;
HWND    g_hWnd = 0;

LmnToolkits::Thread *  g_thrd_db = 0;                     // 数据库线程
LmnToolkits::Thread *  g_thrd_reader = 0;                 // Reader线程

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